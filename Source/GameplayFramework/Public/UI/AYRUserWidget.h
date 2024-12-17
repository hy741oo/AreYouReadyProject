// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UISubsystem.h"

#include "AYRUserWidget.generated.h"

// UI被压栈时的原因。
UENUM(BlueprintType)
namespace EUIStateChangedReason
{
	enum Type
	{
		NewWidgetEntered UMETA(DisplayName = NewWidgetEntered),
		BePopped UMETA(DisplayName = BePoped)
	};
}


/**
 * 
 */
UCLASS()
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
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnEnterThisWidget(APlayerController* PlayerController, const FUIStackInfo UIStackInfo, EUIStateChangedReason::Type Reason);

	// 当该Widget不再成为Widget栈栈顶元素时调用。
	virtual void OnLeaveThisWidget(EUIStateChangedReason::Type Reason);
	// 蓝图版本。
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnLeaveThisWidget(EUIStateChangedReason::Type Reason);

	virtual void SetInputPriority(int32 Priority = 0);

	virtual void SetIsStopAction(bool bShouldBlock = false);
};
