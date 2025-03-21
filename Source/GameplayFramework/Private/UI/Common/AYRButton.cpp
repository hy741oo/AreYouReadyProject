// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/AYRButton.h"

#include "Components/Button.h"
#include "Blueprint/WidgetTree.h"

bool UAYRButton::Initialize()
{
	bool bIsSuccessful = Super::Initialize();

	// 生成按钮。
	check(this->OwningButton = this->WidgetTree->ConstructWidget<UButton>(UButton::StaticClass()));

	this->OwningButton->SetStyle(this->NormalWidgetStyle);

	this->WidgetTree->RootWidget = this->OwningButton;

	//this->OwningButton->OnClicked

	return bIsSuccessful;
}

void UAYRButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (this->bEnableFocusAppearance && this->HasAnyUserFocus())
	{
		// 当该Button启用Focus表现和被Focus的时候启用Focus Style...
		this->OwningButton->SetStyle(this->FocusedWidgetStyle);
	}
	else
	{
		// ...否则使用原本的Style。
		this->OwningButton->SetStyle(this->NormalWidgetStyle);
	}
}

void UAYRButton::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);

	if (this->bEnableFocusAppearance)
	{
		this->OwningButton->SetStyle(this->FocusedWidgetStyle);
	}
	else
	{
		this->OwningButton->SetStyle(this->NormalWidgetStyle);
	}
}

void UAYRButton::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);

	this->OwningButton->SetStyle(this->NormalWidgetStyle);

}

//FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
//{
//}

void UAYRButton::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{

}

void UAYRButton::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{

}


