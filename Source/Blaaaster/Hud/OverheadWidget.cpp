// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"

void UOverheadWidget::SetDisplayText(FString text)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(text));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* inPawn)
{
	ENetRole localRole = inPawn->GetLocalRole();
	FString role;
	switch (localRole)
	{
	case ROLE_None:
		role = FString("None");
		break;
	case ROLE_SimulatedProxy:
		role = FString("Simulated Proxy");
		break;
	case ROLE_AutonomousProxy:
		role = FString("Autonomous Proxy");
		break;
	case ROLE_Authority:
		role = FString("Authority");
		break;
	case ROLE_MAX:
		role = FString("Max");
		break;
	default:
		break;
	}
	FString localRoleStr = FString::Printf(TEXT("Local Role: %s"), *role);
	DisplayText->SetText(FText::FromString(localRoleStr));
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}
