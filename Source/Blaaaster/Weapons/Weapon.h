// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState :uint8 {
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_Max UMETA(DisplayName = "Default Max"),
};

UCLASS()
class BLAAASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
		friend class UCombatComponent;

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
public:
	void ShowPickupWidget(bool show);
	virtual void Fire();

protected:
	virtual void BeginPlay() override;
	// this function only calls on server
	UFUNCTION()
		virtual void OnSphereOverlap(
			UPrimitiveComponent* overlappedComponent,
			AActor* otherActor,
			UPrimitiveComponent* otherComp,
			int32 otherBodyIndex,
			bool fromSweep,
			const FHitResult& sweepResult);

	UFUNCTION()
		virtual void OnSphereEndOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex
		);


private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		USkeletalMeshComponent* mesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		class USphereComponent* areaSphereRange;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
		EWeaponState weaponState;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		class UWidgetComponent* pickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		class UAnimationAsset* fireAnimation;
private:
	UFUNCTION()
		void OnRep_WeaponState();
public:
	void SetWeaponState(EWeaponState state);
public:
	// getters
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh()const { return mesh; }

};
