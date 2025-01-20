// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"

namespace EInputType
{
	enum Type
	{
		IT_KeyboardAndMouse,
		IT_Controller
	};
}

/**
 * 
 */
class GAMEPLAYFRAMEWORK_API FAYRInputProcessor : public IInputProcessor
{

private:
	// 当前输入类型。
	EInputType::Type CurrentInputType = EInputType::IT_KeyboardAndMouse;

	// 单例对象。
	static TSharedPtr<FAYRInputProcessor> SingletonObject;

public:
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
	{
		// 空函数体。
	}

	// 处理键盘按键按下。
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent);

	// 处理摇杆。
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent);

	// 处理鼠标按键按下。
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent);

	virtual const TCHAR* GetDebugName() const
	{
		FString DebugName = TEXT("AYRInputProcessor");
		return *DebugName;
	}

	static TSharedPtr<FAYRInputProcessor> Get();
};

