// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaaaster/Weapons/Weapon.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	blasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float deltaTime)
{
	Super::NativeUpdateAnimation(deltaTime);
	if (blasterCharacter == nullptr)
	{
		blasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
	if (blasterCharacter == nullptr)
	{
		return;
	}
	FVector velocity = blasterCharacter->GetVelocity();
	velocity.Z = .0f;
	speed = velocity.Size();

	isInAir = blasterCharacter->GetCharacterMovement()->IsFalling();

	isAccelerating = blasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0;
	weaponEquipped = blasterCharacter->IsWeaponEquipped();
	const AWeapon* equippedWeapon = blasterCharacter->GetEquippedWeapon();
	isCrouched = blasterCharacter->bIsCrouched;
	isAiming = blasterCharacter->IsAiming();

	// Note we can have the camera point in one direction and the character facing in a different direction than the camera
	// baseAimRotation is the Rotation for the camera in global space
	FRotator aimRotation = blasterCharacter->GetBaseAimRotation();
	// MovementRotation is the Rotation for the character in global space
	FRotator movementRotation = UKismetMathLibrary::MakeRotFromX(blasterCharacter->GetVelocity());
	FRotator deltaRot = UKismetMathLibrary::NormalizedDeltaRotator(movementRotation, aimRotation);
	deltaRotation = FMath::RInterpTo(deltaRotation, deltaRot, deltaTime, 6.f);
	YawOffset = deltaRotation.Yaw;

	characterRotationLastFrame = characterRotation;
	characterRotation = blasterCharacter->GetActorRotation();

	const FRotator delta = UKismetMathLibrary::NormalizedDeltaRotator(characterRotation, characterRotationLastFrame);
	const float target = delta.Yaw / deltaTime;
	const float interp = FMath::FInterpTo(Lean, target, deltaTime, 6.f);
	Lean = FMath::Clamp(interp, -90.f, 90.f);
	AO_Yaw = blasterCharacter->GetAOYaw();
	AO_Pitch = blasterCharacter->GetAOPitch();

	if (weaponEquipped && equippedWeapon->GetWeaponMesh() && blasterCharacter->GetMesh())
	{
		leftHandTransform = equippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector outPos;
		FRotator outRot;
		blasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), leftHandTransform.GetLocation(), FRotator::ZeroRotator, outPos, outRot);

		leftHandTransform.SetLocation(outPos);
		leftHandTransform.SetRotation(FQuat(outRot));
	}
}
