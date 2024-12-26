// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldManager.h"

#include "MoviePlayer.h"
#include "UI/LoadingScreenWidget.h"

DEFINE_LOG_CATEGORY(LogWorldManager)

void UWorldManager::Initialize(FSubsystemCollectionBase& Collection)
{
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UWorldManager::OnPreLoadMap);
}

void UWorldManager::OnPreLoadMap(const FString& InMapName)
{
	UE_LOG(LogWorldManager, Display, TEXT("Loading map: %s..."), *InMapName);

	if (this->CanPlayLoadingScreen())
	{
		FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UWorldManager::OnPostLoadMapWithWorld);

		FLoadingScreenAttributes LoadingScreenAttributes;
		LoadingScreenAttributes.bWaitForManualStop = false;
		LoadingScreenAttributes.MinimumLoadingScreenDisplayTime = 5.f;
		LoadingScreenAttributes.bAllowEngineTick = true;
		UClass* LoadingScreenWidgetClass = LoadClass<UUserWidget>(this, TEXT("/Game/AYR/Blueprints/UIs/LoadingScreen/WBP_LoadingScreen.WBP_LoadingScreen_C"));
		if (ULoadingScreenWidget* UI = CreateWidget<ULoadingScreenWidget>(GetWorld(), LoadingScreenWidgetClass))
		{
			//this->LoadingScreenWidget = UI;
			LoadingScreenAttributes.WidgetLoadingScreen = UI->TakeWidget();
			//LoadingScreenAttributes.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();
		}

		GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);
	}
}

void UWorldManager::OnPostLoadMapWithWorld(UWorld* InLoadedWorld)
{
	UE_LOG(LogWorldManager, Display, TEXT("Loading map finished, new world: %s"), *InLoadedWorld->GetFName().ToString());

	//GetMoviePlayer()->StopMovie();
	//GetMoviePlayer()->WaitForMovieToFinish();
}

bool UWorldManager::CanPlayLoadingScreen()
{
	if (this->bIsFirst)
	{
		this->bIsFirst = false;
		return false;
	}

	return true;
}

