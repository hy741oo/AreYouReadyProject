// Fill out your copyright notice in the Description page of Project Settings.


#include "AYRPlayerController.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/InputSettings.h"
#include "ConfigSubsystem.h"

DEFINE_LOG_CATEGORY(LogAYRPlayerController);

AAYRPlayerController::AAYRPlayerController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void AAYRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 从数据表里获取控制器信息。
	UConfigSubsystem * ConfigSubsystem = this->GetGameInstance()->GetSubsystem<UConfigSubsystem>();
	check(ConfigSubsystem);
	FPlayerControllerInfoTableRow* PlayerControllerInfo = nullptr;
	if (!ConfigSubsystem->GetDataTableRowFromID(this->ID, PlayerControllerInfo))
	{
		UE_LOG(LogAYRPlayerController, Warning, TEXT("Can't find ID: \"%s\""), *this->ID.ToString());
		return;
	}

	this->PlayerControllerInfoTableRow = PlayerControllerInfo;

	// 绑定游戏内输入。
	if (UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(this->InputComponent))
	{

	}

	// 绑定UI输入。
	UInputSettings* IS = UInputSettings::GetInputSettings();
	if (ensure(IS))
	{
		FPlayerUIInputMappingTableRow* UIInputMappingTableRow = nullptr;
		if (ConfigSubsystem->GetDataTableRowFromID(this->PlayerControllerInfoTableRow->PlayerUIInputMappingID, UIInputMappingTableRow))
		{
			UInputSettings* InputSettings = UInputSettings::GetInputSettings();
			checkf(InputSettings, TEXT("Can't get InputSettings!"));

			for (const FUIInputMapping& UIInput : UIInputMappingTableRow->UIInputActions)
			{
				const FName ActionName = UIInput.ActionName;
				for (const FInputActionKeyMapping& InputActionKeyMapping : UIInput.InputActions)
				{
					FInputActionKeyMapping RealInputActionKeyMapping;
					RealInputActionKeyMapping = InputActionKeyMapping;
					RealInputActionKeyMapping.ActionName = ActionName;
					InputSettings->AddActionMapping(RealInputActionKeyMapping);
				}
			}
		}
		else
		{
			UE_LOG(LogAYRPlayerController, Warning, TEXT("Can't find PlayerUIInputMappingID: \"%s\""), *this->PlayerControllerInfoTableRow->PlayerUIInputMappingID.ToString());
		}
	}
}

