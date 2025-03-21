// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AYRButton.generated.h"

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonClicked);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonClicked);

/**
 * 该UserWidget包装了一个按钮，在基础按钮上增加了“聚焦”效果，例如玩家聚焦该按钮时该按钮需要呈现效果。
 */
UCLASS(Blueprintable, BlueprintType)
class GAMEPLAYFRAMEWORK_API UAYRButton : public UUserWidget
{

	GENERATED_BODY()

private:
	UButton* OwningButton;
	
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

	UPROPERTY(BlueprintAssignable)
	FOnButtonClicked OnButtonClicked;

public:
	// 构建内部控件。
	virtual bool Initialize() override;

	// 更改属性后更新。
	virtual void SynchronizeProperties() override;

	// Widget进入Focus Path时更改为Focus表现。
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent);

	// Widget离开Focus Path时更改为普通表现。
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent);

	//virtual FReply NativeOnFocusReceived( const FGeometry& InGeometry, const FFocusEvent& InFocusEvent ) override;
	virtual void NativeOnFocusLost( const FFocusEvent& InFocusEvent ) override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;
	//virtual void NativeOnButtonClicked();
	//virtual void NativeOnButtonPressed();
	//virtual void NativeOnButtonReleased();
};
