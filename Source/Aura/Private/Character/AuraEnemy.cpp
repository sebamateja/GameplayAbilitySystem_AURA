#include "Character/AuraEnemy.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "UI/Widget/AuraUserWidget.h"
#include "AuraGameplayTags.h"
#include "Aura/Aura.h"
#include "AI/AuraAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Components/WidgetComponent.h"

AAuraEnemy::AAuraEnemy()
{
    GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bUseControllerDesiredRotation = true;
    
    AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
    HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
    HealthBar->SetupAttachment(GetRootComponent());
}

void AAuraEnemy::BeginPlay()
{
    Super::BeginPlay();
    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

    InitAbilityActorInfo();
    if (HasAuthority())
    {
        UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
    }

    if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
    {
        AuraUserWidget->SetWidgetController(this);
    }

    if (const UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AttributeSet))
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda(
            [this](const FOnAttributeChangeData& Data){
                OnHealthChanged.Broadcast(Data.NewValue);
            }
        );
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).AddLambda(
            [this](const FOnAttributeChangeData& Data){
                OnMaxHealthChanged.Broadcast(Data.NewValue);
            }
        );
        AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
            this,
            &AAuraEnemy::HitReactTagChanged
        );

        // Broadcasting initial values
        OnHealthChanged.Broadcast(AuraAS->GetHealth());
        OnMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth());
    }
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (!HasAuthority()) return;
    AuraAIController = Cast<AAuraAIController>(NewController);

    AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
    AuraAIController->RunBehaviorTree(BehaviorTree);
    AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
    AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);
}

void AAuraEnemy::HighlightActor()
{
    GetMesh()->SetRenderCustomDepth(true);
    GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

    Weapon->SetRenderCustomDepth(true);
    Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHighlightActor()
{
    GetMesh()->SetRenderCustomDepth(false);
    Weapon->SetRenderCustomDepth(false);
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
    CombatTarget = InCombatTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation() const
{
    return CombatTarget;
}

int32 AAuraEnemy::GetPlayerLevel()
{
    return Level;
}

void AAuraEnemy::Die()
{
    SetLifeSpan(LifeSpan);
    if(AuraAIController) AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
    Super::Die();
}

void AAuraEnemy::InitAbilityActorInfo()
{
    AbilitySystemComponent->InitAbilityActorInfo(this, this);
    Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

    if (HasAuthority())
    {
        InitializeDefaultAttributes();
    }
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
    UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    bHitReacting = NewCount > 0;
    GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.0f : BaseWalkSpeed;

    if (AuraAIController && AuraAIController->GetBlackboardComponent())
    {
        AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
    }
}
