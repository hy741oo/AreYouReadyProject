// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/AYRButton.h"

#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Blueprint/WidgetTree.h"

FReply SAYRButton::OnFocusReceived(const FGeometry& InMyGeometry, const FFocusEvent& InFocusEvent)
{
	FReply ReturnReply = SBorder::OnFocusReceived(InMyGeometry, InFocusEvent);

	if (this->OnButtonFocusReceived.IsBound())
	{
		ReturnReply = this->OnButtonFocusReceived.Execute(InMyGeometry, InFocusEvent);
	}

	return ReturnReply;
}

void SAYRButton::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	SBorder::OnFocusLost(InFocusEvent);

	this->OnButtonFocusLost.ExecuteIfBound(InFocusEvent);
}

void SAYRButton::SetOnButtonFocusReceivedDelegate(FOnButtonFocusReceived InDelegate)
{
	this->OnButtonFocusReceived = InDelegate;
}

void SAYRButton::SetOnButtonFocusLostDelegate(FOnButtonFocusLost InDelegate)
{
	this->OnButtonFocusLost = InDelegate;
}


UAYRButton::UAYRButton(const FObjectInitializer& InObjectInitializer)
	:Super(InObjectInitializer)
{

}

void UAYRButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (this->bEnableFocusAppearance && this->HasAnyUserFocus())
	{
		// 当该Button启用Focus表现和被Focus的时候启用Focus Style...
		this->SetStyle(this->FocusedWidgetStyle);
	}
	else
	{
		// ...否则使用原本的Style。
		this->SetStyle(this->NormalWidgetStyle);
	}
}

TSharedRef<SWidget> UAYRButton::RebuildWidget()
{
	TSharedPtr<SAYRButton> AYRButton = SNew(SAYRButton)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClicked))
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressed))
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleased))
		.OnHovered_UObject( this, &ThisClass::SlateHandleHovered )
		.OnUnhovered_UObject( this, &ThisClass::SlateHandleUnhovered )
		.ButtonStyle(&WidgetStyle)
		.ClickMethod(ClickMethod)
		.TouchMethod(TouchMethod)
		.PressMethod(PressMethod)
		.IsFocusable(IsFocusable)
		;

	if ( GetChildrenCount() > 0 )
	{
		Cast<UButtonSlot>(GetContentSlot())->BuildSlot(AYRButton.ToSharedRef());
	}

	FOnButtonFocusReceived TempOnButtonFocusReceived;
	TempOnButtonFocusReceived.BindUObject(this, &UAYRButton::OnFocusReceived);
	AYRButton->SetOnButtonFocusReceivedDelegate(TempOnButtonFocusReceived);

	FOnButtonFocusLost TempOnButtonFocusLost;
	TempOnButtonFocusLost.BindUObject(this, &UAYRButton::OnFocusLost);
	AYRButton->SetOnButtonFocusLostDelegate(TempOnButtonFocusLost);

	this->MyButton = AYRButton;
	
	return MyButton.ToSharedRef();
}

FReply UAYRButton::OnFocusReceived(const FGeometry& InMyGeometry, const FFocusEvent& InFocusEvent)
{
	this->OnButtonFocusReceived.Broadcast(this);

	if (this->bEnableFocusAppearance)
	{
		this->SetStyle(this->FocusedWidgetStyle);
	}
	else
	{
		this->SetStyle(this->NormalWidgetStyle);
	}

	return FReply::Handled();
}

void UAYRButton::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	this->OnButtonFocusLost.Broadcast();

	this->SetStyle(this->NormalWidgetStyle);
}

