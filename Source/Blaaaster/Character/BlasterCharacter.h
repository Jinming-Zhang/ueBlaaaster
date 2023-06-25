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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void MoveForward(float value);
	virtual void MoveRight(float value);
	virtual void Turn(float value);
	virtual void LookUp(float value);

private:
	UPROPERTY(EditAnywhere)
		class USpringArmComponent* cameraBoom;
	UPROPERTY(VisibleAnywhere)
		class UCameraComponent* followCamera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* overheadWidget;

};
