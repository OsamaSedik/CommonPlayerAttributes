// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonPlayerAttributes/Public/PlayerAttributeComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"


// Sets default values for this component's properties
UPlayerAttributeComponent::UPlayerAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UPlayerAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	CharacterOwnerRef = UGameplayStatics::GetPlayerCharacter(this,0);
	OnHealthChange.Broadcast(CurrentHealth);
	if (CharacterOwnerRef.IsValid())
	{
		CharacterOwnerRef.Get()->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void UPlayerAttributeComponent::AddHealth(const float Value)
{
	if (!CanHeal()) return;
	CurrentHealth = FMath::Clamp(CurrentHealth + Value,0.0f,MaxHealth);
	if (CurrentHealth > 20.0f)
	{
		bInjured = false;
		bCanSprint = true;
	}
	OnHealthChange.Broadcast(CurrentHealth);
}

void UPlayerAttributeComponent::UpgradePlayerHealth(const float Value)
{
	MaxHealth = Value;
	CurrentHealth = MaxHealth;
	OnHealthChange.Broadcast(CurrentHealth);
}

void UPlayerAttributeComponent::Add_XP(const int32 Value)
{
	Player_XP += Value;
}

void UPlayerAttributeComponent::Add_Tokens(const int32 Value)
{
	Player_Tokens += Value;
	OnTokensAdded.Broadcast(Player_Tokens);
}

int32 UPlayerAttributeComponent::GetTotalTokens() const
{
	return Player_Tokens;
}

void UPlayerAttributeComponent::LevelUpPlayer()
{
	if (Player_XP >= Player_Max_XP)
	{
		Player_XP -= Player_Max_XP;
		++Player_Level;
		Player_Max_XP = Player_Max_XP * Player_Level;
		OnPlayerLevelUp.Broadcast(Player_Level);
	}
}

int32 UPlayerAttributeComponent::GetPlayerLevel() const
{
	return Player_Level;
}

float UPlayerAttributeComponent::GetPlayerProgress() const
{
	if (Player_Max_XP <= 0) return 0.0f;
	return static_cast<float>(Player_XP) / static_cast<float>(Player_Max_XP);
}

void UPlayerAttributeComponent::TakeDamage(const float Value, const AActor* Causer)
{
	if (CurrentHealth > 0)
	{
		CurrentHealth = FMath::Clamp((CurrentHealth - Value) ,0.0f,MaxHealth);
		OnHealthChange.Broadcast(CurrentHealth);
		PlayDamageRumbleEffect();
		if (CurrentHealth <= 20.0f)
		{
			bInjured = true;
			bCanSprint = false;
		}
		if (CurrentHealth <= 0.0f)
		{
			bCanMove = false;
			bIsAlive = false;
			CharacterOwnerRef.Get()->GetController()->SetIgnoreLookInput(true);
			CharacterOwnerRef.Get()->GetController()->SetIgnoreMoveInput(true);
			OnPlayerDeath.Broadcast();
		}
	}
	
	if (CharacterOwnerRef.IsValid() && Montage_Hits.Num() > 0)
	{
		if (CharacterOwnerRef.Get()->GetMesh() && CharacterOwnerRef.Get()->GetMesh()->GetAnimInstance())
		{
			const int32 AttackIndex = UKismetMathLibrary::RandomInteger(Montage_Hits.Num());
			CharacterOwnerRef.Get()->GetMesh()->GetAnimInstance()->Montage_Play(Montage_Hits[AttackIndex]);
		}
	}
}

void UPlayerAttributeComponent::SpawnBloodEffect(const FVector HitLocation, const FRotator HitRotation)
{
	if (!VFX_BloodEffect)return;
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,VFX_BloodEffect,HitLocation,
		HitRotation,FVector::One()*2.0f,true,true,ENCPoolMethod::AutoRelease,true);
}

float UPlayerAttributeComponent::GetMaxHealth() const
{
	return MaxHealth;
}

float UPlayerAttributeComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

float UPlayerAttributeComponent::GetHealthNormalized() const
{
	return CurrentHealth / MaxHealth;
}

bool UPlayerAttributeComponent::CanHeal() const
{
	return CurrentHealth < MaxHealth &&  CharacterOwnerRef.IsValid() && !CharacterOwnerRef.Get()->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying();
}

void UPlayerAttributeComponent::UseStamina()
{
	OnToggleSprint.Broadcast(true);
	if (CurrentStamina > 0)
	{
		bSprint = true;
		SetPlayerMovementState(EMovementSpeed::EMS_Sprint);
		GetWorld()->GetTimerManager().SetTimer(TH_StartStamina,this,&UPlayerAttributeComponent::DrainStaminaBySprint,StaminaDrainRate,true,1.0f);
	}
}


