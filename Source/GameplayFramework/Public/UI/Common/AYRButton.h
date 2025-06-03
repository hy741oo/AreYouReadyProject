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

	// 重载键盘/手柄按下后的操作，目的是为了不触发“OnClicked”事件，而用Enhanced Input Action系统来处理玩家按下的操作。
	virtual FReply OnKeyDown( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent ) override;
	virtual FReply OnKeyUp( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent ) override;
};

USTRUCT()
struct FAYRButtonStyle : public FButtonStyle
{
	GENERATED_BODY()

	// 点击音效的ID。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AYRButton Appearance")
	FName PressedSoundID;

	// 悬停音效的ID。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AYRButton Appearance")
	FName HoveredSoundID;

};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonFocusReceivedBP, UAYRButton*, FocusedButton);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonFocusLostBP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSimpleAYRButtonDelegate, UAYRButton*, TheButton);

/**
 * 该UserWidget包装了一个按钮，在基础按钮上增加了“聚焦”效果，例如玩家聚焦该按钮时该按钮需要呈现效果。
 */
UCLASS(Blueprintable, BlueprintType)
class GAMEPLAYFRAMEWORK_API UAYRButton : public UButton
{

	GENERATED_BODY()

private:
	// 当前Button是否被选中。
	UPROPERTY(VisibleAnywhere, Category = "Interactive")
	bool bIsSelected = false;

	// 设置当前按钮的组ID。可以让同一ID的Button被划分为同一组，然后实现当一个按钮被选中时，另一个同组的先前被选中的按钮会自动取消选中的功能。默认值为-1，为-1时说明当前按钮不属于任何组，也就不会自动取消选中状态。
	UPROPERTY(EditAnywhere, Category = "Interactive")
	int32 ButtonGroupID = -1;

	// 已经注册了的ID和Widget映射。Key为组ID，Value为当前组ID中被选中的Widget。
	static TMap<int32, UAYRButton*> RegisteredGroups;

private:
	// 获取指定已经注册了的指定ID对应的Button。
	UAYRButton* GetCurrentRegisteredButton(const int32 InGroupID) const;

protected:
	// 替代原本绑定到SButton的版本，利于扩展。
	virtual FReply AYRSlateHandleClicked();
	virtual void AYRSlateHandlePressed();
	virtual void AYRSlateHandleReleased();
	virtual void AYRSlateHandleHovered();
	virtual void AYRSlateHandleUnhovered();

public:
	// 按钮通常状态下的Style。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AYRButton Appearance")
	FAYRButtonStyle NormalWidgetStyle;

	// 按钮被选中后的Style。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AYRButton Appearance")
	FAYRButtonStyle SelectedWidgetStyle;

	// 按钮Focus Received时调用。
	UPROPERTY(BlueprintAssignable, Category="Button|Event")
	FOnButtonFocusReceivedBP OnButtonFocusReceived;

	// 按钮Focus Lost时调用。
	UPROPERTY(BlueprintAssignable, Category="Button|Event")
	FOnButtonFocusLostBP OnButtonFocusLost;

	// 按钮选中时调用。
	UPROPERTY(BlueprintAssignable, Category="Button|Event")
	FSimpleAYRButtonDelegate OnButtonSelected;

	// 按钮取消选中时调用。
	UPROPERTY(BlueprintAssignable, Category="Button|Event")
	FSimpleAYRButtonDelegate OnButtonUnselected;

	// 是否让Button被Hover时就执行Select操作，或者只有在被点击时才执行Select操作。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AYRButton Appearance")
	bool bIsSelectWhenHovered = false;

public:
	UAYRButton(const FObjectInitializer& InObjectInitializer);

	// 更改属性后更新。
	virtual void SynchronizeProperties() override;

	// 构建我们的SButton。
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual FReply OnFocusReceived(const FGeometry& InMyGeometry, const FFocusEvent& InFocusEvent);

	virtual void OnFocusLost(const FFocusEvent& InFocusEvent);

	// 设置按钮样式，同时设置按钮音效。
	UFUNCTION(BlueprintCallable)
	virtual void SetAYRButtonStyle(FAYRButtonStyle Style);

	// 选中当前按钮。
	UFUNCTION(BlueprintCallable, Category = "Interactive")
	void Select();

	// 取消选中当前按钮。
	UFUNCTION(BlueprintCallable, Category = "Interactive")
	void Unselect();

	// 设置当前按钮的组ID。
	void SetButtonGroupID(const int32 InButtonGroupID);

	// 销毁时检测该Button是否已经被注册。
	virtual void BeginDestroy() override;
};
