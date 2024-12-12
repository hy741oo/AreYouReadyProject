// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu/MainMenuPlayerController.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/InputSettings.h"
#include "ConfigSubsystem.h"

AMainMenuPlayerController::AMainMenuPlayerController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 绑定游戏内输入。
	if (UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(this->InputComponent))
	{

	}

	// 绑定UI输入。
	UInputSettings* IS = UInputSettings::GetInputSettings();
	UConfigSubsystem* ConfigSubsystem = this->GetGameInstance()->GetSubsystem<UConfigSubsystem>();
	if (ensure(IS && ConfigSubsystem))
	{
		FPlayerUIInputMappingTableRow* UIInputMappingTableRow = nullptr;
		if (ensureAlways(ConfigSubsystem->GetDataTableRowFromID<FPlayerUIInputMappingTableRow>(this->GetPlayerControllerInfo().PlayerUIInputMappingID, UIInputMappingTableRow)))
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

