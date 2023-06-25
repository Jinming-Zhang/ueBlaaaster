// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "WolfMultiplayerSessionsSubsystem.generated.h"

/// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWolfOnCreateSessionComplete, bool, successful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FWolfOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& searchResults, bool successful);
DECLARE_MULTICAST_DELEGATE_OneParam(FWolfOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWolfOnStartSessionComplete, bool, successful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWolfOnDestroySessionComplete, bool, successful);
/**
 * A Subsystem that manages multiplayer sessions based on Unreal's Online Subsystem
 * UGameInstanceSubsystem means
 * - An instance of this subsystem will be created automatically when the game instance is created
 * - The instance will be destroyed automatically when the game instance is destroyed
 */
UCLASS()
class WOLFMULTIPLAYERSESSIONS_API UWolfMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UWolfMultiplayerSessionsSubsystem();
	~UWolfMultiplayerSessionsSubsystem();

private:
	IOnlineSessionPtr sessionInterface;
	TSharedPtr<FOnlineSessionSettings> lastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> sessionSearch;
public:
	// own custom delegates for external class to register
	FWolfOnCreateSessionComplete OnCreateSessionComplete;
	FWolfOnFindSessionsComplete OnFindSessionsComplete;
	FWolfOnJoinSessionComplete OnJoinSessionComplete;
	FWolfOnStartSessionComplete OnStartSessionComplete;
	FWolfOnDestroySessionComplete OnDestroySessionComplete;
public:
	void CreateSession(int32 numPublicConnections, FString matchType);
	void FindSessions(int32 maxSearchResult);
	void JoinSession(const FOnlineSessionSearchResult& sessionResult);
	void StartSession();
	void DestroySession();
private:
	// delegates and delegate handles for session management
	FOnCreateSessionCompleteDelegate createSessionCompleteDelegate;
	FDelegateHandle createSessionCompleteDelegateHandle;

	FOnFindSessionsCompleteDelegate findSessionsCompleteDelegate;
	FDelegateHandle findSessionsCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate joinSessionCompleteDelegate;
	FDelegateHandle joinSessionCompleteDelegateHandle;

	FOnStartSessionCompleteDelegate startSessionCompleteDelegate;
	FDelegateHandle startSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate destroySessionCompleteDelegate;
	FDelegateHandle destroySessionCompleteDelegateHandle;

protected:
	// call backs for each delegate
	void OnCreateSessionCompleteCB(FName sessionName, bool successful);
	void OnFindSessionsCompleteCB(bool successful);
	void OnJoinSessionCompleteCB(FName sessionName, EOnJoinSessionCompleteResult::Type result);
	void OnStartSessionCompleteCB(FName sessionName, bool successful);
	void OnDestroySessionCompleteCB(FName sessionName, bool successful);

	bool createSessionOnDestroy;
	int32 lastNumPubConnections;
	FString lastMatchType;
};
