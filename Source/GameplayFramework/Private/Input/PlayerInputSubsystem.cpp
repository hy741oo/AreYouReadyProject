// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/PlayerInputSubsystem.h"

#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"

DEFINE_LOG_CATEGORY(LogPlayerInputSubsystem);

void UPlayerInputSubsystem::AddPlayerInputMappingContext(FName InInputMappingContextID)
{
	APlayerController* CurrentPlayerController = this->GetLocalPlayer()->GetPlayerController(this->GetWorld());

	// 合法性检查。
	if (InInputMappingContextID.IsNone() || CurrentPlayerController == nullptr)
	{
		return;
	}

	// 绑定Input Mapping Context。
	if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(CurrentPlayerController->GetLocalPlayer()))
	{
		// 通过InputMappingContextID获取对应的InputMappingContext。
		UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(CurrentPlayerController->GetGameInstance());
		FPlayerInputMappingContextTableRow* InputMappingContextTableRow = nullptr;
		if (Config->GetDataTableRowFromID<FPlayerInputMappingContextTableRow>(InInputMappingContextID, InputMappingContextTableRow))
		{
			// 绑定MappingContext。

			TSet<FName>* AddedIDs = this->AddedIMCIDs.Find(InputMappingContextTableRow->Level);
			// 当IMC不存在时才添加，防止重复添加。
			if (!AddedIDs || !AddedIDs->Contains(InInputMappingContextID))
			{
				// 记录已经添加了的IMC。
				TSet<FName>& IDs = this->AddedIMCIDs.FindOrAdd(InputMappingContextTableRow->Level);
				IDs.Add(InInputMappingContextID);

				// 判断添加IMC的Level是否超过当前最高等级。
				if (InputMappingContextTableRow->Level >= this->CurrentIMCLevel)
				{
					if (InputMappingContextTableRow->Level > this->CurrentIMCLevel)
					{
						// 如果超过的话需要把上一等级的IMC给暂时去除。
						if (TSet<FName>* Elements = this->AddedIMCIDs.Find(this->CurrentIMCLevel))
						{
							for (const FName& IMCID : *Elements)
							{
								FPlayerInputMappingContextTableRow* LastIMCTableRow = nullptr;
								Config->GetDataTableRowFromID<FPlayerInputMappingContextTableRow>(IMCID, LastIMCTableRow);
								System->RemoveMappingContext(LastIMCTableRow->InputMappingContext);
							}
						}
					}

					System->AddMappingContext(InputMappingContextTableRow->InputMappingContext, InputMappingContextTableRow->Priority);
					this->CurrentIMCLevel = InputMappingContextTableRow->Level;
				}
			}
			else
			{
				UE_LOG(LogPlayerInputSubsystem, Warning, TEXT("Input mapping context:\"%s\", ID:\"%s\", does exists, Adding Abort."), *InputMappingContextTableRow->InputMappingContext->GetName())
			}
		}
	}
}

void UPlayerInputSubsystem::RemovePlayerInputMappingContext(FName InInputMappingContextID)
{
	APlayerController* CurrentPlayerController = this->GetLocalPlayer()->GetPlayerController(this->GetWorld());

	// 合法性检查。
	if (InInputMappingContextID.IsNone() || CurrentPlayerController == nullptr)
	{
		return;
	}

	// 绑定Input Mapping Context。
	if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(CurrentPlayerController->GetLocalPlayer()))
	{
		// 通过InputMappingContextID获取对应的InputMappingContext。
		UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(CurrentPlayerController->GetGameInstance());
		FPlayerInputMappingContextTableRow* InputMappingContextTableRow = nullptr;
		if (Config->GetDataTableRowFromID<FPlayerInputMappingContextTableRow>(InInputMappingContextID, InputMappingContextTableRow))
		{
			// 移除MappingContext。
			if (TSet<FName>* AddedIDs = this->AddedIMCIDs.Find(InputMappingContextTableRow->Level))
			{
				// 只有IMC存在时才执行删除操作。
				int32 ElementNumber = AddedIDs->Remove(InInputMappingContextID);
				if (ElementNumber > 0)
				{
					// 如果删除的IMC等级和当前最高等级相等，则我们需要移除当前的IMC，并从当前最高等级-1开始查询，获取到最近等级的IMC并重新添加。
					if (InputMappingContextTableRow->Level == this->CurrentIMCLevel)
					{
						System->RemoveMappingContext(InputMappingContextTableRow->InputMappingContext);

						// 如果当前Level已经没有添加的IMC的ID，则把当前Level的键值对直接删除。
						if (AddedIDs->Num() == 0)
						{
							this->AddedIMCIDs.Remove(InputMappingContextTableRow->Level);
							int IMCLevel = this->CurrentIMCLevel - 1;
							for (; IMCLevel >= 0; --IMCLevel)
							{
								if (TSet<FName>* CurrentLevelIDs = this->AddedIMCIDs.Find(IMCLevel))
								{
									for (const FName& ID : *CurrentLevelIDs)
									{
										FPlayerInputMappingContextTableRow* LastIMCTableRow = nullptr;
										Config->GetDataTableRowFromID<FPlayerInputMappingContextTableRow>(ID, LastIMCTableRow);
										System->AddMappingContext(LastIMCTableRow->InputMappingContext, LastIMCTableRow->Priority);
									}
									this->CurrentIMCLevel = IMCLevel;
									break;
								}
							}

							// 如果IMCLevel小于0的话，说明后续不再有可用的IMC，需要重置一些属性。
							if (IMCLevel < 0)
							{
								this->CurrentIMCLevel = -1;
							}
						}
					}
				}
				else
				{
					UE_LOG(LogPlayerInputSubsystem, Warning, TEXT("Input mapping context:\"%s\", ID:\"%s\", does not exists, Removing Abort."), *InputMappingContextTableRow->InputMappingContext->GetName(), *InInputMappingContextID.ToString());
				}
			}
			else
			{
				UE_LOG(LogPlayerInputSubsystem, Warning, TEXT("Input mapping context:\"%s\", ID:\"%s\", does not have Level:\"%d\", Removing abort."), *InputMappingContextTableRow->InputMappingContext->GetName(), *InInputMappingContextID.ToString(), InputMappingContextTableRow->Level);
			}
		}
	}
}

