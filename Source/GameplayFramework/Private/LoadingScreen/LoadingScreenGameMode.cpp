// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreen/LoadingScreenGameMode.h"

#include "LoadingScreen/LoadingScreenPlayerController.h"

ALoadingScreenGameMode::ALoadingScreenGameMode(const FObjectInitializer& InObjectInitializer)
	:Super(InObjectInitializer)
{
	this->PlayerControllerClass = ALoadingScreenPlayerController::StaticClass();
	this->DefaultPawnClass = nullptr;
}

APlayerController* ALoadingScreenGameMode::SpawnPlayerControllerCommon(ENetRole InRemoteRole, FVector const& InSpawnLocation, FRotator const& InSpawnRotation, TSubclassOf<APlayerController> InPlayerControllerClass)
{
	return this->LoadingScreenPlayerController = CastChecked<ALoadingScreenPlayerController>(Super::SpawnPlayerControllerCommon(InRemoteRole, InSpawnLocation, InSpawnRotation, InPlayerControllerClass));
}

