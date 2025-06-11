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
	virtual FReply OnFocusReceived(const FGeometry& InMyGeometry, const FFocusEvent& InFocusEvent) override
	{
		// 当用户在下拉菜单选中选项后引擎的Focus又会回到该Slate上，这个时候可以通过判断当前是否打开下拉菜单来确认我们是否要让Focus回到GameViewport上。
		if (!this->ShouldOpenDueToClick())
		{
			FReply Reply = SComboButton::OnButtonClicked();
			Reply.SetUserFocus(FSlateApplication::Get().GetGameViewport().ToSharedRef());
			return Reply;
		}

		return SComboButton::OnButtonClicked();
	}
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
