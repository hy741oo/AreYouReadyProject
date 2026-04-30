// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameConfigSubsystem.h"
#include "Blueprint/UserWidget.h"

#include "UISubsystem.generated.h"

class UGameViewportClient;
class UCanvasPanel;
class FActorsInitializedParams;

DECLARE_LOG_CATEGORY_EXTERN(LogUISubsystem, Log, All);

/*
  UI层级。生成UI时可以指定生成在哪一层级上，用于更好组织UI层级之间的关系，防止出现UI意外覆盖的情况。
  UI层级是根据定义时的顺序指定的，例如Gameplay是第一层，Interface是第二层，那么添加到Gameplay层的UI总是最先渲染的，然后才会开始渲染Interface层的内容，也就是说Interface层的内容*总是*显示在Gameplay层之上，会覆盖Gameplay层的内容（更靠近玩家），然后同一层级之间的UI则使用ZOrder来指定顺序。
  **需要注意的是**这个层级并不是UI管理系统栈里面的层级，UI栈层级是UI生成时候的顺序，方便UI退栈找到之前的UI，该层级仅仅影响UI渲染的顺序。
*/
UENUM(BlueprintType)
namespace EUILayer
{
	enum Type
	{
		// Gameplay层。用于显示玩法相关的UI，如怪物头顶血条、伤害跳字等和游戏强关联的UI。
		UIL_Gameplay UMETA(DisplayName = "Gameplay"),
		// Interface层。用于显示玩家HUD上需要显示的内容，如玩家血条、枪械子弹数量、交互按钮等UI，相比于Gameplay层该层内容并不那么“靠近”游戏场景里，所以作为第二层展示。
		UIL_Interface UMETA(DisplayName = "Interface"),
		// System层。用于显示暂停菜单等和游戏本身关联性不强的UI。
		UIL_System UMETA(DisplayName = "System"),
		// 仅用于计数。
		MAX
	};
}
ENUM_RANGE_BY_COUNT(EUILayer::Type, EUILayer::Type::MAX);

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
};

// UI管理系统栈需要用到的信息
USTRUCT(BlueprintType)
struct FUIStackInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	UOSUserWidget* UserWidget = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FUIStateInfoTableRow UIStateInfo;
};

// UI信息。
USTRUCT(BlueprintType)
struct FUIInfoTableRow : public FAYRTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UOSUserWidget> UIClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName UIName = NAME_None;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FUIStateInfoTableRow UIStateInfo;

	// UI显示的层级。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 ZOrder = 0;

	// UI所在的层。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TEnumAsByte<EUILayer::Type> Layer = EUILayer::UIL_Gameplay;
};

// 输入模式类。用于策略模式设置引擎的输入模式。
struct  FAYRInputModeData : public FInputModeDataBase
{
	// UI输入信息。
	FUIStateInfoTableRow UIStateInfo;

	// 实际应用InputMode的接口。
	virtual void ApplyInputMode(FReply& SlateOperations, UGameViewportClient& GameViewportClient) const override;
};

// UI层级类。用作UI每一层结构的根控件。
UCLASS()
class GAMEPLAYFRAMEWORK_API ULayerWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	// 作为根控件的画板，单独存储变量方便后续添加子控件。
	UPROPERTY()
	UCanvasPanel* RootCanvasPanel = nullptr;

public:
	// 初始化层级控件。
	virtual void NativeOnInitialized() override;

	// 在当前层级里添加子UI。
	void AddUserWidget(UUserWidget* InWidget, int32 InZOurder);
};

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UUISubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

	friend UOSUserWidget;

private:
	// UI栈。生成的UI全部都会并入这个栈里进行管理。
	TArray<FUIStackInfo> UIStack;

	// 清理的委托句柄。
	FDelegateHandle CleanDelegateHandle;

	// 当PlayerController初始化后调用的委托的句柄。
	FDelegateHandle OnPlayerControllerInitializedDelegateHandle;

	// 初始化界面的输入设置。
	void ResetInputSetting() const;

	// 维护生成的UI层，后续添加UI时方便找到对应层级。需要管理GC。
	UPROPERTY()
	TMap<TEnumAsByte<EUILayer::Type>, ULayerWidget*> UILayers;

private:
	// 对指定的UI应用状态信息。
	void ApplyUIInfo(APlayerController* InPlayerController, const FUIStackInfo* InUIStackInfo);
public:
	virtual void Initialize(FSubsystemCollectionBase& InCollection) override;

	virtual void Deinitialize() override;

	// 当PlayerController初始化完成后调用。由于LocalPlayer在引擎启动早期就会创建，因此部分逻辑写在子系统的Initialize函数里会拿不到需要的Actor（如PlayerController），所以用一个这样的回调可以获取创建并初始化后的PlayerController。
	void OnPlayerControllerInitialized(AGameModeBase* InGameMode, APlayerController* InNewPlayerController);
public:
	// 新建一个UI，并压入到UI栈里。
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	UOSUserWidget* PushUI(FName UIID);

	// 弹出指定UI，如果bPopWith该操作会连同在指定UI之后压栈的UI按照顺序弹出。如果没有指定UI则弹出栈顶。
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void PopUI(const UOSUserWidget* InSpecifiedUI = nullptr);

	// 清理目前全部UI，并且将Viewport设置为初始状态。
	void ClearUIStack() ;

};
