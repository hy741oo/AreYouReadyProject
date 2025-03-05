// Fill out your copyright notice in the Description page of Project Settings.


#include "AYRFunctionLibrary.h"

#include "GameConfigSubsystem.h"

bool UAYRFunctionLibrary::GetInputIconBrush(const UObject* InWorldContextObject, const FName InInputName, FSlateBrush& OutInputIconBrush, const EInputDeviceType::Type InInputDeviceType)
{
	bool bIsFind = false;
	if (UWorld* World = GEngine->GetWorldFromContextObject(InWorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(World->GetGameInstance()))
		{
			FInputIconDataTableRow TempTableRow;
			bIsFind = Config->GetInputIconData(InInputName, InInputDeviceType, TempTableRow, OutInputIconBrush);
		}
	}
	
	return bIsFind;
}

bool UAYRFunctionLibrary::GetCurrentInputIconBrush(const UObject* InWorldContextObject, const FName InInputName, FSlateBrush& OutInputIconBrush)
{
	bool bIsFind = false;
	if (UWorld* World = GEngine->GetWorldFromContextObject(InWorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UAYRGameInstance* GI = World->GetGameInstance<UAYRGameInstance>())
		{
			const EInputDeviceType::Type InputDeviceType = GI->GetCurrentInputDeviceType();
			bIsFind = UAYRFunctionLibrary::GetInputIconBrush(InWorldContextObject, InInputName, OutInputIconBrush, InputDeviceType);
		}
	}

	return bIsFind;
}

