// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/OSButton.h"

#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Audio/AudioSubsystem.h"

TMap<int32, TWeakObjectPtr<UOSButton>> UOSButton::RegisteredGroups = TMap<int32, TWeakObjectPtr<UOSButton>>();

FReply SOSButton::OnFocusReceived(const FGeometry& InMyGeometry, const FFocusEvent& InFocusEvent)
{
	FReply ReturnReply = SBorder::OnFocusReceived(InMyGeometry, InFocusEvent);

	if (this->OnButtonFocusReceived.IsBound())
	{
		ReturnReply = this->OnButtonFocusReceived.Execute(InMyGeometry, InFocusEvent);
	}

	return ReturnReply;
}

void SOSButton::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	SBorder::OnFocusLost(InFocusEvent);

	this->OnButtonFocusLost.ExecuteIfBound(InFocusEvent);
}

void SOSButton::SetOnButtonFocusReceivedDelegate(FOnButtonFocusReceived InDelegate)
{
	this->OnButtonFocusReceived = InDelegate;
}

void SOSButton::SetOnButtonFocusLostDelegate(FOnButtonFocusLost InDelegate)
{
	this->OnButtonFocusLost = InDelegate;
}

FReply SOSButton::OnKeyDown( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent )
{
	// 仅仅只做表现，不再让键盘/手柄响应“OnClicked”事件。

	if (IsEnabled() && FSlateApplication::Get().GetNavigationActionFromKey(InKeyEvent) == EUINavigationAction::Accept)
	{
		Press();
	}

	return FReply::Unhandled();
}

FReply SOSButton::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	// 仅仅只做表现，不再让键盘/手柄响应“OnClicked”事件。

	if (IsEnabled() && FSlateApplication::Get().GetNavigationActionFromKey(InKeyEvent) == EUINavigationAction::Accept)
	{
		Release();
	}

	//return the constructed reply
	return FReply::Unhandled();
}

UOSButton::UOSButton(const FObjectInitializer& InObjectInitializer)
	:Super(InObjectInitializer)
{

}

void UOSButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (this->bIsSelected)
	{
		this->SetOSButtonStyle(this->SelectedWidgetStyle);
	}
	else
	{
		this->SetOSButtonStyle(this->NormalWidgetStyle);
	}


}

TSharedRef<SWidget> UOSButton::RebuildWidget()
{
	TSharedPtr<SOSButton> OSButton = SNew(SOSButton)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, AYRSlateHandleClicked))
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, AYRSlateHandlePressed))
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, AYRSlateHandleReleased))
		.OnHovered_UObject( this, &ThisClass::AYRSlateHandleHovered )
		.OnUnhovered_UObject( this, &ThisClass::AYRSlateHandleUnhovered )
		.ButtonStyle(&WidgetStyle)
		.ClickMethod(ClickMethod)
		.TouchMethod(TouchMethod)
		.PressMethod(PressMethod)
		.IsFocusable(IsFocusable)
		;

	if ( GetChildrenCount() > 0 )
	{
		Cast<UButtonSlot>(GetContentSlot())->BuildSlot(OSButton.ToSharedRef());
	}

	FOnButtonFocusReceived TempOnButtonFocusReceived;
	TempOnButtonFocusReceived.BindUObject(this, &UOSButton::OnFocusReceived);
	OSButton->SetOnButtonFocusReceivedDelegate(TempOnButtonFocusReceived);

	FOnButtonFocusLost TempOnButtonFocusLost;
	TempOnButtonFocusLost.BindUObject(this, &UOSButton::OnFocusLost);
	OSButton->SetOnButtonFocusLostDelegate(TempOnButtonFocusLost);

	this->MyButton = OSButton;
	
	return MyButton.ToSharedRef();
}

FReply UOSButton::OnFocusReceived(const FGeometry& InMyGeometry, const FFocusEvent& InFocusEvent)
{
	this->OnButtonFocusReceived.Broadcast(this);

	return FReply::Handled();
}

void UOSButton::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	this->OnButtonFocusLost.Broadcast();
}

