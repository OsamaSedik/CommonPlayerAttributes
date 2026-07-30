// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAttributeComponent.generated.h"

// enum state for player movement speed 
UENUM(Blueprintable,BlueprintType)
enum class EMovementSpeed : uint8
{
	EMS_SlowWalk	UMETA(DisplayName = "Slow Walk"),
	EMS_Walk		UMETA(DisplayName = "Walk"),
	EMS_Sprint		UMETA(DisplayName = "Sprint"),
	EMS_Crouch		UMETA(DisplayName = "Crouch")
};

// enum state for overlay -> used to update the anim blueprint 
UENUM(Blueprintable,BlueprintType)
enum class EOverlayState : uint8
{
	EOS_Unarmed							UMETA(DisplayName = "UnArmed"),
	EOS_Melee							UMETA(DisplayName = "Melee"),
	EOS_Pistol							UMETA(DisplayName = "Pistol"),
	EOS_Shotgun							UMETA(DisplayName = "Shotgun"),
	EOS_Rifle							UMETA(DisplayName = "Rifle"),
};

// dynamic delegate callbacks 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChange, float, Health);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChange,float ,Stamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToggleSprint,bool ,Toggle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTokensAdded,int32,TokensAdded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLevelUp,int32,Level);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOverlayStateChange,EOverlayState,OverlayState);

