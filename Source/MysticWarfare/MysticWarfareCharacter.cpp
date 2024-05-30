#include "MysticWarfareCharacter.h"
#include "MysticWarfareProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Abilities/AbilityComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Health/HealthComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AMysticWarfareCharacter::AMysticWarfareCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	//FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetupAttachment(GetMesh(), "Head");
	//FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->SetRelativeLocation(FVector(25.f, 0.f, 8.f)); // Position the camera
	FirstPersonCameraComponent->SetRelativeRotation(FRotator(155.0f, 66.0f, -55.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	// Ability Component
	AbilityComponent = CreateDefaultSubobject<UAbilityComponent>(TEXT("Ability Component"));

	// Health Component
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	
	// set crouching to true
	CrouchEyeOffset = FVector(0.f);
	CrouchSpeed = 12.f;
}

void AMysticWarfareCharacter::BeginPlay()
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

}

void AMysticWarfareCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	const float CrouchInterpTime = FMath::Min(1.f, CrouchSpeed * DeltaTime);
	CrouchEyeOffset = (1.f - CrouchInterpTime) * CrouchEyeOffset;
}

void AMysticWarfareCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (HalfHeightAdjust == 0)
		return;

	const float StartBaseEyeHeight = BaseEyeHeight;
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CrouchEyeOffset.Z += StartBaseEyeHeight - BaseEyeHeight + HalfHeightAdjust;

	FirstPersonCameraComponent->AddWorldOffset(FVector(0.f, 0.f, -HalfHeightAdjust), false);
}

void AMysticWarfareCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (HalfHeightAdjust == 0)
		return;

	const float StartBaseEyeHeight = BaseEyeHeight;
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CrouchEyeOffset.Z += StartBaseEyeHeight - BaseEyeHeight - HalfHeightAdjust;

	FirstPersonCameraComponent->AddWorldOffset(FVector(0.f, 0.f, HalfHeightAdjust), false);
}

void AMysticWarfareCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	if (FirstPersonCameraComponent)
	{
		FirstPersonCameraComponent->GetCameraView(DeltaTime, OutResult);
		OutResult.Location += CrouchEyeOffset;
	}
}

void AMysticWarfareCharacter::UseAbility(int32 index)
{
	AbilityComponent->ActivateAbility(index);
}

template <int32 index>
void AMysticWarfareCharacter::UseAbility()
{
	UseAbility(index);
}

void AMysticWarfareCharacter::StartCrouching(const FInputActionValue& Value)
{
	Crouch();
}

void AMysticWarfareCharacter::StopCrouching(const FInputActionValue& Value)
{
	UnCrouch();
}

bool AMysticWarfareCharacter::CanJumpInternal_Implementation() const
{
	// be sure you check all
	bool v = JumpIsAllowedInternal();

	// we can
	if (v)
		return true;

	// allow jumping while crouching
	return bIsCrouched && !GetCharacterMovement()->IsFalling();
}

void AMysticWarfareCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMysticWarfareCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMysticWarfareCharacter::Look);
		
		// Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AMysticWarfareCharacter::StartCrouching);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AMysticWarfareCharacter::StopCrouching);

		// Abilities
		// bind first ability (if exists)
		if(AbilitiesActions.Num() > 0)
			EnhancedInputComponent->BindAction(AbilitiesActions[0], ETriggerEvent::Started,
				this, &AMysticWarfareCharacter::UseAbility<0>);

		// bind scnd ability (if exists)
		if(AbilitiesActions.Num() > 1)
			EnhancedInputComponent->BindAction(AbilitiesActions[1], ETriggerEvent::Started,
				this, &AMysticWarfareCharacter::UseAbility<1>);

		// bind third ability (if exists)
		if(AbilitiesActions.Num() > 2)
			EnhancedInputComponent->BindAction(AbilitiesActions[2], ETriggerEvent::Started,
				this, &AMysticWarfareCharacter::UseAbility<2>);

		// if more abilities bind here... 
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AMysticWarfareCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AMysticWarfareCharacter::Look(const FInputActionValue& Value)
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

void AMysticWarfareCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AMysticWarfareCharacter::GetHasRifle()
{
	return bHasRifle;
}