#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLAAASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

		friend class ABlasterCharacter;
public:
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

protected:
	virtual void BeginPlay() override;

protected:
	// combat related
	void TraceUnderCrosshairs(FHitResult& outResult);
public:
	void EquipWeapon(class AWeapon* weapon);
	UFUNCTION()
		void OnRep_EquippedWeapon();
	void SetAiming(bool aiming);
	UFUNCTION(Server, Reliable)
		void ServerSetAiming(bool aiming);

	void FireBtnPressed(bool isPress);
	UFUNCTION(Server, Reliable)
		void ServerFire();
	UFUNCTION(NetMulticast, Reliable)
		void MulticastFire();


private:
	class ABlasterCharacter* character;
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
		class AWeapon* equippedWeapon;
	UPROPERTY(Replicated)
		bool isAiming;

	UPROPERTY(EditAnywhere)
		float baseWalkSpeed;
	UPROPERTY(EditAnywhere)
		float aimWalkSpeed;
	bool fireBtnPressed;
};
