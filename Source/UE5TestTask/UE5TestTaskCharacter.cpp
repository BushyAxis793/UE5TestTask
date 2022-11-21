// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5TestTaskCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// AUE5TestTaskCharacter

AUE5TestTaskCharacter::AUE5TestTaskCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	

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

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
	FPSCamera->SetupAttachment(GetMesh(), FName("Head"));
	FPSCamera->bUsePawnControlRotation = true;

	Gun = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gun"));
	Gun->SetupAttachment(GetMesh(), FName("spine_05"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AUE5TestTaskCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("SwitchPlayerCamera", IE_Pressed, this, &AUE5TestTaskCharacter::SwitchPlayerCamera);
	PlayerInputComponent->BindAction("SwitchPlayerCamera", IE_Released, this, &AUE5TestTaskCharacter::StopSwitchingPlayerCamera);

	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &AUE5TestTaskCharacter::Shoot);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AUE5TestTaskCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AUE5TestTaskCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AUE5TestTaskCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AUE5TestTaskCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AUE5TestTaskCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AUE5TestTaskCharacter::TouchStopped);
}

void AUE5TestTaskCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AUE5TestTaskCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AUE5TestTaskCharacter::SwitchPlayerCamera()
{
	SetFPSMode(true);

	FollowCamera->SetActive(false);
	FPSCamera->SetActive(true);
	bUseControllerRotationYaw = true;

}

void AUE5TestTaskCharacter::StopSwitchingPlayerCamera()
{
	SetFPSMode(false);
	FPSCamera->SetActive(false);
	FollowCamera->SetActive(true);
	bUseControllerRotationYaw = false;
}

void AUE5TestTaskCharacter::Shoot()
{
	FHitResult OutHit;
	FVector StartPosition = Gun->GetComponentLocation();

	FVector ForwardVector = FollowCamera->GetForwardVector();
	FVector EndPosition = ((ForwardVector * 1000.f) + StartPosition);

	FCollisionQueryParams CollisionParams;

	DrawDebugLine(GetWorld(), StartPosition, EndPosition, FColor::Green, true);

	bool isHit = GetWorld()->LineTraceSingleByChannel(OutHit, StartPosition, EndPosition, ECC_Visibility, CollisionParams);

	

	if (isHit)
	{
		if (OutHit.bBlockingHit)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("You are hitting: %s"), *OutHit.GetActor()->GetName()));
		}
	}

		APlayerController* PlayerController = Cast<APlayerController>(GetController());

		FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		FVector SpawnLocation = Gun->GetComponentLocation();

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		GetWorld()->SpawnActor<ABullet>(SpawnLocation, SpawnRotation, ActorSpawnParams);

		
}



void AUE5TestTaskCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AUE5TestTaskCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AUE5TestTaskCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AUE5TestTaskCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

int AUE5TestTaskCharacter::GetPoints()
{
	return Points;
}
void AUE5TestTaskCharacter::AddPoints(int value)
{
	Points += value;
}

bool AUE5TestTaskCharacter::GetFPSMode()
{
	return bFPSMode;
}
void AUE5TestTaskCharacter::SetFPSMode(bool value)
{
	bFPSMode = value;
}
