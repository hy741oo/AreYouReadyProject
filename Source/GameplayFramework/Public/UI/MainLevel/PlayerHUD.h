// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/AYRUserWidget.h"
#include "PlayerHUD.generated.h"

class UInputIcon;

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UPlayerHUD : public UAYRUserWidget
{
	GENERATED_BODY()
	
protected:
	// 准星Widget。
	UPROPERTY(Meta = (BindWidget))
	UUserWidget* Crosshair = nullptr;

	// 可交互按键Icon。
	UPROPERTY(Meta = (BindWidget))
	UInputIcon* InteractionInputIcon = nullptr;

public:
	// 设置准星Widget的位置，用于实现动态准星的效果。
	void SetCrosshairPositionInScreen(const FVector2D& InPosition);

	// 进入可交互状态。
	void OnEnterInteractableState();

	// 退出可交互状态。
	void OnLeaveInteractableState();

	// 清理资产。
	virtual void NativeDestruct() override;

	// 设置交互Widget使用的图标文本字段。
	UFUNCTION(BlueprintCallable)
	void SetInputIconName(const FName InInputName);

	// 关闭ID卡提示图标，用于在最终关卡关闭显示。
	UFUNCTION(BlueprintImplementableEvent)
	void HideIDCardIcons();
};
