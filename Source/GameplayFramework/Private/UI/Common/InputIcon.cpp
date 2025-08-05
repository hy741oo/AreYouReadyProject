// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/InputIcon.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Widgets/Images/SImage.h"
#include "Components/TextBlock.h"
#if WITH_EDITOR
#include "Slate/DeferredCleanupSlateBrush.h"
#endif
#include "GameConfigSubsystem.h"

void UInputIcon::SetInputIconName(const FName& InName)
{
	this->InputIconName = InName;

	this->UpdateInputIcon();
}

FName UInputIcon::GetInputIconName() const
{
	return this->InputIconName;
}

void UInputIcon::UpdateInputIcon()
{
	UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetGameInstance());
	const FInputIconDataTableRow* InputIconTableRow = nullptr;
	const FSlateBrush* InputIconImageBrush = nullptr;
	if (Config->GetCurrentInputIconData(this->InputIconName, InputIconTableRow, InputIconImageBrush))
	{
		this->InputIconHorizontalBox->SetVisibility(EVisibility::HitTestInvisible);

		this->InputIconImage->SetImage(InputIconImageBrush);

		// 是否需要显示提示文本。
		if (InputIconTableRow->bUseIconHintText)
		{
			this->InputIconHintText->SetVisibility(EVisibility::HitTestInvisible);
			this->InputIconHintText->SetText(InputIconTableRow->IconText);
			this->InputIconHintText->SetFont(InputIconTableRow->IconTextFontInfo);
		}
		else
		{
			this->InputIconHintText->SetVisibility(EVisibility::Collapsed);
		}
	}
	else
	{
		// 搜到了TableRow但是没有搜索到对应Input Key的图标信息，根据bCollapseOnIconHidden设置控件隐藏的方式。
		if (InputIconTableRow && InputIconTableRow->bCollapseOnIconHidden)
		{
			this->InputIconHorizontalBox->SetVisibility(EVisibility::Collapsed);
		}
		else
		{
			this->InputIconHorizontalBox->SetVisibility(EVisibility::HitTestInvisible);
		}
	}
}

TSharedRef<SWidget> UInputIcon::RebuildWidget()
{
	this->RootBorder = 
		SNew(SBorder)
		.Padding(FMargin(.0f))
		.BorderImage(FStyleDefaults::GetNoBrush())
		[
			SAssignNew(this->InputIconHorizontalBox, SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SAssignNew(this->InputIconSizeBox, SBox)
				.WidthOverride(64.f)
				.HeightOverride(64.f)
				[
					SAssignNew(this->InputIconImage, SImage)
				]
			]
			+ SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SAssignNew(this->InputIconHintText, STextBlock)
			]
		]
	;

	return this->RootBorder.ToSharedRef();
}

void UInputIcon::OnWidgetRebuilt()
{
#if WITH_EDITOR
	if (this->IsDesignTime())
	{
		// 在UMG编辑界面里预览使用。
		this->SetVisibility(ESlateVisibility::HitTestInvisible);
		UTexture2D* IconImage = LoadObject<UTexture2D>(this, TEXT("Texture2D'/Game/AYR/Textures/InputDeviceIcon/Keyboard___Mouse/Dark/E_Key_Dark.E_Key_Dark'"));
		this->TempSlateBrush = FDeferredCleanupSlateBrush::CreateBrush(IconImage);
		this->InputIconImage->SetImage(this->TempSlateBrush->GetSlateBrush());
		FString HintText = TEXT("交互");
		this->InputIconHintText->SetText(FText::FromString(HintText));
		UObject* FontObject = LoadObject<UObject>(this, TEXT("Font'/Engine/EngineFonts/Roboto.Roboto'"));
		FSlateFontInfo Font(FontObject, 20);
		Font.TypefaceFontName = "Regular";
		this->InputIconHintText->SetFont(Font);
	}
	else
	{
		this->OnPlayInGame();
	}
#else
	this->OnPlayInGame();
#endif
}

void UInputIcon::ReleaseSlateResources(bool bInReleaseChildren)
{
	Super::ReleaseSlateResources(bInReleaseChildren);

	this->InputIconHorizontalBox.Reset();
	this->InputIconSizeBox.Reset();
	this->InputIconImage.Reset();
	this->InputIconHintText.Reset();
	this->RootBorder.Reset();

	// 注销输入设备切换事件。
	UGameplayMessageSubsystem* GMS = UGameInstance::GetSubsystem<UGameplayMessageSubsystem>(this->GetGameInstance());
	if (this->OnInputDeviceChangedGMSHandle.IsValid())
	{
		GMS->Unregister(this->OnInputDeviceChangedGMSHandle);
	}
}

void UInputIcon::OnPlayInGame()
{
	this->UpdateInputIcon();

	// 注册输入设备切换事件。
	UGameplayMessageSubsystem* GMS = UGameInstance::GetSubsystem<UGameplayMessageSubsystem>(this->GetGameInstance());
	FGameplayTag Tag = FGameplayTag::RequestGameplayTag("GMSMessage.System.Input.DeviceType");
	FOnMessageReceived Callback;
	Callback.BindWeakLambda(this,
		[this](const UGMSMessageBase* InMessage) {
			this->UpdateInputIcon();
		}
	);
	this->OnInputDeviceChangedGMSHandle = GMS->Register(Tag, Callback);
}

