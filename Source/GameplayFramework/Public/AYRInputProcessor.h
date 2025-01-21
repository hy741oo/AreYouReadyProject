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
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override
	{
		// 空函数体。
	}

	// 处理键盘或手柄按键按下。
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;

	// 处理摇杆。
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override;

	// 处理鼠标按键按下。
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;

	// 处理鼠标移动。
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;

	// 处理鼠标滚动。
	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override;

	virtual const TCHAR* GetDebugName() const
	{
		FString DebugName = TEXT("AYRInputProcessor");
		return *DebugName;
	}

	static TSharedPtr<FAYRInputProcessor> Get();
};

