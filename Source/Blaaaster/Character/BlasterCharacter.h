// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void MoveForward(float value);
	virtual void MoveRight(float value);
	virtual void Turn(float value);
	virtual void LookUp(float value);
	virtual void EquipBtnPressed();
	virtual void CrounchBtnPressed();
	virtual void AimBtnPressed();
	virtual void AimBtnReleased();

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
	UFUNCTION(Server, Reliable)
		void ServerAimBtnPressed();
	UFUNCTION(Server, Reliable)
		void ServerAimBtnReleased();

public:
	// getters and setters
	void SetOverlappingWeapon(AWeapon* w);
	bool IsWeaponEquipped();
	bool IsAiming();
};
