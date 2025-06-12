// Fill out your copyright notice in the Description page of Project Settings.


#include "AYRGameInstance.h"

#include "Framework/Application/SlateApplication.h"
#include "GameSetting/GameSettingSubsystem.h"
#include "Widgets/SViewport.h"
#include "UI/Common/AYRButton.h"
#include "GameDelegates.h"

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

	// 使用空函数来取代引擎默认导航。目的是为了禁用引擎自带的UI导航系统。
	if (UGameViewportClient* Viewport = this->GetGameViewportClient())
	{
		auto Functor = [](const uint32 InUserIndex, TSharedPtr<SWidget> InDestination) -> bool {
			return true;
			};
		Viewport->OnNavigationOverride().BindLambda(Functor);
	}

	// 在引擎退出之前清理AYRButton的组。
	// 通常情况下AYRButton的组会在该Button被销毁时自动执行Unselect操作，但是如果我们在Unselect操作里访问了一些引擎的对象（如PlayerController）则会因为这些对象提前被销毁而拿不到该对象，
	// 因此我们优先执行清理操作，防止我们要访问的对象在我们访问之前就被清理了。
	FGameDelegates::Get().GetExitCommandDelegate().AddLambda(
		[]() {
			UAYRButton::ClearRegisteredButtons();
		}
	);
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
