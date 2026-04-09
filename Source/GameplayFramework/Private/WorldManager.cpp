// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldManager.h"

#include "GameConfigSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "AudioDevice.h"
#include "GameConfigSubsystem.h"

DEFINE_LOG_CATEGORY(LogWorldManager)

void UWorldManager::Initialize(FSubsystemCollectionBase& Collection)
{
	
}

void UWorldManager::StartFade(const FName InCurveNameID)
{
	this->StartScreenFade(InCurveNameID);
	this->StartAudioFade(InCurveNameID);
}

void UWorldManager::StartFadeWithEvent(FOnFadeEndBPDelegate InOnFadeEndBP,  const FName InCurveNameID)
{
	this->StartScreenFadeWithEvent(InOnFadeEndBP, InCurveNameID);
	this->StartAudioFade(InCurveNameID);
}

void UWorldManager::StartFadeWithEvent(FOnFadeEndDelegate InOnFadeEnd,  const FName InCurveNameID)
{
	this->StartScreenFadeWithEvent(InOnFadeEnd, InCurveNameID);
	this->StartAudioFade(InCurveNameID);
}

void UWorldManager::StartScreenFade(const FName InCurveNameID)
{
	// 通过曲线获取渐变过程。
	UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetGameInstance());
	const UCurveFloat* CurveFloat = nullptr;
	if (Config->GetCurveFloatFromID(InCurveNameID, CurveFloat))
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
				Viewport->StartFade(CurveFloat->FloatCurve);
			}
		}
	}
}

void UWorldManager::StartScreenFadeWithEvent(FOnFadeEndBPDelegate InOnFadeEndBP,  const FName InCurveNameID)
{
	FOnFadeEndDelegate Delegate;
	Delegate.BindUFunction(InOnFadeEndBP.GetUObject(), InOnFadeEndBP.GetFunctionName());
	this->StartFadeWithEvent(Delegate, InCurveNameID);
}

void UWorldManager::StartScreenFadeWithEvent(FOnFadeEndDelegate InOnFadeEnd,  const FName InCurveNameID)
{
	// 通过曲线获取渐变过程。
	UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetGameInstance());
	const UCurveFloat* CurveFloat = nullptr;
	if (Config->GetCurveFloatFromID(InCurveNameID, CurveFloat))
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
				Viewport->StartFadeWithEvent(InOnFadeEnd, CurveFloat->FloatCurve);
			}
		}
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
		this->StopAudioFade();
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
		this->StopAudioFade();
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

void UWorldManager::StartAudioFade( const FName InCurveNameID)
{
	UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetGameInstance());
	const UCurveFloat* CurveFloat = nullptr;
	if (Config->GetCurveFloatFromID(InCurveNameID, CurveFloat))
	{
		FRichCurveKey CurveKey = CurveFloat->FloatCurve.GetLastKey();

		this->bEnableFadeAudio = true;
		this->ElapsedFadeTime = .0f;
		this->Curve = CurveFloat->FloatCurve;
	}
}

void UWorldManager::Tick(float InDeltaTime)
{
	if (this->bEnableFadeAudio)
	{
		this->ElapsedFadeTime += InDeltaTime;
		if (this->ElapsedFadeTime < this->Curve.GetLastKey().Time)
		{
			float CurrentFadeTime = this->Curve.Eval(this->ElapsedFadeTime);
			if (UWorld* World = this->GetWorld())
			{
				if (FAudioDevice* AudioDevice = World->GetAudioDeviceRaw())
				{
					AudioDevice->SetTransientMasterVolume(this->Curve.Eval(this->ElapsedFadeTime));
				}
			}
		}
		else
		{
			this->StopAudioFade();
		}
	}
}

void UWorldManager::StopAudioFade()
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
	this->Curve = FRichCurve();
}


