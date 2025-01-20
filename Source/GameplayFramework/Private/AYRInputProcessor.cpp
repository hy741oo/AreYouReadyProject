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

// 处理键盘按键按下。
bool FAYRInputProcessor::HandleKeyDownEvent(FSlateApplication& InSlateApp, const FKeyEvent& InKeyEvent)
{
	// 只取第一次输入进行判断，而不去后续几次自动重复的按键。例如一直按着A，则只取第一次输入A时的判断。
	if (!InKeyEvent.IsRepeat())
	{
		if (InKeyEvent.GetKey().IsGamepadKey())
		{
			if (this->CurrentInputType != EInputType::IT_Controller)
			{
				this->CurrentInputType = EInputType::IT_Controller;

				UE_LOG(LogTemp, Warning, TEXT("This is a KeyDown gamepad input, user index code: %d"), InKeyEvent.GetUserIndex());
			}
		}
		else
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
	if (this->CurrentInputType != EInputType::IT_Controller)
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
 