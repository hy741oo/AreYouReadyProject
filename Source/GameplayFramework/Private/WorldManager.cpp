// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldManager.h"

#include "MoviePlayer.h"
#include "UI/LoadingScreenWidget.h"

DEFINE_LOG_CATEGORY(LogWorldManager)

void UWorldManager::Initialize(FSubsystemCollectionBase& Collection)
{
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UWorldManager::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UWorldManager::OnPostLoadMapWithWorld);
}

void UWorldManager::OnPreLoadMap(const FString& InMapName)
{
	UE_LOG(LogWorldManager, Display, TEXT("Loading map: %s..."), *InMapName);

	if (this->CanPlayLoadingScreen())
	{
		FLoadingScreenAttributes LoadingScreenAttributes;
		LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes = true;
		LoadingScreenAttributes.bWaitForManualStop = true;
		LoadingScreenAttributes.MinimumLoadingScreenDisplayTime = 1.f;
		UClass* LoadingScreenWidgetClass = LoadClass<UUserWidget>(this, TEXT("/Game/AYR/Blueprints/UIs/LoadingScreen/WBP_LoadingScreen.WBP_LoadingScreen_C"));
		if (ULoadingScreenWidget* UI = CreateWidget<ULoadingScreenWidget>(GetWorld(), LoadingScreenWidgetClass))
		{
			this->LoadingScreenWidget = UI;
			LoadingScreenAttributes.WidgetLoadingScreen = UI->TakeWidget();
		}

		GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);
	}
}

void UWorldManager::OnPostLoadMapWithWorld(UWorld* InLoadedWorld)
{
	UE_LOG(LogWorldManager, Display, TEXT("Loading map finished, new world: %s"), *InLoadedWorld->GetFName().ToString());

	if (this->LoadingScreenWidget)
	{
		this->LoadingScreenWidget->PlayFadeinAnimation();
	}
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