bool UPlayerInputSubsystem::UnbindPlayerInputAction(FPlayerInputActionBindingHandle& Handle)
{
	bool bSuccess = false;

	APlayerController* CurrentPlayerController = this->GetLocalPlayer()->GetPlayerController(this->GetWorld());

	// 合法性检查。
	if (CurrentPlayerController == nullptr || Handle.InputActionBingdingHandle == 0)
	{
		return bSuccess;
	}

	// 解绑Input Action
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(CurrentPlayerController->InputComponent))
	{
		bSuccess = EIC->RemoveBindingByHandle(Handle.InputActionBingdingHandle);
	}

	// 如果解绑成功则重置句柄。
	if (bSuccess)
	{
		Handle.InputActionBingdingHandle = 0;
	}

	return bSuccess;
}

FPlayerInputActionBindingHandle UPlayerInputSubsystem::K2_BindPlayerInputAction(FName InInputActionID, FEnhancedInputActionHandlerDynamicSignature InOnInputActionExecute)
{
	FPlayerInputActionBindingHandle Handle;

	APlayerController* CurrentPlayerController = this->GetLocalPlayer()->GetPlayerController(this->GetWorld());

	// 合法性检查。
	if (InInputActionID.IsNone() || CurrentPlayerController == nullptr || InOnInputActionExecute.IsBound() == false)
	{
		return Handle;
	}

	// 通过InputActionID获取对应的InputAction。
	UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(CurrentPlayerController->GetGameInstance());
	FPlayerInputActionTableRow* InputActionTableRow = nullptr;
	if (Config->GetDataTableRowFromID<FPlayerInputActionTableRow>(InInputActionID, InputActionTableRow))
	{
		// 绑定InputAction。
		if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(CurrentPlayerController->InputComponent))
		{
			FEnhancedInputActionEventBinding& Binding = EIC->BindAction(InputActionTableRow->InputAction, InputActionTableRow->TriggerEvent, InOnInputActionExecute.GetUObject(), InOnInputActionExecute.GetFunctionName());
			Handle.InputActionBingdingHandle = Binding.GetHandle();
		}
	}

	return Handle;
}

bool UPlayerInputSubsystem::K2_UnbindPlayerInputAction(FPlayerInputActionBindingHandle& Handle)
{
	return this->UnbindPlayerInputAction(Handle);
}

bool UPlayerInputSubsystem::GetBoolTypeInstanceValue(const FInputActionInstance& InInstance, bool& OutValue)
{
	bool IsTheType = false;

	FInputActionValue InputActionValue = InInstance.GetValue();
	if (InputActionValue.GetValueType() == EInputActionValueType::Boolean)
	{
		IsTheType = true;

		OutValue = InputActionValue.Get<bool>();
	}

	return IsTheType;
}

bool UPlayerInputSubsystem::GetAxis1DTypeInstanceValue(const FInputActionInstance& InInstance, float& OutValue)
{
	bool IsTheType = false;

	FInputActionValue InputActionValue = InInstance.GetValue();
	if (InputActionValue.GetValueType() == EInputActionValueType::Axis1D)
	{
		IsTheType = true;

		OutValue = InputActionValue.Get<FInputActionValue::Axis1D>();
	}

	return IsTheType;
}

bool UPlayerInputSubsystem::GetAxis2DTypeInstanceValue(const FInputActionInstance& InInstance, FVector2D& OutValue)
{
	bool IsTheType = false;

	FInputActionValue InputActionValue = InInstance.GetValue();
	if (InputActionValue.GetValueType() == EInputActionValueType::Axis2D)
	{
		IsTheType = true;

		OutValue = InputActionValue.Get<FInputActionValue::Axis2D>();
	}

	return IsTheType;
}

bool UPlayerInputSubsystem::GetAxis3DTypeInstanceValue(const FInputActionInstance& InInstance, FVector& OutValue)
{
	bool IsTheType = false;

	FInputActionValue InputActionValue = InInstance.GetValue();
	if (InputActionValue.GetValueType() == EInputActionValueType::Axis3D)
	{
		IsTheType = true;

		OutValue = InputActionValue.Get<FInputActionValue::Axis3D>();
	}

	return IsTheType;
}
