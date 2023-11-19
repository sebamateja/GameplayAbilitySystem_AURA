#include "Player/AuraPlayerController.h"

#include "Interaction/TargetInterface.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AAuraPlayerController::AAuraPlayerController()
{
    bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    CursorTrace();
}

void AAuraPlayerController::CursorTrace()
{
    FHitResult CursorHit;
    GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
    if (!CursorHit.bBlockingHit) return;

    LastActor = ThisActor;
    ThisActor = Cast<ITargetInterface>(CursorHit.GetActor());

    /**
     * Line trace from cursor. There are several scenarios:
     * A. LastActor is null && ThisActor is null
     *      - Do nothing
     * B. LastActor is null && ThisActor is valid
     *      - Highlight ThisActor
     * C. LastActor is valid && ThisActor is null
     *      - UnHighlight LastActor
     * D. Both actors are valid, but LastActor != ThisActor
     *      - UnHighlight LastActor
     *      - Highlight ThisActor
     * E. Both actors are valid, and are the same actor
     *      - Do nothing
     */

    // CASE A and E - Both actors are the same, either null or valid - do nothing
    if (LastActor == ThisActor)
    {
        return;
    }
    // CASE C
    if (LastActor && ThisActor == nullptr)
    {
        LastActor->UnHighlightActor();
        return;
    }
    // CASE B
    if (LastActor == nullptr && ThisActor)
    {
        ThisActor->HighlightActor();
        return;
    }
    // CASE D
    if (LastActor != ThisActor)
    {
        LastActor->UnHighlightActor();
        ThisActor->HighlightActor();
    }
}

void AAuraPlayerController::BeginPlay()
{
    Super::BeginPlay();

    check(AuraContext);

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    if (Subsystem)
    {
        Subsystem->AddMappingContext(AuraContext, 0); // 0 -> Priority, we can have multiple MappingContexts
    }

    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;

    FInputModeGameAndUI InputModeData;
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputModeData.SetHideCursorDuringCapture(false);
    SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

// It is called every frame
void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
    const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
    const FRotator Rotation = GetControlRotation();
    const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    // It might be called to early to have ControlledPawn valid,
    // so it is better to check it with if statement
    if (APawn* ControlledPawn = GetPawn<APawn>())
    {
        ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
        ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
    }
}