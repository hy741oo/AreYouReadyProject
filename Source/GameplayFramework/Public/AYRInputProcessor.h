// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"
#include "GameplayMessageSubsystem.h"
#include "UObject/StrongObjectPtr.h"

#include "AYRInputProcessor.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAYRInputProcessor, Log, All);

UENUM()
namespace EInputDeviceType
{
	enum Type
	{
		IDT_None UMETA(Hidden, DisplayName = "None"),
		IDT_KeyboardAndMouse UMETA(DisplayName = "Keyboard and Mouse"),
		IDT_Controller UMETA(DisplayName = "Controller")
	};
}

UCLASS()
class UGMSInputDeviceType : public UGMSMessageBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ExposeOnSpawn))
	TEnumAsByte<EInputDeviceType::Type> CurrentType = EInputDeviceType::IDT_Controller;
};

UCLASS()
class UGMSHandledKey : public UGMSMessageBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ExposeOnSpawn))
	FKey HandledKey;
};

/**
 * 
 */
class GAMEPLAYFRAMEWORK_API FAYRInputProcessor : public IInputProcessor
{

public:
	FAYRInputProcessor(UGameInstance* GameInstance);

private:
	// 当前输入类型。
	EInputDeviceType::Type CurrentInputDeviceType = EInputDeviceType::IDT_KeyboardAndMouse;

	// GameInstance，用于后续处理Gameplay相关的逻辑。
	UGameInstance* GameInstance = nullptr;

	// 输入设备切换事件的Message，定义成成员变量是为了防止频繁生成造成的性能低下。UPROPERTY是为了防止被GC掉。
	TStrongObjectPtr<UGMSInputDeviceType> InputDeviceTypeMessage = nullptr;

	// 由于按键输入是一个频繁的事件，所以为了节省性能，在这里定义一个GMS会用到的Message，这样后续就不需要反复生成。UPROPERTY防止被GC掉。
	TStrongObjectPtr<UGMSHandledKey> InputKeyMessage = nullptr;

private:
	// 当玩家的输入设备类型改变时调用。
	virtual void OnPlayerInputDeviceChanged(const EInputDeviceType::Type InInputDeviceType);

	// 当玩家输入按键（键鼠或是手柄的按键）时调用。
	virtual void OnHandleAnyPressableKey(const FKey& InHandledKey);

public:
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override
	{
		// 空函数体。
	}

	// 处理键盘或手柄按键按下。
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;

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

	// 获取当前输入设备类型。
	EInputDeviceType::Type GetCurrentInputDeviceType() const;
};

