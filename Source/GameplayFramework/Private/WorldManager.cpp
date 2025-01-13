// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldManager.h"

#include "MoviePlayer.h"

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

