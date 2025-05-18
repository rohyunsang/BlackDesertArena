// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlackDesertCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameplayAbilitySpec.h"
#include "GA/BDGA_BasicAttack.h" 
#include "GA/BDGA_ArcherAttack.h"
#include "Character/Attribute/BDAttributeSet.h"
#include "GameplayEffect.h"
#include "Blueprint/UserWidget.h"
#include "UI/BDHealthBarWidget.h"
#include "Component/BDInventoryComponent.h"
#include "Item/BDDropItem.h"
#include "Item/BDItemBase.h"
#include "UI/BDInventoryWidget.h"
#include "UI/BDInventorySlotWidget.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ABlackDesertCharacter

ABlackDesertCharacter::ABlackDesertCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Camera Boom 설정 
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 600.0f;
	CameraBoom->bUsePawnControlRotation = false; // 플레이어 회전에 따라 움직이지 않도록
	CameraBoom->bInheritPitch = false; // Pitch 회전을 상속받지 않음
	CameraBoom->bInheritYaw = false; // Yaw 회전도 상속받지 않도록 변경
	CameraBoom->bInheritRoll = false; // Roll 회전을 상속받지 않음
	CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
	// 초기 회전값 설정
	CameraBoom->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f));

	// Follow Camera 설정 
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)


	// Ability 
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	
	// Inventory
	Inventory = CreateDefaultSubobject<UBDInventoryComponent>(TEXT("InventoryComponent"));

	// Camera

}

UAbilitySystemComponent* ABlackDesertCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABlackDesertCharacter::BeginPlay()
{
	// Call the base class  
    Super::BeginPlay();

    // Add Input Mapping Context
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    // Initialize ability system
    if (AbilitySystemComponent)
    {
        // Setup actor info
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
        
        // Create and add attribute set
        UBDAttributeSet* Set = NewObject<UBDAttributeSet>(this);
        AbilitySystemComponent->AddAttributeSetSubobject(Set);
        AttributeSet = AbilitySystemComponent->GetSet<UBDAttributeSet>();

        // Bind attribute events
        if (Set)
        {
			Set->InitializeLevel(1);

            // Health change delegate
            Set->OnHealthChanged.AddDynamic(this, &ABlackDesertCharacter::HandleHealthChanged);
            
            // Level change delegate
            Set->OnLevelChanged.AddDynamic(this, &ABlackDesertCharacter::HandleLevelChanged);
        }

        // Initialize abilities
        InitializeAbilities();

        // Apply default attributes
        if (DefaultAttributeEffect)
        {
            FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
            EffectContext.AddSourceObject(this);
            FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1.f, EffectContext);
            if (SpecHandle.IsValid())
            {
                AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                
                // Log initial attribute values
                if (AttributeSet)
                {
                    UE_LOG(LogTemp, Log, TEXT("BD_LOG Character initialized with Level: %d, Health: %.1f/%.1f, XP: %.1f/%.1f"),
                        FMath::FloorToInt(AttributeSet->GetLevel()),
                        AttributeSet->GetHealth(),
                        AttributeSet->GetMaxHealth(),
                        AttributeSet->GetXP(),
                        AttributeSet->GetNextLevelXP());
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("BD_LOG DefaultAttributeEffect not set!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BD_LOG AbilitySystemComponent not found!"));
    }

	// Mobile Touch Input 
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}

	// UI 초기화는 다음 틱에 실행하여 모든 초기화가 완료된 후 실행되도록 함
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			APlayerController* PC = Cast<APlayerController>(GetController());
			if (PC && HealthBarWidgetClass)
			{
				UE_LOG(LogTemp, Log, TEXT("BD_LOG HealthBarWidgetClass"));
				HealthBarWidget = CreateWidget<UBDHealthBarWidget>(PC, HealthBarWidgetClass);
				if (HealthBarWidget)
				{
					UE_LOG(LogTemp, Log, TEXT("BD_LOG HealthBarWidget"));
					HealthBarWidget->AddToViewport();

					// Make sure AttributeSet is valid before using it
					if (AttributeSet)
					{
						UE_LOG(LogTemp, Log, TEXT("BD_LOG AttributeSet"));
						float HealthPercent = AttributeSet->GetHealth() / FMath::Max(1.0f, AttributeSet->GetMaxHealth());
						HealthBarWidget->SetHealthPercent(HealthPercent);
					}
				}
			}
		});


	// 미니맵 위젯 생성
	if (MiniMapWidgetClass)
	{
		MiniMapWidget = CreateWidget<UUserWidget>(GetWorld(), MiniMapWidgetClass);

		// 기본적으로는 미니맵 숨김 상태로 시작
		bIsMiniMapVisible = false;
	}
}

