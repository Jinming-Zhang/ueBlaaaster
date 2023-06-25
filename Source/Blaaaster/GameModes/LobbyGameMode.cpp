// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* newPlayer)
{
	Super::PostLogin(newPlayer);
	int32 numPlayers{ GameState.Get()->PlayerArray.Num() };
	if (NumPlayers >= playersNeeded)
	{
		if (UWorld * world{ GetWorld() })
		{
			FString path = pathToMap.Append("?listen");
			bUseSeamlessTravel = true;
			world->ServerTravel(path);
		}
	}
}
