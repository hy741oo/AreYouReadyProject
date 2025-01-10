// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldManager.h"

#include "MoviePlayer.h"

DEFINE_LOG_CATEGORY(LogWorldManager)

void UWorldManager::Initialize(FSubsystemCollectionBase& Collection)
{
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UWorldManager::OnPreLoadMap);
}

void UWorldManager::Tick(float InDeltaTime)
{
	Super::Tick(InDeltaTime);

	if (this->bIsTick)
	{
		this->ElapsedTime += InDeltaTime;
		if (this->ElapsedTime >= this->TargetTime)
		{
			this->OnLoadingScreenEnd();
			this->ResetTickTimer();
		}
	}
}

void UWorldManager::OnPreLoadMap(const FString& InMapName)
{
	UE_LOG(LogWorldManager, Display, TEXT("Loading map: %s..."), *InMapName);

	if (this->CanPlayLoadingScreen())
	{
		// 当关卡加载完后调用。
		FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UWorldManager::OnPostLoadMapWithWorld);

		float DelayTimeBeforeLoadingEnd = 3.f;

		FLoadingScreenAttributes LoadingScreenAttributes;
		LoadingScreenAttributes.bWaitForManualStop = true;
		//LoadingScreenAttributes.MinimumLoadingScreenDisplayTime = DelayTimeBeforeLoadingEnd;
		LoadingScreenAttributes.bAllowEngineTick = true;
		LoadingScreenAttributes.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();

		GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);
		GetMoviePlayer()->PlayMovie();

		this->StartTickTimer(DelayTimeBeforeLoadingEnd);
	}
	else
	{
		this->bIsFirst = false;
	}
}

void UWorldManager::OnPostLoadMapWithWorld(UWorld* InLoadedWorld)
{
	UE_LOG(LogWorldManager, Display, TEXT("Loading map finished, new world: %s"), *InLoadedWorld->GetFName().ToString());
}

void UWorldManager::OnLoadingScreenEnd()
{
	UE_LOG(LogWorldManager, Display, TEXT("Loading screen end."));

	GetMoviePlayer()->StopMovie();
	GetMoviePlayer()->WaitForMovieToFinish();
}

bool UWorldManager::CanPlayLoadingScreen() const
{
	return !this->bIsFirst;
}

void UWorldManager::StartTickTimer(float InTargetTime)
{
	if (!this->bIsTick)
	{
		this->bIsTick = true;
		this->TargetTime = InTargetTime;
		this->ElapsedTime = .0f;
	}
}

void UWorldManager::ResetTickTimer()
{
	this->bIsTick = false;
	this->TargetTime = this->ElapsedTime = .0f;
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
				Viewport->StartFade(InOnFadeEnd, InDurationTime, InbFadeIn);
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

