// Fill out your copyright notice in the Description page of Project Settings.


#include "WolfMultiplayerSessionsMenu.h"
#include "Components/Button.h"
#include "OnlineSubsystem.h"
#include "WolfMultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"

void UWolfMultiplayerSessionsMenu::MenuSetup(int32 numberOfPublicConnections, FString typeOfMatch, FString lobbyPath) {
	NumPublicConnections = numberOfPublicConnections;
	MatchType = typeOfMatch;
	PathToLobby = FString::Printf(TEXT("%s?listen"), *lobbyPath);

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	if (UWorld * world{ GetWorld() }) {
		if (APlayerController * pController{ world->GetFirstPlayerController() })
		{
			// set the ui (only) input mode data for the player's controller
			FInputModeUIOnly inputModeData;
			inputModeData.SetWidgetToFocus(TakeWidget());
			inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			pController->SetInputMode(inputModeData);
			pController->SetShowMouseCursor(true);
		}
	}
	if (UGameInstance * gameInstance{ GetGameInstance() }) {
		sessionInterface = gameInstance->GetSubsystem<UWolfMultiplayerSessionsSubsystem>();
	}

	// bind delegates
	if (sessionInterface)
	{
		sessionInterface->OnCreateSessionComplete.AddDynamic(this, &UWolfMultiplayerSessionsMenu::OnCreateSession);
		sessionInterface->OnFindSessionsComplete.AddUObject(this, &UWolfMultiplayerSessionsMenu::OnFindSessions);
		sessionInterface->OnJoinSessionComplete.AddUObject(this, &UWolfMultiplayerSessionsMenu::OnJoinSession);
		sessionInterface->OnStartSessionComplete.AddDynamic(this, &UWolfMultiplayerSessionsMenu::OnStartSession);
		sessionInterface->OnDestroySessionComplete.AddDynamic(this, &UWolfMultiplayerSessionsMenu::OnDestroySession);
	}
}

bool UWolfMultiplayerSessionsMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &UWolfMultiplayerSessionsMenu::onHostBtnClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UWolfMultiplayerSessionsMenu::onJoinBtnClicked);
	}
	return false;
}

void UWolfMultiplayerSessionsMenu::onHostBtnClicked()
{
	HostButton->SetIsEnabled(false);
	if (sessionInterface)
	{
		sessionInterface->CreateSession(NumPublicConnections, MatchType);
	}
}

void UWolfMultiplayerSessionsMenu::onJoinBtnClicked()
{
	JoinButton->SetIsEnabled(false);
	if (sessionInterface)
	{
		sessionInterface->FindSessions(1000);
	}
}


void UWolfMultiplayerSessionsMenu::OnCreateSession(bool successful)
{
	HostButton->SetIsEnabled(!successful);
	if (GEngine)
	{
		if (successful)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString(TEXT("Create Session Success"))
			);
			if (UWorld * world{ GetWorld() }) {
				world->ServerTravel(PathToLobby);
			}
		}
		else {
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString(TEXT("Create Session Failed"))
			);
		}
	}
}

void UWolfMultiplayerSessionsMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& searchResults, bool successful)
{
	JoinButton->SetIsEnabled(!(successful && searchResults.Num() > 0));
	if (searchResults.Num() == 0)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString(TEXT("No session available to join at the moment."))
			);
		}
		return;
	}

	for (auto result : searchResults)
	{
		FString settingsValue;
		result.Session.SessionSettings.Get(FName("MatchType"), settingsValue);
		if (settingsValue == MatchType)
		{
			sessionInterface->JoinSession(result);
			return;
		}
	}
}

void UWolfMultiplayerSessionsMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type result)
{
	JoinButton->SetIsEnabled(!result == EOnJoinSessionCompleteResult::Success);
	IOnlineSubsystem* subsystem{ IOnlineSubsystem::Get() };
	if (subsystem)
	{
		IOnlineSessionPtr isession{ subsystem->GetSessionInterface() };
		if (isession.IsValid())
		{
			FString address;
			isession->GetResolvedConnectString(NAME_GameSession, address);
			if (APlayerController * pController{ GetGameInstance()->GetFirstLocalPlayerController() }) {
				pController->ClientTravel(address, ETravelType::TRAVEL_Absolute);
			}
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("Joined Session"))
		);
	}

}


void UWolfMultiplayerSessionsMenu::OnStartSession(bool successful)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("Started Session"))
		);
	}
}

void UWolfMultiplayerSessionsMenu::OnDestroySession(bool successful)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("Destroyed Session"))
		);
	}
}
void UWolfMultiplayerSessionsMenu::NativeDestruct()
{
	MenuTeardown();
	Super::NativeDestruct();
}


void UWolfMultiplayerSessionsMenu::MenuTeardown()
{
	RemoveFromParent();
	if (UWorld * world{ GetWorld() }) {
		if (APlayerController * pController{ world->GetFirstPlayerController() }) {
			FInputModeGameOnly inputModelData;
			pController->SetInputMode(inputModelData);
			pController->SetShowMouseCursor(false);
		}
	}
}

