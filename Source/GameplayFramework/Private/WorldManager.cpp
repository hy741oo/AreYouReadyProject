// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldManager.h"

#include "GameConfigSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "AudioDevice.h"

DEFINE_LOG_CATEGORY(LogWorldManager)

void UWorldManager::Initialize(FSubsystemCollectionBase& Collection)
{
	
}

void UWorldManager::StartFade(const bool InbFadeIn,const float InDurationTime, const bool InbEnableFadeAudio)
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (UAYRGameViewportClient* Viewport = CastChecked<UAYRGameViewportClient>(World->GetGameViewport()))
		{
			// 如果正处于Fade状态，则先中断之前的Fade。
			if (Viewport->IsFading())
			{
				Viewport->AbortFade();
			}
			Viewport->StartFade(InbFadeIn, InDurationTime);
		}
	}

	if (InbEnableFadeAudio)
	{
		this->StartFadeAudio(InbFadeIn, InDurationTime);
	}
}

void UWorldManager::StartFadeWithEvent(FOnFadeEndBPDelegate InOnFadeEndBP, const bool InbFadeIn, const float InDurationTime, const bool InbEnableFadeAudio)
{
	FOnFadeEndDelegate Delegate;
	Delegate.BindUFunction(InOnFadeEndBP.GetUObject(), InOnFadeEndBP.GetFunctionName());
	this->StartFadeWithEvent(Delegate, InbFadeIn, InDurationTime, InbEnableFadeAudio);
}

void UWorldManager::StartFadeWithEvent(FOnFadeEndDelegate InOnFadeEnd, const bool InbFadeIn, const float InDurationTime, const bool InbEnableFadeAudio)
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (UAYRGameViewportClient* Viewport = CastChecked<UAYRGameViewportClient>(World->GetGameViewport()))
		{
			// 如果正处于Fade状态，则先中断之前的Fade。
			if (Viewport->IsFading())
			{
				Viewport->AbortFade();
			}
			Viewport->StartFadeWithEvent(InOnFadeEnd, InbFadeIn, InDurationTime);
		}
	}

	if (InbEnableFadeAudio)
	{
		this->StartFadeAudio(InbFadeIn, InDurationTime);
	}
}

void UWorldManager::StopFade()
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (UAYRGameViewportClient* Viewport = CastChecked<UAYRGameViewportClient>(World->GetGameViewport()))
		{
			Viewport->StopFade();
		}
	}

	if (this->bEnableFadeAudio)
	{
		this->StopFadeAudio();
	}
}

void UWorldManager::AbortFade()
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (UAYRGameViewportClient* Viewport = CastChecked<UAYRGameViewportClient>(World->GetGameViewport()))
		{
			Viewport->AbortFade();
		}
	}

	if (this->bEnableFadeAudio)
	{
		this->StopFadeAudio();
	}
}

void UWorldManager::OpenNewLevel(const FName InNewLevelID) const
{
	// 检查关卡名字是否合法，以及是否可以在数据表查找到，否则可能会造成已进入加载场景但是找不到新关卡的情况。
	if (!InNewLevelID.IsNone())
	{
		UGameConfigSubsystem* ConfigSubsystem = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetGameInstance());
		if (ConfigSubsystem->HasDataTableRowFromID<FLevelData>(InNewLevelID))
		{
			const FString Options = FString::Printf(TEXT("NewLevelID=%s"), *InNewLevelID.ToString());
			const FName LoadingScreenLevelName = "/Game/AYR/Maps/LoadingScreen/MAP_LoadingScreen";
			UGameplayStatics::OpenLevel(this, LoadingScreenLevelName, true, Options);
			return;
		}
	}

	UE_LOG(LogWorldManager, Warning, TEXT("InNewLevelID is invalid!: \"%s\""), *InNewLevelID.ToString());
}

void UWorldManager::StartFadeAudio(const bool InbFadeIn, const float InDurationTime)
{
	this->bEnableFadeAudio = true;
	this->ElapsedFadeTime = .0f;
	this->bIsFadeIn = InbFadeIn;
	this->TargetFadeTime = InDurationTime;
}

void UWorldManager::Tick(float InDeltaTime)
{
	if (this->bEnableFadeAudio)
	{
		this->ElapsedFadeTime += InDeltaTime;
		if (this->ElapsedFadeTime < this->TargetFadeTime)
		{
			float CurrentFadeTime = this->ElapsedFadeTime / this->TargetFadeTime;
			if (UWorld* World = this->GetWorld())
			{
				if (FAudioDevice* AudioDevice = World->GetAudioDeviceRaw())
				{
					AudioDevice->SetTransientMasterVolume(this->bIsFadeIn ? CurrentFadeTime : 1.f - CurrentFadeTime);
				}
			}
		}
		else
		{
			this->StopFadeAudio();
		}
	}
}

void UWorldManager::StopFadeAudio()
{
	// 停止渐变音频时需要将总音频音量调为原来的值。
	if (UWorld* World = this->GetWorld())
	{
		if (FAudioDevice* AudioDevice = World->GetAudioDeviceRaw())
		{
			AudioDevice->SetTransientMasterVolume(1.f);
		}
	}
	this->bEnableFadeAudio = false;
	this->ElapsedFadeTime = .0f;
	this->TargetFadeTime = .0f;
}


