// Fill out your copyright notice in the Description page of Project Settings.


#include "AYRInputProcessor.h"

TSharedPtr<FAYRInputProcessor> FAYRInputProcessor::SingletonObject;

TSharedPtr<FAYRInputProcessor> FAYRInputProcessor::Get()
{
	if (SingletonObject.IsValid())
	{
		return SingletonObject;
	}
	else
	{
		SingletonObject = MakeShared<FAYRInputProcessor>();
		return SingletonObject;
	}
}

// 处理键盘和手柄按键按下。
bool FAYRInputProcessor::HandleKeyDownEvent(FSlateApplication& InSlateApp, const FKeyEvent& InKeyEvent)
{
	// 只取第一次输入进行判断，而不去后续几次自动重复的按键。例如一直按着A，则只取第一次输入A时的判断。
	if (!InKeyEvent.IsRepeat())
	{
		// 按下的是手柄按键。
		if (InKeyEvent.GetKey().IsGamepadKey())
		{
			if (this->CurrentInputType != EInputType::IT_Controller)
			{
				this->CurrentInputType = EInputType::IT_Controller;

				UE_LOG(LogTemp, Warning, TEXT("This is a KeyDown gamepad input, user index code: %d"), InKeyEvent.GetUserIndex());
			}
		}
		else // 按下的是键盘按键。
		{
			if (this->CurrentInputType != EInputType::IT_KeyboardAndMouse)
			{
				this->CurrentInputType = EInputType::IT_KeyboardAndMouse;

				UE_LOG(LogTemp, Warning, TEXT("This is a KeyDown Keyboard input, user index code: %d"), InKeyEvent.GetUserIndex());
			}
		}
	}

	return IInputProcessor::HandleKeyDownEvent(InSlateApp, InKeyEvent);
}

// 处理摇杆。
bool FAYRInputProcessor::HandleAnalogInputEvent(FSlateApplication& InSlateApp, const FAnalogInputEvent& InAnalogInputEvent)
{
	// 判断摇杆输入值大于0.4时才触发检测。防止摇杆灵敏度太高导致的检测太频繁。
	if (this->CurrentInputType != EInputType::IT_Controller && FMath::Abs(InAnalogInputEvent.GetAnalogValue()) > .4f)
	{
		this->CurrentInputType = EInputType::IT_Controller;

		UE_LOG(LogTemp, Warning, TEXT("This is a Analog input, user index code: %d"), InAnalogInputEvent.GetUserIndex());
	}

	return IInputProcessor::HandleAnalogInputEvent(InSlateApp, InAnalogInputEvent);

}

// 处理鼠标按键按下。
bool FAYRInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& InSlateApp, const FPointerEvent& InMouseEvent)
{
	if (this->CurrentInputType != EInputType::IT_KeyboardAndMouse)
	{
		this->CurrentInputType = EInputType::IT_KeyboardAndMouse;

		UE_LOG(LogTemp, Warning, TEXT("This is a MouseButtonDown input, user index code: %d"), InMouseEvent.GetUserIndex());
	}

	return IInputProcessor::HandleMouseButtonDownEvent(InSlateApp, InMouseEvent);
}

// 处理鼠标移动。
bool FAYRInputProcessor::HandleMouseMoveEvent(FSlateApplication& InSlateApp, const FPointerEvent& InMouseEvent)
{
	// 判断鼠标移动向量的模大于0.4时才触发检测，防止鼠标灵敏度太高导致检测太频繁。
	if (this->CurrentInputType != EInputType::IT_KeyboardAndMouse && InMouseEvent.GetCursorDelta().Size() > .4f )
	{
		this->CurrentInputType = EInputType::IT_KeyboardAndMouse;

		UE_LOG(LogTemp, Warning, TEXT("This is a MouseMove input, user index code: %d"), InMouseEvent.GetUserIndex());
	}

	return IInputProcessor::HandleMouseMoveEvent(InSlateApp, InMouseEvent);
}

// 处理鼠标滚轮。
bool FAYRInputProcessor::HandleMouseWheelOrGestureEvent(FSlateApplication& InSlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent)
{
	if (this->CurrentInputType != EInputType::IT_KeyboardAndMouse)
	{
		this->CurrentInputType = EInputType::IT_KeyboardAndMouse;

		UE_LOG(LogTemp, Warning, TEXT("This is a MouseMove input, user index code: %d"), InWheelEvent.GetUserIndex());
	}

	return IInputProcessor::HandleMouseWheelOrGestureEvent(InSlateApp, InWheelEvent, InGestureEvent);
}

 