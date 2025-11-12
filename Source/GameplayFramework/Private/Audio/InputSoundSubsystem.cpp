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
		// 顺序搜索。
		for (int32 Index = 0; Index < this->InputSoundStack.Num(); ++Index)
		{
			FInputSoundStackData& InputSoundStackData = this->InputSoundStack[Index];
			// 如果该层Input Sound被静音了则说明不需要播放输入音效，直接退出。
			if (InputSoundStackData.bMute)
			{
				return;
			}
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

bool UInputSoundSubsystem::PushInputSoundData(FName InInputSoundID, FInputSoundDataHandle& OutHandle, bool bInMute)
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
		StackData.bMute = bInMute;

		FInputSoundDataHandle Handle;
		// 如果当前按键音效栈为空的话，则直接插入数据。
		if (this->InputSoundStack.Num() == 0)
		{
			this->InputSoundStack.Add(StackData);
		}
		else
		{
			// 遍历数组，根据优先级来查找需要插入的Index。
			bool bIsFound = false;
			for (int32 CurrentIndex = 0; CurrentIndex < this->InputSoundStack.Num(); ++CurrentIndex)
			{
				FInputSoundStackData& ElementData = this->InputSoundStack[CurrentIndex];
				if (TableRow->Priority >= ElementData.DataTableRow->Priority)
				{
					this->InputSoundStack.Insert(StackData, CurrentIndex);
					bIsFound = true;
					break;
				}
			}
			if (bIsFound == false)
			{
				this->InputSoundStack.Add(StackData);
			}
		}
		Handle.CurrentID = CurrentUniqueID;
		OutHandle = Handle;

		bSuccessful = true;
	}
	else
	{
		UE_LOG(LogInputSoundSubsystem, Warning, TEXT("Can't find InputSoundID: %s"), *InInputSoundID.ToString());
	}

	return bSuccessful;
}

void UInputSoundSubsystem::PopInputSoundData(FInputSoundDataHandle& InHandle)
{
	// 删除Handle指定Index的按键音效数据。
	const int32 Index = this->InputSoundStack.IndexOfByPredicate(
		[&InHandle](const FInputSoundStackData& InElement) {
			return InHandle.CurrentID == InElement.UniqueID;
		}
	);

	if (Index != INDEX_NONE)
	{
		this->InputSoundStack.RemoveAt(Index);
	}
}

void UInputSoundSubsystem::MuteInputSound(const FInputSoundDataHandle& InHandle)
{
	FInputSoundStackData* StackData = this->InputSoundStack.FindByPredicate(
		[&InHandle](const FInputSoundStackData& InElement) {
			return InHandle.CurrentID == InElement.UniqueID;
		}
	);

	if (StackData)
	{
		StackData->bMute = true;
	}
}

void UInputSoundSubsystem::UnMuteInputSound(const FInputSoundDataHandle& InHandle)
{
	FInputSoundStackData* StackData = this->InputSoundStack.FindByPredicate(
		[&InHandle](const FInputSoundStackData& InElement) {
			return InHandle.CurrentID == InElement.UniqueID;
		}
	);

	if (StackData)
	{
		StackData->bMute = false;
	}
}

