// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/AYRComboBox.h"

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

	this->MyComboBox = this->MyAYRComboBox;

	return MyComboBox.ToSharedRef();
}

void UAYRComboBox::ReleaseSlateResources(bool bInReleaseChildren)
{
	this->MyAYRComboBox.Reset();

	Super::ReleaseSlateResources(bInReleaseChildren);
}


