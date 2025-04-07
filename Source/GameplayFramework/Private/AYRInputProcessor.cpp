// Fill out your copyright notice in the Description page of Project Settings.


#include "AYRInputProcessor.h"

#include "Kismet/GameplayStatics.h"
#include "AYRPlayerController.h"

DEFINE_LOG_CATEGORY(LogAYRInputProcessor);

FAYRInputProcessor::FAYRInputProcessor(UGameInstance* InGameInstance)
	:GameInstance(InGameInstance)
{
	ensureAlways(this->GameInstance);
}

// 处理键盘和手柄按键按下。
bool FAYRInputProcessor::HandleKeyDownEvent(FSlateApplication& InSlateApp, const FKeyEvent& InKeyEvent)
{
	// 按下按键时广播按键消息。
	this->OnHandleAnyPressableKey(InKeyEvent.GetKey());

	// 只取第一次输入进行判断，而不去后续几次自动重复的按键。例如一直按着A，则只取第一次输入A时的判断。
	if (!InKeyEvent.IsRepeat())
	{
		// 按下的是手柄按键。
		if (InKeyEvent.GetKey().IsGamepadKey())
		{
			if (this->CurrentInputDeviceType != EInputDeviceType::IDT_Controller)
			{
				this->CurrentInputDeviceType = EInputDeviceType::IDT_Controller;

				UE_LOG(LogAYRInputProcessor, Display, TEXT("This is a KeyDown gamepad input, user index code: %d"), InKeyEvent.GetUserIndex());
				this->OnPlayerInputDeviceChanged(this->GetCurrentInputDeviceType());
			}
		}
		else // 按下的是键盘按键。
		{
			if (this->CurrentInputDeviceType != EInputDeviceType::IDT_KeyboardAndMouse)
			{
				this->CurrentInputDeviceType = EInputDeviceType::IDT_KeyboardAndMouse;

				UE_LOG(LogAYRInputProcessor, Display, TEXT("This is a KeyDown Keyboard input, user index code: %d"), InKeyEvent.GetUserIndex());
				this->OnPlayerInputDeviceChanged(this->GetCurrentInputDeviceType());
			}
		}
	}

	return IInputProcessor::HandleKeyDownEvent(InSlateApp, InKeyEvent);
}

// 处理鼠标按键按下。
bool FAYRInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& InSlateApp, const FPointerEvent& InMouseEvent)
{
	// 按下按键时广播按键消息。
	this->OnHandleAnyPressableKey(InMouseEvent.GetEffectingButton());

	if (this->CurrentInputDeviceType != EInputDeviceType::IDT_KeyboardAndMouse)
	{
		this->CurrentInputDeviceType = EInputDeviceType::IDT_KeyboardAndMouse;

		UE_LOG(LogAYRInputProcessor, Display, TEXT("This is a MouseButtonDown input, user index code: %d"), InMouseEvent.GetUserIndex());
		this->OnPlayerInputDeviceChanged(this->GetCurrentInputDeviceType());
	}

	return IInputProcessor::HandleMouseButtonDownEvent(InSlateApp, InMouseEvent);
}

// 处理鼠标移动。
bool FAYRInputProcessor::HandleMouseMoveEvent(FSlateApplication& InSlateApp, const FPointerEvent& InMouseEvent)
{
	// 判断鼠标移动向量的模大于0.4时才触发检测，防止鼠标灵敏度太高导致检测太频繁。
	if (this->CurrentInputDeviceType != EInputDeviceType::IDT_KeyboardAndMouse && InMouseEvent.GetCursorDelta().Size() > 3.f )
	{
		this->CurrentInputDeviceType = EInputDeviceType::IDT_KeyboardAndMouse;

		UE_LOG(LogAYRInputProcessor, Display, TEXT("This is a MouseMove input, user index code: %d"), InMouseEvent.GetUserIndex());
		this->OnPlayerInputDeviceChanged(this->GetCurrentInputDeviceType());
	}

	return IInputProcessor::HandleMouseMoveEvent(InSlateApp, InMouseEvent);
}

// 处理鼠标滚轮。
bool FAYRInputProcessor::HandleMouseWheelOrGestureEvent(FSlateApplication& InSlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent)
{
	if (this->CurrentInputDeviceType != EInputDeviceType::IDT_KeyboardAndMouse)
	{
		this->CurrentInputDeviceType = EInputDeviceType::IDT_KeyboardAndMouse;

		UE_LOG(LogAYRInputProcessor, Display, TEXT("This is a MouseMove input, user index code: %d"), InWheelEvent.GetUserIndex());
		this->OnPlayerInputDeviceChanged(this->GetCurrentInputDeviceType());
	}

	return IInputProcessor::HandleMouseWheelOrGestureEvent(InSlateApp, InWheelEvent, InGestureEvent);
}

EInputDeviceType::Type FAYRInputProcessor::GetCurrentInputDeviceType() const
{
	return this->CurrentInputDeviceType;
}

void FAYRInputProcessor::OnPlayerInputDeviceChanged(const EInputDeviceType::Type InInputDeviceType)
{
	FGameplayTag Tag = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("GMSMessage.System.Input.DeviceType"));
	if (Tag.IsValid())
	{
		UGMSInputDeviceType* Message = NewObject<UGMSInputDeviceType>();
		Message->CurrentType = InInputDeviceType;

		UGameplayMessageSystem* GMS = UGameInstance::GetSubsystem<UGameplayMessageSystem>(this->GameInstance);
		GMS->Broadcast(Tag, Message);
	}
}

void FAYRInputProcessor::OnHandleAnyPressableKey(const FKey& InHandledKey)
{
	UE_LOG(LogAYRInputProcessor, Display, TEXT("Pressed key is: %s"), *InHandledKey.ToString());
	FGameplayTag Tag = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("GMSMessage.System.Input.HandleKey"));
	if (Tag.IsValid())
	{
		UGMSHandledKey* Message = NewObject<UGMSHandledKey>();
		Message->HandledKey = InHandledKey;

		UGameplayMessageSystem* GMS = UGameInstance::GetSubsystem<UGameplayMessageSystem>(this->GameInstance);
		GMS->Broadcast(Tag, Message);
	}
}
 