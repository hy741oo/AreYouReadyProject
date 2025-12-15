// Fill out your copyright notice in the Description page of Project Settings.


#include "AYRGameInstance.h"

#include "Framework/Application/SlateApplication.h"
#include "GameSetting/GameSettingSubsystem.h"
#include "GameConfigSubsystem.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"

DEFINE_LOG_CATEGORY(LogAYRGameInstance);

void UAYRGameInstance::Init()
{
	Super::Init();

	// 添加InputProcess。
	FSlateApplication& App = FSlateApplication::Get();
	// 检测是否已经添加过一次，防止重复添加。
	if (!this->InputProcessor.IsValid())
	{
		this->InputProcessor = MakeShared<FAYRInputProcessor>(this);
		App.RegisterInputPreProcessor(this->InputProcessor);
	}

	// 应用设置系统里面的设置。
	UGameSettingSubsystem* GameSetting = UGameInstance::GetSubsystem<UGameSettingSubsystem>(this);
	GameSetting->ApplySetting();

	// 使用空函数来取代引擎默认导航。目的是为了禁用引擎自带的UI导航系统。
	if (UGameViewportClient* Viewport = this->GetGameViewportClient())
	{
		auto Functor = [](const uint32 InUserIndex, TSharedPtr<SWidget> InDestination) -> bool {
			return true;
			};
		Viewport->OnNavigationOverride().BindLambda(Functor);
	}

	// 获取玩家Steam成就信息。
	if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get(STEAM_SUBSYSTEM))
	{
		if (IOnlineAchievementsPtr Achievements = OnlineSubsystem->GetAchievementsInterface())
		{
			if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
			{
				if (FUniqueNetIdPtr UniqueNetId = Identity->GetUniquePlayerId(0))
				{

					Achievements->QueryAchievements(*UniqueNetId,
						FOnQueryAchievementsCompleteDelegate::CreateWeakLambda(this,
							[](const FUniqueNetId& InUniqueNetId, const bool bSuccess)
							{
								if (bSuccess)
								{
									UE_LOG(LogAYRGameInstance, Log, TEXT("Query achievements successfully."));
								}
								else
								{
									UE_LOG(LogAYRGameInstance, Error, TEXT("Failed to query achievements."))
								}
							})
						);
				}
			}
		}
	}
}

void UAYRGameInstance::Shutdown()
{
	FSlateApplication& App = FSlateApplication::Get();
	App.UnregisterInputPreProcessor(this->InputProcessor);
	this->InputProcessor.Reset();

	Super::Shutdown();
}

EInputDeviceType::Type UAYRGameInstance::GetCurrentInputDeviceType() const
{
	if (this->InputProcessor.IsValid())
	{
		return this->InputProcessor->GetCurrentInputDeviceType();
	}

	return EInputDeviceType::IDT_KeyboardAndMouse;
}
