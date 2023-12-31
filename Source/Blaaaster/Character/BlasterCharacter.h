// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaaaster/BlasterTypes/TurningInPlace.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLAAASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
	virtual void PostInitializeComponents()override;

protected:
	// character controls
	virtual void BeginPlay() override;
	virtual void MoveForward(float value);
	virtual void MoveRight(float value);
	virtual void Turn(float value);
	virtual void LookUp(float value);
	virtual void EquipBtnPressed();
	virtual void CrounchBtnPressed();
	virtual void AimBtnPressed();
	virtual void AimBtnReleased();
	virtual void FireBtnPressed();
	virtual void FireBtnReleased();
	virtual void Jump()override;

public:
	// animation properties
	void PlayFireMontage(bool aiming);
protected:
	void AimOffset(float deltaTime);
	float AO_Yaw;
	float interpAO_Yaw;
	float AO_Pitch;
	FRotator startingAimRotation;
	ETurningInPlace turningInPlace;

	void TurnInPlace(float deltaTime);
	UPROPERTY(EditAnywhere, Category = Combat)
		class UAnimMontage* fireWeaponMontage;


private:
	UPROPERTY(EditAnywhere)
		class USpringArmComponent* cameraBoom;
	UPROPERTY(VisibleAnywhere)
		class UCameraComponent* followCamera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* overheadWidget;
	UPROPERTY(VisibleAnywhere)
		class UCombatComponent* combat;

private:
	// replicated variables
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
		class AWeapon* overlappingWeapon;
	UFUNCTION()
		void OnRep_OverlappingWeapon(AWeapon* lastWeapon);

	UFUNCTION(Server, Reliable)
		void ServerEquipBtnPressed();

public:
	// getters and setters
	void SetOverlappingWeapon(AWeapon* w);
	bool IsWeaponEquipped();
	bool IsAiming();
	const AWeapon* GetEquippedWeapon()const;
	FORCEINLINE float GetAOYaw() { return AO_Yaw; }
	FORCEINLINE float GetAOPitch() { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace()const { return turningInPlace; }
};
