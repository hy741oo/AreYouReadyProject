// Fill out your copyright notice in the Description page of Project Settings.


#include "AYRGameInstance.h"

#include "Framework/Application/SlateApplication.h"

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