void UOSButton::SetOSButtonStyle(FOSButtonStyle InStyle)
{
	this->SetStyle(InStyle);

	// 由于该部分代码会在Editor模式下被调用（如打开UMG Designer时），这个时候子系统还没有生成，所以需要判断一下。
	if (!this->IsDesignTime())
	{
		// 设置鼠标悬停音效。
		UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetGameInstance());
		FAudioManagerDataTableRow* TableRow = nullptr;
		if (!InStyle.HoveredSoundID.IsNone() && Config->GetDataTableRowFromID<FAudioManagerDataTableRow>(InStyle.HoveredSoundID, TableRow))
		{
			FSlateSound SlateSound;
			SlateSound.SetResourceObject(TableRow->SoundBase);
			TOptional<FSlateSound> ButtonSound = SlateSound;
			this->MyButton->SetHoveredSound(ButtonSound);
		}

		// 设置点击音效。
		TableRow = nullptr;
		if (!InStyle.HoveredSoundID.IsNone() && Config->GetDataTableRowFromID<FAudioManagerDataTableRow>(InStyle.PressedSoundID, TableRow))
		{
			FSlateSound SlateSound;
			SlateSound.SetResourceObject(TableRow->SoundBase);
			TOptional<FSlateSound> ButtonSound = SlateSound;
			this->MyButton->SetPressedSound(ButtonSound);
		}
	}
}

void UOSButton::Select()
{
	if (!this->bIsSelected)
	{
		this->bIsSelected = true;

		this->SetOSButtonStyle(this->SelectedWidgetStyle);

		// 处理组ID。
		if (this->ButtonGroupID != -1)
		{
			if (TWeakObjectPtr<UOSButton>* Button = this->RegisteredGroups.Find(this->ButtonGroupID))
			{
				if (Button->IsValid())
				{
					(*Button)->Unselect();
				}
			}

			this->RegisteredGroups.Add(this->ButtonGroupID, this);
		}

		this->OnButtonSelected.Broadcast(this);
	}
}

void UOSButton::Unselect()
{
	if (this->bIsSelected)
	{
		this->bIsSelected = false;

		this->SetOSButtonStyle(this->NormalWidgetStyle);

		// 处理组ID。
		TWeakObjectPtr<UOSButton> RegisteredButton = this->GetCurrentRegisteredButton(this->ButtonGroupID);
		if (RegisteredButton.IsValid())
		{
			if (RegisteredButton == this)
			{
				this->RegisteredGroups.Remove(this->ButtonGroupID);
			}
		}

		this->OnButtonUnselected.Broadcast(this);
	}
}

void UOSButton::SetButtonGroupID(const int32 InButtonGroupID)
{
	if (InButtonGroupID == this->ButtonGroupID) return;

	if (this->bIsSelected == true && InButtonGroupID != -1)
	{
		TWeakObjectPtr<UOSButton> CurrentRegisteredButton = this->GetCurrentRegisteredButton(InButtonGroupID);
		if (CurrentRegisteredButton.IsValid())
		{
			CurrentRegisteredButton->Unselect();
		}
	}

	this->ButtonGroupID = InButtonGroupID;
}

TWeakObjectPtr<UOSButton> UOSButton::GetCurrentRegisteredButton(const int32 InGroupID) const
{
	TWeakObjectPtr<UOSButton> Button = nullptr;

	if (InGroupID != -1)
	{
		if (TWeakObjectPtr<UOSButton>* FoundButton = this->RegisteredGroups.Find(this->ButtonGroupID))
		{
			return *FoundButton;
		}
	}

	return Button;
}

void UOSButton::ReleaseSlateResources(bool bInReleaseChildren)
{
	TWeakObjectPtr<UOSButton> Button = this->GetCurrentRegisteredButton(this->ButtonGroupID);
	if (Button.IsValid())
	{
		if (Button == this)
		{
			this->Unselect();
			
			this->RegisteredGroups.Remove(this->ButtonGroupID);
		}
	}

	Super::ReleaseSlateResources(bInReleaseChildren);
}

FReply UOSButton::AYRSlateHandleClicked()
{
	return this->SlateHandleClicked();
}

void UOSButton::AYRSlateHandlePressed()
{
	if (!this->bIsSelectWhenHovered)
	{
		this->Select();
	}

	this->SlateHandlePressed();
}

void UOSButton::AYRSlateHandleReleased()
{
	this->SlateHandleReleased();
}

void UOSButton::AYRSlateHandleHovered()
{
	if (this->bIsSelectWhenHovered)
	{
		this->Select();
	}

	this->SlateHandleHovered();
}

void UOSButton::AYRSlateHandleUnhovered()
{
	this->SlateHandleUnhovered();
}

void UOSButton::ClearRegisteredButtons()
{
	TMap<int32, TWeakObjectPtr<UOSButton>> Groups = RegisteredGroups;

	for (TPair<int32, TWeakObjectPtr<UOSButton>> Element : Groups)
	{
		if (Element.Value.IsValid())
		{
			Element.Value->Unselect();
		}

		RegisteredGroups.Remove(Element.Key);
	}
}

