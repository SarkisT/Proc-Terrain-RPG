// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PTRPGCharacter.h"
#include "FireBall.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// APTRPGCharacter

APTRPGCharacter::APTRPGCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.0f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	Wand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wand"));
	Wand->SetupAttachment(FirstPersonCameraComponent);
	Wand->RelativeLocation = FVector(114.514038, 81.390511, -207.057617);
	Wand->RelativeRotation = FRotator(-87.199997, -49.714008, 49.004013);
	

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	HP = maxHP;
	MP = maxMP;

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void APTRPGCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bLaser) {
		DestroyBlock();
	}

	if (bIsHealing && MP >= 15) {
		MP -= 15;
		if (HP < maxHP) {
			HP += 1;
		}
		if (HP > maxHP) {
			HP = maxHP;
		}
	}

	if (bSpeedBuffed && MP >= 1) {
		MP -= 1;
	}
	

	if (MP < maxMP) {
		MP += 1;
	}
}

void APTRPGCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void APTRPGCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//Bind Attack Rotation
	PlayerInputComponent->BindAction("RotateAttack", IE_Pressed, this, &APTRPGCharacter::ChangeAttack);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APTRPGCharacter::OnFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APTRPGCharacter::StopLaser);

	//Bind Sprint
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APTRPGCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APTRPGCharacter::StopSpeed);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &APTRPGCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &APTRPGCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APTRPGCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APTRPGCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APTRPGCharacter::LookUpAtRate);
}

void APTRPGCharacter::OnFire()
{

	switch (AttackIndex) {
	case 0://Fire Ball

		if (MP >= 100) {

			MP -= 100;

			if (ProjectileClass != NULL)
			{
				UWorld* const World = GetWorld();
				if (World != NULL)
				{
					if (bUsingMotionControllers)
					{
						const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
						const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
						World->SpawnActor<AFireBall>(ProjectileClass, SpawnLocation, SpawnRotation);
					}
					else
					{
						const FRotator SpawnRotation = GetControlRotation();
						// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
						const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

						//Set Spawn Collision Handling Override
						FActorSpawnParameters ActorSpawnParams;
						ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

						// spawn the projectile at the muzzle
						World->SpawnActor<AFireBall>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
					}
				}
			}

			// try and play the sound if specified
			if (FireSound != NULL)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}
		}

		break;
	case 1://Laser Beam
		if (MP >= 3) {
			bLaser = true;
			bCanRotateAttack = false;
		}
		break;
	case 2://Speed Buff

		/*
		if (MP >= 300 && !bSpeedBuffed) {

			MP -= 300;

			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("SUCCESS")));

			GetCharacterMovement()->MaxWalkSpeed = 10000;

			bSpeedBuffed = true;

			GetWorld()->GetTimerManager().SetTimer(SpeedTimer, this, &APTRPGCharacter::StopSpeed, 10.0f, false);
		}
		*/

		break;
	case 3://Heal

		if (MP >= 15) {
			bIsHealing = true;
			bCanRotateAttack = false;
		}

		break;
	}
	
	/*
	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
	*/
}

void APTRPGCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void APTRPGCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void APTRPGCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void APTRPGCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void APTRPGCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APTRPGCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void APTRPGCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APTRPGCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool APTRPGCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &APTRPGCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &APTRPGCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &APTRPGCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

void APTRPGCharacter::DestroyBlock()
{

	if (MP >= 3) {

		MP -= 3;

		FHitResult Hit;

		TArray<FHitResult> Hits;




		//GetWorldLocation for component
		Start = Wand->GetComponentToWorld().GetLocation();

		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Silver, FString::Printf(TEXT("Initial Block Location is : %s "), *Start.ToString()));

		End = ((FirstPersonCameraComponent->GetForwardVector() * 100000.0f) + Start);
		FCollisionQueryParams TraceParams;

		TraceParams.AddIgnoredActor(this);


		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Silver, FString::Printf(TEXT("Initial Block Location is : %s "), *End.ToString()));

		//Channel2 is the CustomOverlap Trace, (Overlaps All)
		//bool bHit = GetWorld()->LineTraceMultiByChannel(Hits, Start, End, ECC_GameTraceChannel2, TraceParams);


		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);

		//GetWorld().linetrac

		FTransform InstanceTransform;


		/*
		if (bHit) {

			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("SUCCESS")));

			for (FHitResult h : Hits) {
				auto tempComponent = Cast<UInstancedStaticMeshComponent>(h.Component);

				auto tempTransform = tempComponent->GetInstanceTransform(h.Item, InstanceTransform, false);

				//tempComponent->AddInstance(FTransform(FVector(InstanceTransform.GetLocation().X, InstanceTransform.GetLocation().Y, InstanceTransform.GetLocation().Z - 250.0f)));

				tempComponent->RemoveInstance(h.Item);

			}

		}
		*/


		if (bHit) {
			
			if (UInstancedStaticMeshComponent* tempComponent = Cast<UInstancedStaticMeshComponent>(Hit.Component)) {
				tempComponent = Cast<UInstancedStaticMeshComponent>(Hit.Component);

				GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, FString::Printf(TEXT("ye")));

				if (tempComponent != nullptr || tempComponent != NULL) {
					auto tempTransform = tempComponent->GetInstanceTransform(Hit.Item, InstanceTransform, false);

					//tempComponent->AddInstance(FTransform(FVector(InstanceTransform.GetLocation().X, InstanceTransform.GetLocation().Y, InstanceTransform.GetLocation().Z - 250.0f)));

					tempComponent->RemoveInstance(Hit.Item);
				}
			}
		}
	

	}
	else {
		bLaser = false;
		bCanRotateAttack = true;
	}

}

void APTRPGCharacter::ChangeAttack()
{
	if (bCanRotateAttack) {
		if (AttackIndex > 4) {
			AttackIndex = 0;
		}
		else {
			AttackIndex++;
		}
	}

}

void APTRPGCharacter::StopLaser()
{
	bLaser = false;
	bCanRotateAttack = true;
	bIsHealing = false;
}

void APTRPGCharacter::StopSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = 2500;
	bSpeedBuffed = false;


}

void APTRPGCharacter::Sprint()
{
	if (!bSpeedBuffed && MP >= 1) {

		GetCharacterMovement()->MaxWalkSpeed = 6500;

		bSpeedBuffed = true;
	}
}
