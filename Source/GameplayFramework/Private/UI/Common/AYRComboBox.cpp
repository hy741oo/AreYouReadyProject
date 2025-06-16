// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/AYRComboBox.h"

template<typename OptionType>
FReply SAYRComboBox<OptionType>::OnFocusReceived(const FGeometry& InMyGeometry, const FFocusEvent& InFocusEvent)
{
	if (SMenuAnchor::ShouldOpenDueToClick())
	{
		// 模拟手柄按下事件。
		FKeyEvent KeyEvent(EKeys::Gamepad_FaceButton_Bottom, FModifierKeysState(), 0, false, 0, 0);
		FSlateApplication::Get().ProcessKeyDownEvent(KeyEvent);
	}
	else
	{
		// 让Focus切换到GameViewport上。
		FSlateApplication::Get().SetAllUserFocusToGameViewport();
	}

	return FReply::Handled();
}

template<typename OptionType>
void SAYRComboBox<OptionType>::BindOnMenuOpenStateChangedDelegate()
{
	SMenuAnchor::OnMenuOpenChanged.BindRaw(this, &SAYRComboBox::OnMenuOpenStateChanged);
}

template<typename OptionType>
void SAYRComboBox<OptionType>::OnMenuOpenStateChanged(bool bInIsOpened)
{
	// 当下拉菜单关闭时让Focus切换到GameViewport上，让Enhanced Input能够继续保持输入处理。
	if (!bInIsOpened)
	{
		FSlateApplication::Get().SetAllUserFocusToGameViewport();
	}
}

TSharedRef<SWidget> UAYRComboBox::RebuildWidget()
{
	int32 InitialIndex = FindOptionIndex(this->GetSelectedOption());
	if ( InitialIndex != -1 )
	{
		CurrentOptionPtr = Options[InitialIndex];
	}

	this->MyAYRComboBox = SNew(SAYRComboBox< TSharedPtr<FString> >)
		.ComboBoxStyle(&WidgetStyle)
		.ItemStyle(&ItemStyle)
		.ForegroundColor(ForegroundColor)
		.OptionsSource(&Options)
		.InitiallySelectedItem(CurrentOptionPtr)
		.ContentPadding(ContentPadding)
		.MaxListHeight(MaxListHeight)
		.HasDownArrow(HasDownArrow)
		.EnableGamepadNavigationMode(EnableGamepadNavigationMode)
		.OnGenerateWidget(BIND_UOBJECT_DELEGATE(SComboBox< TSharedPtr<FString> >::FOnGenerateWidget, HandleGenerateWidget))
		.OnSelectionChanged(BIND_UOBJECT_DELEGATE(SComboBox< TSharedPtr<FString> >::FOnSelectionChanged, HandleSelectionChanged))
		.OnComboBoxOpening(BIND_UOBJECT_DELEGATE(FOnComboBoxOpening, HandleOpening))
		.IsFocusable(bIsFocusable)
		[
			SAssignNew(ComboBoxContent, SBox)
		];

	if ( InitialIndex != -1 )
	{
		// Generate the widget for the initially selected widget if needed
		UpdateOrGenerateWidget(CurrentOptionPtr);
	}

	this->MyAYRComboBox->BindOnMenuOpenStateChangedDelegate();

	this->MyComboBox = this->MyAYRComboBox;

	return MyComboBox.ToSharedRef();
}

void UAYRComboBox::ReleaseSlateResources(bool bInReleaseChildren)
{
	this->MyAYRComboBox.Reset();

	Super::ReleaseSlateResources(bInReleaseChildren);
}


