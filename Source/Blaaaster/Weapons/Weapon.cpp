// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blaaaster/Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	// mesh
	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	SetRootComponent(mesh);

	mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// sphere collider
	// disable collisions by default 
	// ONLY ENABLE ON THE SERVER INSTANCE by checking authority
	areaSphereRange = CreateDefaultSubobject<USphereComponent>(TEXT("Overlap Sphere"));
	areaSphereRange->SetupAttachment(RootComponent);
	areaSphereRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	areaSphereRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	pickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pickup Widget"));
	pickupWidget->SetupAttachment(RootComponent);
	pickupWidget->SetVisibility(false);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	// enable collision only on server, either condition check are fine
	if (GetLocalRole() == ENetRole::ROLE_Authority || HasAuthority())
	{
		areaSphereRange->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		areaSphereRange->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		areaSphereRange->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		areaSphereRange->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}
	pickupWidget->SetVisibility(false);
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME_CONDITION(AWeapon, weaponState)
	DOREPLIFETIME(AWeapon, weaponState);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool fromSweep, const FHitResult& sweepResult)
{
	if (ABlasterCharacter* bCharacter = Cast<ABlasterCharacter>(otherActor)) {
		bCharacter->SetOverlappingWeapon(this);
	}
}

// only called on server
void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABlasterCharacter* bCharacter = Cast<ABlasterCharacter>(OtherActor)) {
		bCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::SetWeaponState(EWeaponState state)
{
	weaponState = state;
	switch (weaponState)
	{
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		areaSphereRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		break;
	case EWeaponState::EWS_Max:
		break;
	default:
		break;
	}

}

void AWeapon::OnRep_WeaponState()
{
	switch (weaponState)
	{
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		areaSphereRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		break;
	case EWeaponState::EWS_Max:
		break;
	default:
		break;
	}
}


void AWeapon::ShowPickupWidget(bool show)
{
	if (pickupWidget != nullptr)
	{
		pickupWidget->SetVisibility(show);
	}
}

