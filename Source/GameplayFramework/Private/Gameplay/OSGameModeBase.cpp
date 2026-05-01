// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/OSGameModeBase.h"

#include "Gameplay/OSPlayerController.h"

AOSGameModeBase::AOSGameModeBase(const FObjectInitializer& InObjectInitializer)
	:Super(InObjectInitializer)
{

}

APlayerController* AOSGameModeBase::SpawnPlayerControllerCommon(ENetRole InRemoteRole, FVector const& SpawnLocation, FRotator const& SpawnRotation, TSubclassOf<APlayerController> InPlayerControllerClass)
{
	AOSPlayerController* PlayerController = Cast<AOSPlayerController>(Super::SpawnPlayerControllerCommon(InRemoteRole, SpawnLocation, SpawnRotation, InPlayerControllerClass));
	if (ensureAlways(PlayerController))
	{
		PlayerController->ID = this->PlayerControllerID;
	}

	return PlayerController;
}

