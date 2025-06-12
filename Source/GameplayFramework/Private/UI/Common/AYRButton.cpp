// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/AYRButton.h"

#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Audio/AudioManagerSubsystem.h"

TMap<int32, TWeakObjectPtr<UAYRButton>> UAYRButton::RegisteredGroups = TMap<int32, TWeakObjectPtr<UAYRButton>>();

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

FReply SAYRButton::OnKeyDown( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent )
{
	// 仅仅只做表现，不再让键盘/手柄响应“OnClicked”事件。

	if (IsEnabled() && FSlateApplication::Get().GetNavigationActionFromKey(InKeyEvent) == EUINavigationAction::Accept)
	{
		Press();
	}

	return FReply::Unhandled();
}

FReply SAYRButton::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	// 仅仅只做表现，不再让键盘/手柄响应“OnClicked”事件。

	if (IsEnabled() && FSlateApplication::Get().GetNavigationActionFromKey(InKeyEvent) == EUINavigationAction::Accept)
	{
		Release();
	}

	//return the constructed reply
	return FReply::Unhandled();
}

UAYRButton::UAYRButton(const FObjectInitializer& InObjectInitializer)
	:Super(InObjectInitializer)
{

}

void UAYRButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (this->bIsSelected)
	{
		this->SetAYRButtonStyle(this->SelectedWidgetStyle);
	}
	else
	{
		this->SetAYRButtonStyle(this->NormalWidgetStyle);
	}


}

TSharedRef<SWidget> UAYRButton::RebuildWidget()
{
	TSharedPtr<SAYRButton> AYRButton = SNew(SAYRButton)
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

	return FReply::Handled();
}

void UAYRButton::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	this->OnButtonFocusLost.Broadcast();
}

void UAYRButton::SetAYRButtonStyle(FAYRButtonStyle InStyle)
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

void UAYRButton::Select()
{
	if (!this->bIsSelected)
	{
		this->bIsSelected = true;

		this->SetAYRButtonStyle(this->SelectedWidgetStyle);

		// 处理组ID。
		if (this->ButtonGroupID != -1)
		{
			if (TWeakObjectPtr<UAYRButton>* Button = this->RegisteredGroups.Find(this->ButtonGroupID))
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

void UAYRButton::Unselect()
{
	if (this->bIsSelected)
	{
		this->bIsSelected = false;

		this->SetAYRButtonStyle(this->NormalWidgetStyle);

		// 处理组ID。
		TWeakObjectPtr<UAYRButton> RegisteredButton = this->GetCurrentRegisteredButton(this->ButtonGroupID);
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

void UAYRButton::SetButtonGroupID(const int32 InButtonGroupID)
{
	if (InButtonGroupID == this->ButtonGroupID) return;

	if (this->bIsSelected == true && InButtonGroupID != -1)
	{
		TWeakObjectPtr<UAYRButton> CurrentRegisteredButton = this->GetCurrentRegisteredButton(InButtonGroupID);
		if (CurrentRegisteredButton.IsValid())
		{
			CurrentRegisteredButton->Unselect();
		}
	}

	this->ButtonGroupID = InButtonGroupID;
}

TWeakObjectPtr<UAYRButton> UAYRButton::GetCurrentRegisteredButton(const int32 InGroupID) const
{
	TWeakObjectPtr<UAYRButton> Button = nullptr;

	if (InGroupID != -1)
	{
		if (TWeakObjectPtr<UAYRButton>* FoundButton = this->RegisteredGroups.Find(this->ButtonGroupID))
		{
			return *FoundButton;
		}
	}

	return Button;
}

void UAYRButton::ReleaseSlateResources(bool bInReleaseChildren)
{
	TWeakObjectPtr<UAYRButton> Button = this->GetCurrentRegisteredButton(this->ButtonGroupID);
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

FReply UAYRButton::AYRSlateHandleClicked()
{
	return this->SlateHandleClicked();
}

void UAYRButton::AYRSlateHandlePressed()
{
	if (!this->bIsSelectWhenHovered)
	{
		this->Select();
	}

	this->SlateHandlePressed();
}

void UAYRButton::AYRSlateHandleReleased()
{
	this->SlateHandleReleased();
}

void UAYRButton::AYRSlateHandleHovered()
{
	if (this->bIsSelectWhenHovered)
	{
		this->Select();
	}

	this->SlateHandleHovered();
}

void UAYRButton::AYRSlateHandleUnhovered()
{
	this->SlateHandleUnhovered();
}

void UAYRButton::ClearRegisteredButtons()
{
	TMap<int32, TWeakObjectPtr<UAYRButton>> Groups = RegisteredGroups;

	for (TPair<int32, TWeakObjectPtr<UAYRButton>> Element : Groups)
	{
		if (Element.Value.IsValid())
		{
			Element.Value->Unselect();
		}

		RegisteredGroups.Remove(Element.Key);
	}
}