void ABlackDesertCharacter::InitializeAbilities()
{
	UE_LOG(LogTemp, Warning, TEXT("BD_LOG BlackDesertCharacter InitializeAbilities"));
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABlackDesertCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABlackDesertCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Started,   this, &ABlackDesertCharacter::LookStarted);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABlackDesertCharacter::Look);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Completed, this, &ABlackDesertCharacter::LookComplete);

		// Dash
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ABlackDesertCharacter::Dash);

		// Attacks
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ABlackDesertCharacter::Attack);
		EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Started, this, &ABlackDesertCharacter::PrimaryAttack);
		EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Started, this, &ABlackDesertCharacter::SecondaryAttack);
		EnhancedInputComponent->BindAction(TertiaryAction, ETriggerEvent::Started, this, &ABlackDesertCharacter::TertiaryAttack);
		EnhancedInputComponent->BindAction(UltimateAction, ETriggerEvent::Started, this, &ABlackDesertCharacter::UltimateAttack);

		// MiniMap
		EnhancedInputComponent->BindAction(MiniMapAction, ETriggerEvent::Started, this, &ABlackDesertCharacter::ToggleMiniMap);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ABlackDesertCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}
void ABlackDesertCharacter::LookStarted(const FInputActionValue& Value)
{

}
void ABlackDesertCharacter::Look(const FInputActionValue& Value)
{

}
void ABlackDesertCharacter::LookComplete(const FInputActionValue& Value)
{

}

void ABlackDesertCharacter::Dash()
{
	UE_LOG(LogTemp, Log, TEXT("BD_LOG Dash"));

	if (Controller)
	{
		FVector DashDirection = GetActorForwardVector();
		float DashDistance = 1200.f; // 원하는 거리 조절 가능
		FVector DashTarget = GetActorLocation() + DashDirection * DashDistance;

		LaunchCharacter(DashDirection * DashDistance, true, true);
	}
}

void ABlackDesertCharacter::Attack()
{
	if (AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG BlackDesertCharacter Attack()"));


		const FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(FName("Input.Action.Attack"));
		AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));
	}
}

void ABlackDesertCharacter::PrimaryAttack()
{
	UE_LOG(LogTemp, Log, TEXT("BD_LOG PrimaryAttack()"));

	if (AbilitySystemComponent)
	{
		const FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(FName("Input.Action.Primary"));
		AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));
	}
}

void ABlackDesertCharacter::SecondaryAttack()
{
	UE_LOG(LogTemp, Log, TEXT("BD_LOG SecondaryAttack()"));

	if (AbilitySystemComponent)
	{
		const FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(FName("Input.Action.Secondary"));
		AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));
	}
}

void ABlackDesertCharacter::TertiaryAttack()
{
	UE_LOG(LogTemp, Log, TEXT("BD_LOG TertiaryAttack()"));

	if (AbilitySystemComponent)
	{
		const FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(FName("Input.Action.Tertiary"));
		AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));
	}
}

void ABlackDesertCharacter::UltimateAttack()
{
	UE_LOG(LogTemp, Log, TEXT("BD_LOG UltimateAttack()"));

	if (AbilitySystemComponent)
	{
		const FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(FName("Input.Action.Ultimate"));
		AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));
	}
}


float ABlackDesertCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UE_LOG(LogTemp, Log, TEXT("BD_LOG %s Attacked! Damage: %.1f (by %s)"),
		*GetName(), DamageAmount, *GetNameSafe(DamageCauser));

	if (AbilitySystemComponent && DamageGameplayEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
		ContextHandle.AddSourceObject(DamageCauser);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DamageGameplayEffectClass, 1.f, ContextHandle);
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), -DamageAmount);
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	// 현재 체력 로그 출력
	if (const UBDAttributeSet* AttrSet = AbilitySystemComponent->GetSet<UBDAttributeSet>())
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG %s Current Health: %.1f"), *GetName(), AttrSet->GetHealth());
	}

	return ActualDamage;
}


