// Fill out your copyright notice in the Description page of Project Settings.


#include "AYRPlayerController.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/InputSettings.h"
#include "ConfigSubsystem.h"

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
	if (!ConfigSubsystem->GetDataTableRowFromID<FPlayerControllerInfoTableRow>(this->ID, PlayerControllerInfo))
	{
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
		if (ConfigSubsystem->GetDataTableRowFromID<FPlayerUIInputMappingTableRow>(this->PlayerControllerInfoTableRow->PlayerUIInputMappingID, UIInputMappingTableRow))
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
	}
}

