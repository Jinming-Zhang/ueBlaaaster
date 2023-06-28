// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaaaster/Weapons/Weapon.h"
#include "Blaaaster/BlasterComponents/CombatComponent.h"

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
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

}
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABlasterCharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::EquipBtnPressed);
	PlayerInputComponent->BindAction("Crounch", IE_Pressed, this, &ABlasterCharacter::CrounchBtnPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimBtnPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimBtnReleased);

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
		if (HasAuthority())
		{
			combat->isAiming = true;
		}
		else {
			ServerAimBtnPressed();
		}
	}
}

void ABlasterCharacter::ServerAimBtnPressed_Implementation()
{
	if (combat)
	{
		combat->isAiming = true;
	}
}

void ABlasterCharacter::AimBtnReleased()
{
	if (combat)
	{
		if (HasAuthority())
		{
			combat->isAiming = false;
		}
		else {
			ServerAimBtnReleased();
		}
	}
}

void ABlasterCharacter::ServerAimBtnReleased_Implementation()
{
	if (combat)
	{
		combat->isAiming = false;
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
