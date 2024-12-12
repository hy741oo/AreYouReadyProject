// Fill out your copyright notice in the Description page of Project Settings.


#include "AYRGameModeBase.h"

#include "AYRPlayerController.h"

APlayerController* AAYRGameModeBase::SpawnPlayerControllerCommon(ENetRole InRemoteRole, FVector const& SpawnLocation, FRotator const& SpawnRotation, TSubclassOf<APlayerController> InPlayerControllerClass)
{
	AAYRPlayerController* PlayerController = Cast<AAYRPlayerController>(Super::SpawnPlayerControllerCommon(InRemoteRole, SpawnLocation, SpawnRotation, InPlayerControllerClass));
	if (ensureAlways(PlayerController))
	{
		PlayerController->ID = this->PlayerControllerID;
	}

	return PlayerController;
}

