// Fill out your copyright notice in the Description page of Project Settings.


#include "WolfMultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineIdentityInterface.h"

UWolfMultiplayerSessionsSubsystem::UWolfMultiplayerSessionsSubsystem()
{
	// retrieve the abstract online subsystem layer from unreal and initialize the session interface
	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
	if (onlineSubsystem)
	{
		sessionInterface = onlineSubsystem->GetSessionInterface();
	}
	// bind delegates to online session interface
	createSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UWolfMultiplayerSessionsSubsystem::OnCreateSessionCompleteCB);
	findSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UWolfMultiplayerSessionsSubsystem::OnFindSessionsCompleteCB);
	joinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UWolfMultiplayerSessionsSubsystem::OnJoinSessionCompleteCB);
	startSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UWolfMultiplayerSessionsSubsystem::OnStartSessionCompleteCB);
	destroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UWolfMultiplayerSessionsSubsystem::OnDestroySessionCompleteCB);
}
UWolfMultiplayerSessionsSubsystem::~UWolfMultiplayerSessionsSubsystem() { }

void UWolfMultiplayerSessionsSubsystem::CreateSession(int32 numPublicConnections, FString matchType)
{
	if (!sessionInterface.IsValid()) { return; }

	FNamedOnlineSession* existingSession = sessionInterface->GetNamedSession(NAME_GameSession);
	if (existingSession != nullptr)
	{
		createSessionOnDestroy = true;
		lastNumPubConnections = numPublicConnections;
		lastMatchType = matchType;
		DestroySession();
		return;
	}

	createSessionCompleteDelegateHandle = sessionInterface->AddOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegate);

	// session settings
	lastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	lastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	lastSessionSettings->NumPublicConnections = numPublicConnections;
	lastSessionSettings->bAllowJoinInProgress = true;
	lastSessionSettings->bAllowJoinViaPresence = true;
	lastSessionSettings->bShouldAdvertise = true;
	lastSessionSettings->bUsesPresence = true;
	lastSessionSettings->bUseLobbiesIfAvailable = true;
	lastSessionSettings->BuildUniqueId = 1;
	lastSessionSettings->Set(FName("MatchType"), matchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// create session
	const ULocalPlayer* localPlayer{ GetWorld()->GetFirstLocalPlayerFromController() };
	if (!sessionInterface->CreateSession(*(localPlayer->GetPreferredUniqueNetId()), NAME_GameSession, *lastSessionSettings)) {
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegateHandle);
		OnCreateSessionComplete.Broadcast(false);
	}
}

void UWolfMultiplayerSessionsSubsystem::OnCreateSessionCompleteCB(FName sessionName, bool successful)
{
	sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegateHandle);
	OnCreateSessionComplete.Broadcast(successful);
}

void UWolfMultiplayerSessionsSubsystem::FindSessions(int32 maxSearchResult)
{
	if (!sessionInterface.IsValid())
	{
		return;
	}
	findSessionsCompleteDelegateHandle = sessionInterface->AddOnFindSessionsCompleteDelegate_Handle(findSessionsCompleteDelegate);

	sessionSearch = MakeShareable(new FOnlineSessionSearch());
	sessionSearch->MaxSearchResults = 10000;
	sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* ulocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!sessionInterface->FindSessions(*(ulocalPlayer->GetPreferredUniqueNetId()), sessionSearch.ToSharedRef())) {
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(findSessionsCompleteDelegateHandle);

		OnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UWolfMultiplayerSessionsSubsystem::OnFindSessionsCompleteCB(bool successful)
{
	sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(findSessionsCompleteDelegateHandle);

	OnFindSessionsComplete.Broadcast(sessionSearch->SearchResults, successful);
}

void UWolfMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& sessionResult)
{
	if (!sessionInterface.IsValid())
	{
		OnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}
	joinSessionCompleteDelegateHandle = sessionInterface->AddOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegate);

	const ULocalPlayer* ulocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!sessionInterface->JoinSession(*(ulocalPlayer->GetPreferredUniqueNetId()), NAME_GameSession, sessionResult)) {
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegateHandle);
		OnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UWolfMultiplayerSessionsSubsystem::OnJoinSessionCompleteCB(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	if (sessionInterface)
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegateHandle);
		OnJoinSessionComplete.Broadcast(result);
	}
}

void UWolfMultiplayerSessionsSubsystem::StartSession()
{
}

void UWolfMultiplayerSessionsSubsystem::OnStartSessionCompleteCB(FName sessionName, bool successful)
{
}


void UWolfMultiplayerSessionsSubsystem::DestroySession()
{
	if (!sessionInterface.IsValid())
	{
		OnDestroySessionComplete.Broadcast(false);
		return;
	}
	destroySessionCompleteDelegateHandle = sessionInterface->AddOnDestroySessionCompleteDelegate_Handle(destroySessionCompleteDelegate);
	if (!sessionInterface->DestroySession(NAME_GameSession)) {
		sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(destroySessionCompleteDelegateHandle);
		OnDestroySessionComplete.Broadcast(false);
	}
}



void UWolfMultiplayerSessionsSubsystem::OnDestroySessionCompleteCB(FName sessionName, bool successful)
{
	if (sessionInterface)
	{
		destroySessionCompleteDelegateHandle = sessionInterface->AddOnDestroySessionCompleteDelegate_Handle(destroySessionCompleteDelegate);
	}
	if (successful && createSessionOnDestroy)
	{
		createSessionOnDestroy = false;
		CreateSession(lastNumPubConnections, lastMatchType);
	}
	OnDestroySessionComplete.Broadcast(successful);
}

FString UWolfMultiplayerSessionsSubsystem::GetPlayerNickName()
{
	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
	const ULocalPlayer* ulocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	auto uid = (ulocalPlayer->GetPreferredUniqueNetId());
	return onlineSubsystem->GetIdentityInterface()->GetPlayerNickname(*uid);
}
