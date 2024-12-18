// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldManager.h"

#include "MoviePlayer.h"
#include "UMG.h"


void UWorldManager::Initialize(FSubsystemCollectionBase& Collection)
{
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UWorldManager::OnPreLoadMap);
}

void UWorldManager::OnPreLoadMap(const FString& InMapName)
{
	FLoadingScreenAttributes LoadingScreenAttributes;
	LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes = false;
	LoadingScreenAttributes.bWaitForManualStop = true;
	LoadingScreenAttributes.MinimumLoadingScreenDisplayTime = 10.f;
	UClass* LoadingScreenWidget = LoadClass<UUserWidget>(this, TEXT("/Game/AYR/Blueprints/UIs/LoadingScreen/WBP_LoadingScreen.WBP_LoadingScreen_C"));
	UUserWidget* UI = CreateWidget<UUserWidget>(GetWorld(), LoadingScreenWidget);
	{
		LoadingScreenAttributes.WidgetLoadingScreen = UI->TakeWidget();
	}

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);
}

void UWorldManager::OnPostLoadMapWithWorld(UWorld* InLoadedWorld)
{

}

