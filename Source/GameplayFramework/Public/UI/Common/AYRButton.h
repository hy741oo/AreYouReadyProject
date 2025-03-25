// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "AYRButton.generated.h"

DECLARE_DELEGATE_RetVal_TwoParams(FReply, FOnButtonFocusReceived, const FGeometry&, const FFocusEvent&);
DECLARE_DELEGATE_OneParam(FOnButtonFocusLost, const FFocusEvent&);


class GAMEPLAYFRAMEWORK_API SAYRButton : public SButton
{
public:
	FOnButtonFocusReceived OnButtonFocusReceived;

	FOnButtonFocusLost OnButtonFocusLost;

public:
	void SetOnButtonFocusReceivedDelegate(FOnButtonFocusReceived InDelegate);

	void SetOnButtonFocusLostDelegate(FOnButtonFocusLost InDelegate);

public:
	virtual FReply OnFocusReceived(const FGeometry& InMyGeometry, const FFocusEvent& InFocusEvent) override;

	virtual void OnFocusLost( const FFocusEvent& InFocusEvent ) override;
};




DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonFocusReceivedBP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonFocusLostBP);

/**
 * 该UserWidget包装了一个按钮，在基础按钮上增加了“聚焦”效果，例如玩家聚焦该按钮时该按钮需要呈现效果。
 */
UCLASS(Blueprintable, BlueprintType)
class GAMEPLAYFRAMEWORK_API UAYRButton : public UButton
{

	GENERATED_BODY()

public:
	// 按钮通常状态下的Style。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance")
	FButtonStyle NormalWidgetStyle;

	// 按钮被Focus后的Style。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance")
	FButtonStyle FocusedWidgetStyle;

	// 按钮是否启用Focus表现。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	bool bEnableFocusAppearance = false;

	// 按钮Focus Received时调用。
	UPROPERTY(BlueprintAssignable, Category="Button|Event")
	FOnButtonFocusReceivedBP OnButtonFocusReceived;

	// 按钮Focus Lost时调用。
	UPROPERTY(BlueprintAssignable, Category="Button|Event")
	FOnButtonFocusLostBP OnButtonFocusLost;


public:
	UAYRButton(const FObjectInitializer& InObjectInitializer);

	// 更改属性后更新。
	virtual void SynchronizeProperties() override;

	// 构建我们的SButton。
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual FReply OnFocusReceived(const FGeometry& InMyGeometry, const FFocusEvent& InFocusEvent);

	virtual void OnFocusLost(const FFocusEvent& InFocusEvent);
};