void UPlayerAttributeComponent::DrainStaminaBySprint()
{
	CurrentStamina = FMath::Clamp(CurrentStamina - StaminaDrainAmount,0.0f,MaxStamina);
	OnStaminaChange.Broadcast(CurrentStamina/MaxStamina);
	if (CurrentStamina <= 0)
	{
		bSprint = false;
		MovementState = EMovementSpeed::EMS_Walk;
		SetPlayerMovementState(EMovementSpeed::EMS_Walk);
		GetWorld()->GetTimerManager().ClearTimer(TH_StartStamina);
	}
	//UE_LOG(LogTemp,Warning,TEXT("Stamina %f"),CurrentStamina)
}

void UPlayerAttributeComponent::ReleaseStamina()
{
	bSprint = false;
	SetPlayerMovementState(EMovementSpeed::EMS_Walk);
	GetWorld()->GetTimerManager().ClearTimer(TH_StartStamina);
	if (!GetWorld()->GetTimerManager().IsTimerActive(TH_EndStamina))
	{
		GetWorld()->GetTimerManager().SetTimer(TH_EndStamina,this,&UPlayerAttributeComponent::RecoverStaminaFromSprint,StaminaFillRate,true,1.0f);
	}
}

void UPlayerAttributeComponent::RecoverStaminaFromSprint()
{
	CurrentStamina = FMath::Clamp(CurrentStamina + StaminaFillAmount,0.0f,MaxStamina);
	OnStaminaChange.Broadcast(CurrentStamina/MaxStamina);
	if (CurrentStamina >= MaxStamina)
	{
		GetWorld()->GetTimerManager().ClearTimer(TH_EndStamina);
		OnToggleSprint.Broadcast(false);
	}
	//UE_LOG(LogTemp,Warning,TEXT("Stamina %f"),CurrentStamina)
}



float UPlayerAttributeComponent::GetMaxStamina() const
{
	return MaxStamina;
}

float UPlayerAttributeComponent::GetCurrentStamina() const
{
	return CurrentStamina;
}

bool UPlayerAttributeComponent::IsAlive() const
{
	return CurrentHealth > 0.0f && bIsAlive;
}

void UPlayerAttributeComponent::TogglePlayerMovement(const bool In_CanMove)
{
	bCanMove = In_CanMove;
}

void UPlayerAttributeComponent::SetPlayerMovementState(const EMovementSpeed& State)
{
	if (CharacterOwnerRef.IsValid())
	{
		switch (State)
		{
		case EMovementSpeed::EMS_SlowWalk:
			CharacterOwnerRef.Get()->GetCharacterMovement()->MaxWalkSpeed = WalkSlowSpeed;
			break;
		case EMovementSpeed::EMS_Walk:
			CharacterOwnerRef.Get()->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
			break;
		case EMovementSpeed::EMS_Crouch:
			CharacterOwnerRef.Get()->GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
			break;
		case EMovementSpeed::EMS_Sprint:
			CharacterOwnerRef.Get()->GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
			break;
		default: ;
		}
	}
}

EMovementSpeed UPlayerAttributeComponent::GetPlayerMovementState() const
{
	return MovementState;
}

bool UPlayerAttributeComponent::CanMove() const
{
	return bCanMove && bIsAlive;
}

void UPlayerAttributeComponent::ChangeOverlayState(const EOverlayState State)
{
	OverlayState = State;
	OnOverlayStateChange.Broadcast(OverlayState);
}

EOverlayState UPlayerAttributeComponent::GetOverlayState() const
{
	return OverlayState;
}

bool UPlayerAttributeComponent::IsOverlayMatch(const EOverlayState State)
{
	return OverlayState == State;
}

void UPlayerAttributeComponent::DrainStaminaByMeleeAttack(const float Amount)
{
	CurrentStamina-=Amount;
	if (!GetWorld()->GetTimerManager().IsTimerActive(TH_RecoverStaminaFromMelee))
	{
		GetWorld()->GetTimerManager().SetTimer(TH_RecoverStaminaFromMelee,this,&UPlayerAttributeComponent::RecoverStaminaFromMeleeAttack,
			RecoverTimeFromMelee,false,1.0f);
	}
}

void UPlayerAttributeComponent::RecoverStaminaFromMeleeAttack()
{
	CurrentStamina = MaxStamina;
}

void UPlayerAttributeComponent::ToggleSprintAbility(const bool bToggle)
{
	bCanSprint = bToggle;
}

void UPlayerAttributeComponent::PlayDamageRumbleEffect()
{
	if (!DamageRumbleEffect) return;
	if (APlayerController* PC = (UGameplayStatics::GetPlayerController(GetWorld(),0)))
	{
		PC->ClientPlayForceFeedback(DamageRumbleEffect);
	}
}



