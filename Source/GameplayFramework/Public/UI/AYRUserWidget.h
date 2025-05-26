// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UISubsystem.h"

#include "AYRUserWidget.generated.h"

// UI栈变动时的原因。
UENUM(BlueprintType)
namespace EUIStateChangedReason
{
	enum Type
	{
		// 当处于Enter事件时表明该UI是最新添加的UI；当处于Leave事件时表明有新UI添加，而当前UI不再是栈顶UI。
		UISCR_NewWidgetEntered UMETA(DisplayName = NewWidgetEntered),
		// 当处于Enter事件时表明上层UI已经被移除，栈顶UI重新变为该UI；当处于Leave事件时表明当前UI被移除。
		UISCR_BePopped UMETA(DisplayName = BePoped),
		// 玩家输入设备变更时UI栈里最顶层的UI会接收到的事件，用于处理一些键鼠和手柄差异化功能。目前只在Enter事件会用到。
		UISCR_InputDeviceChangedIntoGamepad UMETA(DisplayName = InputDeviceChangedIntoGamepad)
	};
}


/**
 * 
 */
UCLASS(Abstract)
class GAMEPLAYFRAMEWORK_API UAYRUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// UI子系统。
	UUISubsystem* UISubsystem = nullptr;

protected:
	virtual void NativeConstruct() override;

public:
	// 当前UI在UI栈中的下标。
	int32 StackIndex = -1;
	
public:
	// 当该Widget成为Widget栈栈顶元素（新添加或该Widget以上的Widget被移除）时调用。
	virtual void OnEnterThisWidget(APlayerController* PlayerController, const FUIStackInfo* UIStackInfo, EUIStateChangedReason::Type Reason);
	// 蓝图版本。
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "On Enter This Widget"))
	void BP_OnEnterThisWidget(APlayerController* PlayerController, const FUIStackInfo UIStackInfo, EUIStateChangedReason::Type Reason);

	// 当该Widget不再成为Widget栈栈顶元素时调用。
	virtual void OnLeaveThisWidget(EUIStateChangedReason::Type Reason);
	// 蓝图版本。
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "On Leave This Widget"))
	void BP_OnLeaveThisWidget(EUIStateChangedReason::Type Reason);
};
