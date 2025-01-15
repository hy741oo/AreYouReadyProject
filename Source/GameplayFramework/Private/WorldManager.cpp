// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldManager.h"

#include "MoviePlayer.h"
#include "ConfigSubsystem.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogWorldManager)

void UWorldManager::Initialize(FSubsystemCollectionBase& Collection)
{
	
}

void UWorldManager::StartFade(const float DurationTime, const bool bFadeIn) const
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (UAYRGameViewportClient* Viewport = CastChecked<UAYRGameViewportClient>(World->GetGameViewport()))
		{
			if (!Viewport->IsFading())
			{
				Viewport->StartFade(DurationTime, bFadeIn);
			}
		}
	}
}

void UWorldManager::StartFadeWithEvent(FOnFadeEnd InOnFadeEnd, const float InDurationTime, const bool InbFadeIn) const
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (UAYRGameViewportClient* Viewport = CastChecked<UAYRGameViewportClient>(World->GetGameViewport()))
		{
			if (!Viewport->IsFading())
			{
				Viewport->StartFade(InDurationTime, InbFadeIn, InOnFadeEnd);
			}
		}
	}
}

void UWorldManager::StopFade() const
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (UAYRGameViewportClient* Viewport = CastChecked<UAYRGameViewportClient>(World->GetGameViewport()))
		{
			Viewport->ResetFade();
		}
	}
}

void UWorldManager::OpenNewLevel(const FName InNewLevelID) const
{
	// 检查关卡名字是否合法，以及是否可以在数据表查找到，否则可能会造成已进入加载场景但是找不到新关卡的情况。
	if (!InNewLevelID.IsNone())
	{
		if (UConfigSubsystem* ConfigSubsystem = UGameInstance::GetSubsystem<UConfigSubsystem>(this->GetGameInstance()))
		{
			if (ConfigSubsystem->HasDataTableRowFromID<FLevelData>(InNewLevelID))
			{
				const FString Options = FString::Printf(TEXT("NewLevelID=%s"), *InNewLevelID.ToString());
				const FName LoadingScreenLevelName = "/Game/AYR/Maps/LoadingScreen/MAP_LoadingScreen";
				UGameplayStatics::OpenLevel(this, LoadingScreenLevelName, true, Options);
			}
		}
	}

	UE_LOG(LogWorldManager, Warning, TEXT("InNewLevelID is invalid!: \"%s\""), *InNewLevelID.ToString());
}

