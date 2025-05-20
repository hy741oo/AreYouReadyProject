// Fill out your copyright notice in the Description page of Project Settings.


#include "Audio/InputSoundSubsystem.h"

#include "GameplayMessageSubsystem.h"
#include "Audio/AudioManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogInputSoundSubsystem)

void UInputSoundSubsystem::Initialize(FSubsystemCollectionBase& InCollection)
{
	// 注册响应按键的回调事件。
	UGameplayMessageSubsystem* GMS = UGameInstance::GetSubsystem<UGameplayMessageSubsystem>(this->GetGameInstance());
	FGameplayTag Tag = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("GMSMessage.System.Input.HandleKey"));
	FOnMessageReceived Callback;
	Callback.BindUObject(this, &UInputSoundSubsystem::OnHandledInputKey);
	this->GMSHandle = GMS->Register(Tag, Callback);

	// 保存音频子系统，后续不再需要频繁获取。
	if (!this->AudioManager)
	{
		this->AudioManager = UGameInstance::GetSubsystem<UAudioManagerSubsystem>(this->GetGameInstance());
	}
}

void UInputSoundSubsystem::Deinitialize()
{
	// 注销GMS的回调事件。
	UGameplayMessageSubsystem* GMS = UGameInstance::GetSubsystem<UGameplayMessageSubsystem>(this->GetGameInstance());
	FGameplayTag Tag = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("GMSMessage.System.Input.HandleKey"));
	GMS->Unregister(this->GMSHandle);

	this->AudioManager = nullptr;
}

void UInputSoundSubsystem::OnHandledInputKey(UGMSMessageBase* InMessage)
{
	if (UGMSHandledKey* Msg = Cast<UGMSHandledKey>(InMessage))
	{
		// 逆序搜索。
		for (int32 Index = this->InputSoundStack.Num() - 1; Index >= 0; --Index)
		{
			FInputSoundStackData& InputSoundStackData = this->InputSoundStack[Index];
			FInputSoundTableRow* InputSoundTableRow = InputSoundStackData.DataTableRow;
			if (InputSoundTableRow->KeySoundMap.Contains(Msg->HandledKey))
			{
				FName SoundName = InputSoundTableRow->KeySoundMap[Msg->HandledKey];
				this->AudioManager->PlaySound2D(SoundName);
				break;
			}
			else
			{
				// 栈顶并不包含我们要处理的按键，判断是否继续往栈的下层搜索。
				if (InputSoundTableRow->bHandled)
				{
					// 停止遍历，不播放任何音效。
					break;
				}
				else
				{
					// 继续遍历。
					continue;
				}
			}
		}
	}
}

bool UInputSoundSubsystem::PushInputSoundData(FName InInputSoundID, FInputSoundDataHandle& OutHandle)
{
	UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetGameInstance());
	FInputSoundTableRow* TableRow = nullptr;
	bool bSuccessful = false;

	if (Config->GetDataTableRowFromID<FInputSoundTableRow>(InInputSoundID, TableRow))
	{
		int32 CurrentUniqueID = ++this->UniqueID;

		FInputSoundStackData StackData;
		StackData.DataTableRow = TableRow;
		StackData.UniqueID = CurrentUniqueID;

		FInputSoundDataHandle Handle;
		Handle.StackIndex = this->InputSoundStack.Add(StackData);
		Handle.CurrentID = CurrentUniqueID;
		OutHandle = Handle;
	}
	else
	{
		UE_LOG(LogInputSoundSubsystem, Warning, TEXT("Can't find InputSoundID: %s"), *InInputSoundID.ToString());
	}

	return bSuccessful;
}

void UInputSoundSubsystem::PopInputSoundData(FInputSoundDataHandle& Handle)
{
	while (this->InputSoundStack.Num() > 0)
	{
		FInputSoundStackData StackData = this->InputSoundStack.Pop();
		if (StackData.UniqueID == Handle.CurrentID)
		{
			return;
		}
	}
}

