// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/AYRPlayerCameraManager.h"
#include "GameConfigSubsystem.h"

void AAYRPlayerCameraManager::StartCameraShakeByID(const FName& InCameraShakeID, ECameraShakePlaySpace InCameraShakePlaySpace, FRotator InUserPlaySpaceRot)
{
	FCameraShakeInfoTableRow* Info = nullptr;
	if (this->GetCameraShakeInfo(InCameraShakeID, Info))
	{
		this->StartCameraShake(Info->CameraShakeClass, Info->Scale, InCameraShakePlaySpace, InUserPlaySpaceRot);
	}
}

void AAYRPlayerCameraManager::StopCameraShakeByID(const FName& InCameraShakeID)
{
	FCameraShakeInfoTableRow* Info = nullptr;
	if (this->GetCameraShakeInfo(InCameraShakeID, Info))
	{
		this->StopAllInstancesOfCameraShake(Info->CameraShakeClass, Info->bStopImmediately);
	}
}

bool AAYRPlayerCameraManager::GetCameraShakeInfo(const FName& InCameraShakeID, FCameraShakeInfoTableRow*& OutCameraShakeInfo)
{
	UGameConfigSubsystem* GameConfig = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetGameInstance());

	bool Ret = false;
	FCameraShakeInfoTableRow* Info = nullptr;
	if (GameConfig->GetDataTableRowFromID<FCameraShakeInfoTableRow>(InCameraShakeID, Info))
	{
		OutCameraShakeInfo = Info;
		Ret = true;
	}

	return Ret;
}

bool AAYRPlayerCameraManager::GetCameraShakeInfo(const FName& InCameraShakeID, FCameraShakeInfoTableRow& OutCameraShakeInfo)
{
	bool Ret = false;
	FCameraShakeInfoTableRow* Info = nullptr;
	if (this->GetCameraShakeInfo(InCameraShakeID, Info))
	{
		OutCameraShakeInfo = *Info;
		Ret = true;
	}

	return Ret;
}
