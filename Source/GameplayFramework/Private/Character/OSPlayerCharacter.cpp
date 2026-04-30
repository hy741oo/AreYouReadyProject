// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/OSPlayerCharacter.h"

#include "AYRPlayerController.h"
#include "Camera/OSPlayerCameraManager.h"

void AOSPlayerCharacter::StartCameraShake(const FName& InCameraShakeID)
{
	this->GetController<AAYRPlayerController>()->GetPlayerCameraManager()->StartCameraShakeByID(InCameraShakeID);
}

void AOSPlayerCharacter::StopCameraShake(const FName& InCameraShakeID)
{
	this->GetController<AAYRPlayerController>()->GetPlayerCameraManager()->StopCameraShakeByID(InCameraShakeID);
}

