

#include "CombatComponent.h"
#include "CombatComponent.h"
#include "Blaaaster/Weapons/Weapon.h"
#include "Blaaaster/Character/BlasterCharacter.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h" 
#include "Net/UnrealNetwork.h" 
#include "GameFramework/CharacterMovementComponent.h" 

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	baseWalkSpeed = 600.f;
	aimWalkSpeed = 450.f;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (character)
	{
		character->GetCharacterMovement()->MaxWalkSpeed = baseWalkSpeed;
	}
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent, equippedWeapon);
	DOREPLIFETIME(UCombatComponent, isAiming);
}

void UCombatComponent::EquipWeapon(AWeapon* weapon)
{
	if (character == nullptr || weapon == nullptr)
	{
		return;
	}

	equippedWeapon = weapon;
	equippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* handSocket = character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (handSocket)
	{
		handSocket->AttachActor(equippedWeapon, character->GetMesh());
	}
	equippedWeapon->SetOwner(character);
	character->GetCharacterMovement()->bOrientRotationToMovement = false;
	character->bUseControllerRotationYaw = true;
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (equippedWeapon && character)
	{
		character->GetCharacterMovement()->bOrientRotationToMovement = false;
		character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::SetAiming(bool aiming)
{
	isAiming = aiming;
	ServerSetAiming(aiming);
	if (character)
	{
		character->GetCharacterMovement()->MaxWalkSpeed = aiming ? aimWalkSpeed : baseWalkSpeed;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool aiming)
{
	isAiming = aiming;
	if (character)
	{
		character->GetCharacterMovement()->MaxWalkSpeed = aiming ? aimWalkSpeed : baseWalkSpeed;
	}
}

