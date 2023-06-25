// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
}
