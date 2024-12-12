// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu/MainMenuGameMode.h"

#include "MainMenu/MainMenuPlayerController.h"

AMainMenuGameMode::AMainMenuGameMode(const FObjectInitializer& ObjectInitializer)
{
	this->PlayerControllerClass = AMainMenuPlayerController::StaticClass();
	this->DefaultPawnClass = nullptr;
}