// Add this new function to handle health changes
void ABlackDesertCharacter::HandleHealthChanged(float Health, float MaxHealth)
{
	if (HealthBarWidget)
	{
		float HealthPercent = Health / FMath::Max(1.0f, MaxHealth);
		HealthBarWidget->SetHealthPercent(HealthPercent);

		UE_LOG(LogTemp, Log, TEXT("BD_LOG Health Widget Updated: %.1f%% (%.1f/%.1f)"),
			HealthPercent * 100.0f, Health, MaxHealth);
	}
}
bool ABlackDesertCharacter::PickupItem(ABDDropItem* DroppedItem)
{
	if (!DroppedItem || !Inventory)
	{
		return false;
	}

	FString ItemID = DroppedItem->ItemID;
	int32 Quantity = DroppedItem->Quantity;

	// 인벤토리에 아이템 추가 (UI 업데이트는 컴포넌트에서 처리)
	EInventoryResult Result = Inventory->AddItemByID(ItemID, Quantity);
	if (Result == EInventoryResult::Success)
	{
		// 월드에서 아이템 제거
		DroppedItem->Destroy();
		return true;
	}
	else
	{
		// 실패 메시지 처리
		switch (Result)
		{
		case EInventoryResult::Failed_InventoryFull:
			UE_LOG(LogTemp, Warning, TEXT("BD_LOG Inventory is full"));
			break;
		case EInventoryResult::Failed_InvalidItem:
			UE_LOG(LogTemp, Warning, TEXT("BD_LOG Invalid item"));
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("BD_LOG Could not add item to inventory"));
			break;
		}

		return false;
	}
}



void ABlackDesertCharacter::ToggleInventory()
{
	if (Inventory)
	{
		Inventory->ToggleInventoryUI();
	}
}

void ABlackDesertCharacter::ShowMiniMap()
{
	if (MiniMapWidget && !bIsMiniMapVisible)
	{
		MiniMapWidget->AddToViewport();
		bIsMiniMapVisible = true;
	}
}

void ABlackDesertCharacter::HideMiniMap()
{
	if (MiniMapWidget && bIsMiniMapVisible)
	{
		MiniMapWidget->RemoveFromViewport();
		bIsMiniMapVisible = false;
	}
}

void ABlackDesertCharacter::ToggleMiniMap()
{
	if (bIsMiniMapVisible)
	{
		HideMiniMap();
	}
	else
	{
		ShowMiniMap();
	}
}

void ABlackDesertCharacter::HandleLevelChanged(int32 NewLevel, float CurrentXP, float RequiredXP)
{
	UE_LOG(LogTemp, Log, TEXT("BD_LOG %s Level Changed to %d! XP: %.1f/%.1f"),
		*GetName(), NewLevel, CurrentXP, RequiredXP);

	// You can add additional level-up effects here
	// For example, play a particle effect, sound, or animation

	// Update UI elements if needed
	// For example, if you have a level display UI
}

void ABlackDesertCharacter::AddExperience(float XPAmount)
{
	if (AbilitySystemComponent && ExpGameplayEffectClass && XPAmount > 0.0f)
	{
		FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
		ContextHandle.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(ExpGameplayEffectClass, 1.f, ContextHandle);
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.XP")), XPAmount);
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

			UE_LOG(LogTemp, Log, TEXT("BD_LOG %s gained %.1f XP"), *GetName(), XPAmount);
		}
	}
}

int32 ABlackDesertCharacter::GetCharacterLevel() const
{
	if (AttributeSet)
	{
		return FMath::FloorToInt(AttributeSet->GetLevel());
	}
	return 1; // Default level if attributes are not available
}

void ABlackDesertCharacter::GetExperienceInfo(float& OutCurrentXP, float& OutRequiredXP) const
{
	if (AttributeSet)
	{
		OutCurrentXP = AttributeSet->GetXP();
		OutRequiredXP = AttributeSet->GetNextLevelXP();
	}
	else
	{
		OutCurrentXP = 0.0f;
		OutRequiredXP = 0.0f;
	}
}