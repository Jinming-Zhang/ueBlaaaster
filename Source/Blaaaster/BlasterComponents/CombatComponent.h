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
public:
	void EquipWeapon(class AWeapon* weapon);
protected:
	virtual void BeginPlay() override;

private:
	class ABlasterCharacter* character;
	UPROPERTY(Replicated)
		class AWeapon* equippedWeapon;
	UPROPERTY(Replicated)
		bool isAiming;
};
