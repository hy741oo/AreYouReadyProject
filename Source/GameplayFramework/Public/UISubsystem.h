// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameConfigSubsystem.h"

#include "UISubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUISubsystem, Log, All);

// 玩家输入类型。
UENUM(BlueprintType)
namespace EUIInputMode
{
	enum Type
	{
		IM_UIOnly UMETA(DisplayName = "UI Only"),
		IM_GameOnly UMETA(DisplayName = "Game Only"),
		IM_GameAndUI UMETA(DisplayName = "Game and UI")
	};
}

// UI状态信息。即每次激活UI时需要设置的
USTRUCT(BlueprintType)
struct FUIStateInfoTableRow
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input Mode")
	TEnumAsByte<EUIInputMode::Type> InputMode = EUIInputMode::IM_GameAndUI;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input Mode", Meta = (EditCondition = "InputMode == EUIInputMode::IM_GameAndUI || InputMode == EUIInputMode::IM_UIOnly"))
	EMouseLockMode MouseLockMode = EMouseLockMode::DoNotLock;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input Mode", Meta = (EditCondition = "InputMode == EUIInputMode::IM_GameAndUI"))
	bool bHideCursorDuringCapture = true;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bShowMouseCursor = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 ZOrder = -1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bStopAction = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 Priority = 0;
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


/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UUISubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

private:
	TArray<FUIStackInfo> UIStack;

public:
	// 新建一个UI，并压入到UI栈里。
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	UAYRUserWidget* PushUI(FName UIID);

	// 弹出指定UI，如果bPopWith该操作会连同在指定UI之后压栈的UI按照顺序弹出。如果没有指定UI则弹出栈顶。
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void PopUI(const UAYRUserWidget* InSpecifiedUI = nullptr);

	// 对指定的UI应用状态信息。
	void ApplyUIInfo(APlayerController* InPlayerController, const FUIStackInfo* InUIStackInfo);

};
