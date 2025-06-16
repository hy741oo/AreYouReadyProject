// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ComboBoxString.h"
#include "Widgets/SViewport.h"

#include "AYRComboBox.generated.h"

/**
 * 自定义的SComboBox。
 */
template< typename OptionType >
class GAMEPLAYFRAMEWORK_API SAYRComboBox : public SComboBox<OptionType>
{

public:
	// 当我们设置Focus时就开始让ComboBox开启。
	virtual FReply OnFocusReceived(const FGeometry& InMyGeometry, const FFocusEvent& InFocusEvent) override;

	// 绑定下拉菜单开关事件。
	void BindOnMenuOpenStateChangedDelegate();

	// 开关下拉菜单的回调事件。
	virtual void OnMenuOpenStateChanged(bool bInIsOpened);
};

/**
 * 自定义的ComboBox控件。
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UAYRComboBox : public UComboBoxString
{
	GENERATED_BODY()

private:
	// 保存构建后的新ComboBox。
	TSharedPtr<SAYRComboBox<TSharedPtr<FString>>> MyAYRComboBox;
	
public:
	// 构建我们的SAYRComboBox。
	virtual TSharedRef<SWidget> RebuildWidget() override;

	// 清理新建的ComboBox。
	virtual void ReleaseSlateResources(bool bInReleaseChildren) override;
};
