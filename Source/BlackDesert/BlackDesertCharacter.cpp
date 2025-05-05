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

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)


	// Ability 
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	


}

UAbilitySystemComponent* ABlackDesertCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABlackDesertCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Ability
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAbilities();

		UBDAttributeSet* Set = NewObject<UBDAttributeSet>(this);
		AbilitySystemComponent->AddAttributeSetSubobject(Set);
		AttributeSet = AbilitySystemComponent->GetSet<UBDAttributeSet>();

		// 델리게이트 바인딩 시 생성한 직접 참조를 사용
		if (Set)
		{
			Set->OnHealthChanged.AddDynamic(this, &ABlackDesertCharacter::HandleHealthChanged);
		}

		// BeginPlay 또는 적절한 초기화 위치에서
		if (DefaultAttributeEffect)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);
			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1.f, EffectContext);
			if (SpecHandle.IsValid())
			{
				AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
		UE_LOG(LogTemp, Log, TEXT("BD_LOG ASC "));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("BD_LOG NO ASC "));
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
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABlackDesertCharacter::Look);

		// Dash
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ABlackDesertCharacter::Dash);

		// Attacks
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ABlackDesertCharacter::Attack);
		EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Started, this, &ABlackDesertCharacter::PrimaryAttack);
		EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Started, this, &ABlackDesertCharacter::SecondaryAttack);
		EnhancedInputComponent->BindAction(UltimateAction, ETriggerEvent::Started, this, &ABlackDesertCharacter::UltimateAttack);
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

void ABlackDesertCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
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
