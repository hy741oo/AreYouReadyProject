// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreen/LoadingScreenGameMode.h"

#include "LoadingScreen/LoadingScreenPlayerController.h"

ALoadingScreenGameMode::ALoadingScreenGameMode(const FObjectInitializer& InObjectInitializer)
	:Super(InObjectInitializer)
{
	this->PlayerControllerClass = ALoadingScreenPlayerController::StaticClass();
	this->DefaultPawnClass = nullptr;
}

