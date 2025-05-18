// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "UI/BDHealthBarWidget.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"
#include "Component/BDInventoryComponent.h"
#include "BlackDesertCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ABlackDesertCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;

	/** Basic Attack Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PrimaryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SecondaryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* TertiaryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* UltimateAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MiniMapAction;


public:
	ABlackDesertCharacter();

public:
	// GAS Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;


protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void LookStarted(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void LookComplete(const FInputActionValue& Value);

public:
	UFUNCTION(BlueprintCallable)
	void Dash();
	UFUNCTION(BlueprintCallable)
	void Attack();
	UFUNCTION(BlueprintCallable)
	void PrimaryAttack();
	UFUNCTION(BlueprintCallable)
	void SecondaryAttack();
	UFUNCTION(BlueprintCallable)
	void TertiaryAttack();
	UFUNCTION(BlueprintCallable)
	void UltimateAttack();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void InitializeAbilities();  // Give Ability 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS|Attributes", meta = (AllowPrivateAccess = "true"))
	const class UBDAttributeSet* AttributeSet;

	// Attribute 초기값 세팅용 GameplayEffect 클래스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS|Attributes")
	TSubclassOf<UGameplayEffect> DefaultAttributeEffect;

	// Damage Effect 
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass;

public:
	// Exp Effect 
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> ExpGameplayEffectClass;

	
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;



public:
	//UI
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBDHealthBarWidget> HealthBarWidgetClass;
	
	UPROPERTY()
	UBDHealthBarWidget* HealthBarWidget;
		// Add this function declaration



public:
	// Inventory
	// 인벤토리 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBDInventoryComponent* Inventory;

	// 아이템 획득 함수
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool PickupItem(class ABDDropItem* DroppedItem);


	// 인벤토리 토글 함수
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ToggleInventory();




protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MiniMapWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MiniMap")
	class UUserWidget* MiniMapWidget;

	bool bIsMiniMapVisible;

public:
	// 미니맵 켜기/끄기 함수
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMiniMap();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideMiniMap();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ToggleMiniMap();


public:
	// Attribute event handlers
	UFUNCTION()
	void HandleHealthChanged(float Health, float MaxHealth);

	UFUNCTION()
	void HandleLevelChanged(int32 NewLevel, float CurrentXP, float RequiredXP);

	UFUNCTION()
	void AddExperience(float XPAmount);

	// Getter for current character level
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	int32 GetCharacterLevel() const;

	// Getter for current XP information
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void GetExperienceInfo(float& OutCurrentXP, float& OutRequiredXP) const;


};