class UAnimMontage;
class UForceFeedbackEffect;
class ACharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMMONPLAYERATTRIBUTES_API UPlayerAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerAttributeComponent();

	// maximum health for the player  
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---")
	float MaxHealth = 100.0f;

	// set current health to max health 
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---")
	float CurrentHealth = MaxHealth;

	// is player alive? -> used for anim blueprint 
	UPROPERTY(BlueprintReadWrite,Category="--- Player Attribute Component ---")
	bool bIsAlive = true;
	
	// is player injured? -> used for anim blueprint 
	UPROPERTY(BlueprintReadWrite,Category="--- Player Attribute Component ---")
	bool bInjured = false;
	
	// maximum stamina 
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---")
	float MaxStamina = 100.0f;
	
	// set current stamina to max stamina 
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---")
	float CurrentStamina = MaxStamina;

	// how frequency stamina drain per second 
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---")
	float StaminaDrainRate = 0.2f;

	// how much stamina drain amount by the drain rate 
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---")
	float StaminaDrainAmount = 5.0f;

	// how frequency stamina fill per second 
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---")
	float StaminaFillRate = 0.1f;
	
	// time to recover stamina from melee attack 
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---")
	float RecoverTimeFromMelee = 4.0f;
	
	// how much stamina amount to fill per second 
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---")
	float StaminaFillAmount = 10.0f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---")
	float SprintSpeed = 450.0f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---")
	float WalkSpeed = 200.0f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---")
	float WalkSlowSpeed = 120.0f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---")
	float CrouchSpeed = 100.0f;
	
	// how much time to start moving -> smooth movement used with player input movement if player press longer than this value 
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---")
	float DelayMovementTime = 0.2f;

	// can player sprint ? -> master toggle on / off sprint -> used to turn off the sprint in some area of the level 
	UPROPERTY(BlueprintReadWrite,Category="--- Player Attribute Component ---")
	bool bCanSprint = true;
	
	// is player sprinting? -> used for anim blueprint 
	UPROPERTY(BlueprintReadWrite,Category="--- Player Attribute Component ---")
	bool bSprint = false;
	
	// player xp 
	UPROPERTY(BlueprintReadWrite,Category="--- Player Attribute Component ---")
	int32 Player_XP = 0;
	
	// player max xp -> used to compare with xp to level up
	UPROPERTY(BlueprintReadWrite,Category="--- Player Attribute Component ---")
	int32 Player_Max_XP = 500;
	
	// player level progression 
	UPROPERTY(BlueprintReadWrite,Category="--- Player Attribute Component ---")
	int32 Player_Level = 1;
	
	// player tokens -> used to purchase stuff or abilities in the game 
	UPROPERTY(BlueprintReadWrite,Category="--- Player Attribute Component ---")
	int32 Player_Tokens = 0;
	
	// current player movement state 
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---")
	EMovementSpeed MovementState = EMovementSpeed::EMS_Walk;
	
	// can player move or not 
	UPROPERTY(BlueprintReadWrite,Category="--- Player Attribute Component ---")
	bool bCanMove = true;
	
	// current player overlay state -> used for anim blueprint 
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---")
	EOverlayState OverlayState = EOverlayState::EOS_Unarmed;
	
	// an array of anim montages used when player gets damage 
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---",DisplayName=" Hit Montages ")
	TArray<UAnimMontage*> Montage_Hits;
	
	// blood effect if player gets damage 
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---",DisplayName="Hit Blood Effect")
	class UNiagaraSystem* VFX_BloodEffect;
	
	// rumble effect for gamepad vibration 
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="--- Player Attribute Component ---")
	UForceFeedbackEffect* DamageRumbleEffect;
	
	// timers 
	FTimerHandle TH_StartStamina;
	FTimerHandle TH_EndStamina;
	FTimerHandle TH_RecoverStaminaFromMelee;
	
	// reference to the owing player 
	UPROPERTY()
	TWeakObjectPtr<ACharacter> CharacterOwnerRef;
	
	// delegate to broadcast current health -> used for UI
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FOnHealthChange OnHealthChange;

	// delegate to broadcast player death -> used for UI and other like enemies 
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FOnPlayerDeath OnPlayerDeath;

	// delegate to broadcast current stamina -> used for UI
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FOnStaminaChange OnStaminaChange;

	// delegate to broadcast whatever the player is sprinting or not -> used for UI
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FOnToggleSprint OnToggleSprint;
	
	// delegate to broadcast current tokens -> used for UI
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FOnTokensAdded OnTokensAdded;
	
	// delegate to broadcast current player level -> used for UI
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FOnPlayerLevelUp OnPlayerLevelUp;
	
	// delegate to broadcast current overlay -> used for anim blueprint 
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FOnOverlayStateChange OnOverlayStateChange;

	// add health 
	UFUNCTION(BlueprintCallable,Category="Player Attribute Component")
	void AddHealth(const float Value);
	
	// check if player can heal 
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Player Attribute Component")
	bool CanHeal()const;
	
	// gets the maximum health 
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Player Attribute Component")
	float GetMaxHealth()const;

	// gets the current health 
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Player Attribute Component")
	float GetCurrentHealth()const;
	
	// get current health normalized between 0 and 1 -> used for UI 
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Player Attribute Component")
	float GetHealthNormalized()const;
	
	// upgrade the current and max health 
	UFUNCTION(BlueprintCallable,Category="Player Attribute Component")
	void UpgradePlayerHealth(const float Value);
	
	// adding xp
	UFUNCTION(BlueprintCallable,Category="Player Attribute Component")
	void Add_XP(const int32 Value);
	
	// add tokens 
	UFUNCTION(BlueprintCallable,Category="Player Attribute Component")
	void Add_Tokens(const int32 Value);
	
	// get total player tokens 
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Player Attribute Component")
	int32 GetTotalTokens()const;
	
	// increase player level state 
	UFUNCTION(BlueprintCallable,Category="Player Attribute Component")
	void LevelUpPlayer();
	
	// gets the current player level 
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Player Attribute Component")
	int32 GetPlayerLevel()const;
	
	// get player progression by dividing the xp / max xp -> used for UI 
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Player Attribute Component")
	float GetPlayerProgress()const;
	
	// handle damage 
	UFUNCTION(BlueprintCallable,Category="Player Attribute Component")
	void TakeDamage(const float Value , const AActor* Causer);

	// spawn blood damage effect 
	UFUNCTION(BlueprintCallable,Category="Player Attribute Component")
	void SpawnBloodEffect(const FVector HitLocation , const FRotator HitRotation);
	
	// start to use the stamina called when sprinting 
	UFUNCTION(BlueprintCallable,Category="Player Attribute Component")
	void UseStamina();

	// release stamina called when end sprinting 
	UFUNCTION(BlueprintCallable,Category="Player Attribute Component")
	void ReleaseStamina();
	
	// recovering the stamina by timer to full when ReleaseStamina is called 
	UFUNCTION(BlueprintCallable,Category="Player Attribute Component")
	void RecoverStaminaFromSprint();

	// drain out stamina amount called by timer throw the use stamina 
	UFUNCTION(BlueprintCallable,Category="Player Attribute Component")
	void DrainStaminaBySprint();
	
	// gets the maximum stamina 
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Player Attribute Component")
	float GetMaxStamina()const;

	// gets the current stamina 
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Player Attribute Component")
	float GetCurrentStamina()const;

	// is player alive 
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Player Attribute Component")
	bool IsAlive()const;
	
	// pause or unpause player movement in some cases 
	UFUNCTION(BlueprintCallable,Category="Player Attribute Component")
	void TogglePlayerMovement(const bool In_CanMove);
	
	// update the player movement state that change the movement speed 
	UFUNCTION(BlueprintCallable,Category="Player Attribute Component")
	void SetPlayerMovementState(const EMovementSpeed& State);

	// gets the current movement state 
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Player Attribute Component")
	EMovementSpeed GetPlayerMovementState()const;

	// can player move -> used when player tries to move 
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Player Attribute Component")
	bool CanMove()const;
	
	// update the overlay state 
	UFUNCTION(BlueprintCallable,Category="Player Attribute Component")
	void ChangeOverlayState(const EOverlayState State);

	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Player Attribute Component")
	EOverlayState GetOverlayState()const;

	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Player Attribute Component")
	bool IsOverlayMatch(const EOverlayState State);
	
	// drain stamina if player perform melee attacks 
	UFUNCTION(BlueprintCallable,Category="Player Behavior Component")
	void DrainStaminaByMeleeAttack(const float Amount);

	// recover the stamina to full called by timer throw DrainStaminaByMeleeAttack
	UFUNCTION(BlueprintCallable,Category="Player Behavior Component")
	void RecoverStaminaFromMeleeAttack();

	// toggles on / off the ability to sprint 
	UFUNCTION(BlueprintCallable,Category="Player Behavior Component")
	void ToggleSprintAbility(const bool bToggle);

	// play gamepad rumble effect on damage 
	UFUNCTION(BlueprintCallable,Category="Player Behavior Component")
	void PlayDamageRumbleEffect();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
