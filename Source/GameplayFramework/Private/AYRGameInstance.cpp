// Fill out your copyright notice in the Description page of Project Settings.


#include "AYRGameInstance.h"

#include "Framework/Application/SlateApplication.h"
#include "GameSetting/GameSettingSubsystem.h"
#include "Widgets/SViewport.h"

void UAYRGameInstance::OnStart()
{
	Super::OnStart();

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

	// 使用空函数来取代引擎默认导航。但是暂时不启用。未来需要实现完全自定义导航逻辑时再启用。
	//if (UGameViewportClient* Viewport = this->GetGameViewportClient())
	//{
	//	auto Functor = [](const uint32 InUserIndex, TSharedPtr<SWidget> InDestination) -> bool {
	//		return true;
	//		};
	//	Viewport->OnNavigationOverride().BindLambda(Functor);
	//}
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
