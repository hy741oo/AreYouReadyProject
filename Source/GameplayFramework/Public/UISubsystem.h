// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameConfigSubsystem.h"

#include "UISubsystem.generated.h"

class UGameViewportClient;

DECLARE_LOG_CATEGORY_EXTERN(LogUISubsystem, Log, All);

// UI状态信息。即每次激活UI时需要设置的
USTRUCT(BlueprintType)
struct FUIStateInfoTableRow
{
	GENERATED_BODY()

	// 鼠标锁定类型。表明鼠标是会锁定在窗口内还是可以随意移动。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input Mode")
	EMouseLockMode MouseLockMode = EMouseLockMode::DoNotLock;

	// 鼠标捕获类型。表明鼠标从窗口外移动到窗口内时执行什么操作会被窗口捕获。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input Mode")
	EMouseCaptureMode MouseCaptureMode = EMouseCaptureMode::CapturePermanently;

	// 是否显示鼠标。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bShowMouseCursor = true;

	// 鼠标被捕获后是否隐藏鼠标。true为捕获时隐藏。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input Mode", Meta = (EditCondition = "bShowMouseCursor == true"))
	bool bHideCursorDuringCapture = true;

	// UI显示的层级。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 ZOrder = -1;
};

// UI管理系统栈需要用到的信息
USTRUCT(BlueprintType)
struct FUIStackInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	UAYRUserWidget* UserWidget = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FUIStateInfoTableRow UIStateInfo;
};

// UI信息。
USTRUCT(BlueprintType)
struct FUIInfoTableRow : public FAYRTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UAYRUserWidget> UIClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName UIName = NAME_None;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FUIStateInfoTableRow UIStateInfo;
};

// 输入模式类。用于策略模式设置引擎的输入模式。
struct  FAYRInputModeData : public FInputModeDataBase
{
	// UI输入信息。
	FUIStateInfoTableRow UIStateInfo;

	// 实际应用InputMode的接口。
	virtual void ApplyInputMode(FReply& SlateOperations, UGameViewportClient& GameViewportClient) const override;
};


/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend UAYRUserWidget;

private:
	TArray<FUIStackInfo> UIStack;

	// 清理委托句柄。
	FDelegateHandle CleanDelegateHandle;

	// 输入设备切换时注册的消息句柄。
	FGMSListenerHandle InputDeviceMessageHandle;

	// 初始化界面的输入设置。
	void ResetInputSetting() const;

private:
	// 对指定的UI应用状态信息。
	void ApplyUIInfo(APlayerController* InPlayerController, const FUIStackInfo* InUIStackInfo);

	// 当输入设备切换时调用的函数。
	void OnInputDeviceChanged(UGMSMessageBase* InMessage);

	// 当玩家输入设备切换到手柄时需要让栈顶UI响应的操作。
	void OnInputDeviceChangeIntoGamepad(APlayerController* InPlayerController);
public:
	virtual void Initialize(FSubsystemCollectionBase& InCollection) override;

	virtual void Deinitialize() override;

public:
	// 新建一个UI，并压入到UI栈里。
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	UAYRUserWidget* PushUI(FName UIID);

	// 弹出指定UI，如果bPopWith该操作会连同在指定UI之后压栈的UI按照顺序弹出。如果没有指定UI则弹出栈顶。
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void PopUI(const UAYRUserWidget* InSpecifiedUI = nullptr);

	// 清理目前全部UI，并且将Viewport设置为初始状态。
	void ClearUIStack() ;

};
