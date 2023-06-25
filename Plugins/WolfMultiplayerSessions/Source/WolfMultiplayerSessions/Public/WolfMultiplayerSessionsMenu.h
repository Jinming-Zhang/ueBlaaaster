// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "WolfMultiplayerSessionsMenu.generated.h"

/**
 *
 */
UCLASS()
class WOLFMULTIPLAYERSESSIONS_API UWolfMultiplayerSessionsMenu : public UUserWidget
{
	GENERATED_BODY()
private:
public:
	UPROPERTY(meta = (BindWidget))
		class UButton* HostButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* JoinButton;
	UFUNCTION()
		void onHostBtnClicked();
	UFUNCTION()
		void onJoinBtnClicked();
	// session settings ui
	int32 NumPublicConnections{ 4 };
	FString MatchType{ TEXT("FreeForAll") };
	FString PathToLobby{ TEXT("") };
protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;
public:
	UFUNCTION(BlueprintCallable)
		void MenuSetup(
			int32 numberOfPublicConnections = 4,
			FString typeOfMatch = FString(TEXT("FreeForAll")),
			FString lobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby")));
	UFUNCTION(BlueprintCallable)
		void MenuTeardown();
	// multiplayer session variables
	class UWolfMultiplayerSessionsSubsystem* sessionInterface;

protected:
	// call back delegates
	// since these will be bound to unreal dynamic delegates
	// we need to have UFUNCTION attribute for each function
	UFUNCTION()
		void OnCreateSession(bool successful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& searchResults, bool successful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type result);
	UFUNCTION()
		void OnStartSession(bool successful);
	UFUNCTION()
		void OnDestroySession(bool successful);
};
