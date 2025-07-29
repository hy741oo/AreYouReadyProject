// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayMessageSubsystem.h"

#include "InputIcon.generated.h"

class SSizeBox;
class FDeferredCleanupSlateBrush;

/**
 * 通用的按键图标显示控件。通过表“DT_InputIconData”来配置需要显示的内容，再通过“InputIconName”来指定需要显示的按键信息。
 */
UCLASS(Blueprintable, BlueprintType)
class GAMEPLAYFRAMEWORK_API UInputIcon : public UWidget
{
	GENERATED_BODY()
	
private:
	// Button Icon的名称。即“DT_InputIconData”的RowName。
	UPROPERTY(EditAnywhere, Category = "Button Icon")
	FName InputIconName = "Confirm";

	// 全局消息接收句柄。
	FGMSListenerHandle OnInputDeviceChangedGMSHandle;

#if WITH_EDITOR
	TSharedPtr<FDeferredCleanupSlateBrush> TempSlateBrush;
#endif

private:
	// 根组件。
	TSharedPtr<SBorder> RootBorder;

	// 水平盒体组件。包含按键图标Image和可选的显示文本TextBlock。
	TSharedPtr<SHorizontalBox> InputIconHorizontalBox;

	// 控制按键Image大小。
	TSharedPtr<SBox> InputIconSizeBox;

	// 输入案件图标。
	TSharedPtr<SImage> InputIconImage;

	// 可选的按键提示文本。
	TSharedPtr<STextBlock> InputIconHintText;

private:
	// 处于游戏中而不是编辑器状态时调用，为了和UMG设计界面的逻辑区分开。
	void OnPlayInGame();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void OnWidgetRebuilt() override;

public:
	// 设置Button Icon的Name，同时更新Icon信息。
	UFUNCTION(BlueprintCallable, Category = "Button Icon")
	void SetInputIconName(const FName& InName);

	UFUNCTION(BlueprintCallable, Category = "Button Icon")
	FName GetInputIconName() const;

	UFUNCTION(BlueprintCallable, Category = "Button Icon")
	// 根据InputIconnName更新UI信息。
	void UpdateInputIcon();

	// 释放Slate资产。
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
};
