// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaaaster/Weapons/Weapon.h"
#include "Blaaaster/BlasterComponents/CombatComponent.h"
#include "BlasterAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	cameraBoom->SetupAttachment(GetMesh());
	cameraBoom->TargetArmLength = 600.f;
	cameraBoom->bUsePawnControlRotation = true;

	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);
	followCamera->bUsePawnControlRotation = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	overheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Overhead Widget"));
	overheadWidget->SetupAttachment(RootComponent);

	combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	combat->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	turningInPlace = ETurningInPlace::ETIP_NotTurning;

	NetUpdateFrequency = 100.f;
	MinNetUpdateFrequency = 33.f;
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

}
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimOffset(DeltaTime);
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABlasterCharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::EquipBtnPressed);
	PlayerInputComponent->BindAction("Crounch", IE_Pressed, this, &ABlasterCharacter::CrounchBtnPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimBtnPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimBtnReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABlasterCharacter::FireBtnPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABlasterCharacter::FireBtnReleased);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABlasterCharacter::LookUp);
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ABlasterCharacter, overlappingWeapon, COND_OwnerOnly);
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (combat)
	{
		combat->character = this;
	}
}

void ABlasterCharacter::MoveForward(float value)
{
	if (Controller != nullptr && value != .0f)
	{
		const FRotator yaw{ .0f, Controller->GetControlRotation().Yaw, .0f };
		const FVector dir{ FRotationMatrix(yaw).GetUnitAxis(EAxis::X) };
		AddMovementInput(dir, value);
	}
}

void ABlasterCharacter::MoveRight(float value)
{
	if (Controller != nullptr && value != .0f)
	{
		const FRotator yaw{ .0f, Controller->GetControlRotation().Yaw, .0f };
		const FVector dir{ FRotationMatrix(yaw).GetUnitAxis(EAxis::Y) };
		AddMovementInput(dir, value);
	}
}

void ABlasterCharacter::Turn(float value)
{
	AddControllerYawInput(value);
}

void ABlasterCharacter::LookUp(float value)
{
	AddControllerPitchInput(value);
}

void ABlasterCharacter::EquipBtnPressed()
{
	if (combat && overlappingWeapon)
	{
		if (HasAuthority())
		{
			combat->EquipWeapon(overlappingWeapon);
		}
		else {
			ServerEquipBtnPressed();
		}
	}
}


void ABlasterCharacter::ServerEquipBtnPressed_Implementation()
{
	if (combat)
	{
		combat->EquipWeapon(overlappingWeapon);
	}
}

void ABlasterCharacter::CrounchBtnPressed()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else {
		Crouch();
	}
}

void ABlasterCharacter::AimBtnPressed()
{
	if (combat)
	{
		combat->SetAiming(true);
	}
}


void ABlasterCharacter::AimBtnReleased()
{
	if (combat)
	{
		combat->SetAiming(false);
	}
}

void ABlasterCharacter::FireBtnPressed()
{
	if (combat)
	{
		combat->FireBtnPressed(true);
	}
}

void ABlasterCharacter::FireBtnReleased()
{
	if (combat)
	{
		combat->FireBtnPressed(false);
	}
}

void ABlasterCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	Super::Jump();
}

void ABlasterCharacter::PlayFireMontage(bool aiming)
{
	if (combat == nullptr || combat->equippedWeapon == nullptr || !fireWeaponMontage)
	{
		return;
	}
	UAnimInstance* animInstance{ GetMesh()->GetAnimInstance() };
	if (animInstance)
	{
		animInstance->Montage_Play(fireWeaponMontage);
		FName sectionName;
		sectionName = aiming ? "RifleAim" : "RifleHip";
		animInstance->Montage_JumpToSection(sectionName);
	}

}

void ABlasterCharacter::AimOffset(float deltaTime)
{
	if (combat && combat->equippedWeapon == nullptr)
	{
		return;
	}

	FVector velocity{ GetVelocity() };
	velocity.Z = 0.f;
	float speed{ StaticCast<float>(velocity.Size()) };
	bool isInAir{ GetCharacterMovement()->IsFalling() };
	// standing still
	if (speed <= .0f && !isInAir)
	{
		FRotator currentAimRotation{ FRotator(.0f, GetBaseAimRotation().Yaw, .0f) };
		FRotator deltaAimRotation{ UKismetMathLibrary::NormalizedDeltaRotator(currentAimRotation, startingAimRotation) };

		// the difference between camera rotation and character front rotation
		AO_Yaw = deltaAimRotation.Yaw;
		if (turningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			interpAO_Yaw = AO_Yaw;
		}
		// this sets if the camera should follow the character's facing direction
		bUseControllerRotationYaw = true;
		TurnInPlace(deltaTime);
	}
	// moving or jumbing
	else {
		startingAimRotation = FRotator(.0f, GetBaseAimRotation().Yaw, .0f);
		AO_Yaw = .0f;
		bUseControllerRotationYaw = true;
		turningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	AO_Pitch = GetBaseAimRotation().Pitch;
	// pitch correction for server packing, map pitch fro [270,360) to [-90, 0)
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		FVector2D inRange{ 270.f, 360.f };
		FVector2D outRange{ -90.f, 0.f };
		AO_Pitch = FMath::GetMappedRangeValueClamped(inRange, outRange, AO_Pitch);
	}
}

void ABlasterCharacter::TurnInPlace(float deltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("AO_Yaw: %f"), AO_Yaw);
	if (AO_Yaw > 90.f)
	{
		turningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		turningInPlace = ETurningInPlace::ETIP_Left;
	}

	if (turningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		interpAO_Yaw = FMath::FInterpTo(interpAO_Yaw, .0f, deltaTime, 3.f);
		AO_Yaw = interpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			turningInPlace = ETurningInPlace::ETIP_NotTurning;
			startingAimRotation = FRotator(.0f, GetBaseAimRotation().Yaw, .0f);
		}
	}

}

// this function is called only on the server, the new value will be replicated
// to the corresponding client
void ABlasterCharacter::SetOverlappingWeapon(AWeapon* w)
{
	if (overlappingWeapon)
	{
		overlappingWeapon->ShowPickupWidget(false);
	}

	overlappingWeapon = w;
	// we are on the server
	// and we are controlling the character
	if (IsLocallyControlled())
	{
		if (overlappingWeapon)
		{
			overlappingWeapon->ShowPickupWidget(true);
		}
	}
}


// this function is only called on the target client when the new value of 
// weapon is being replicated
// so we can have the logic here to shows the ui only on the client side
void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* lastWeapon)
{
	overlappingWeapon ? overlappingWeapon->ShowPickupWidget(true) : lastWeapon->ShowPickupWidget(false);
}


// getters and setters

bool ABlasterCharacter::IsWeaponEquipped()
{
	return combat && combat->equippedWeapon;
}

bool ABlasterCharacter::IsAiming()
{
	return combat && combat->isAiming;
}

const AWeapon* ABlasterCharacter::GetEquippedWeapon() const
{
	if (combat == nullptr)
	{
		return nullptr;
	}
	return combat->equippedWeapon;
}
