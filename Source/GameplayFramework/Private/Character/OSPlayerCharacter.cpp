// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/OSPlayerCharacter.h"

#include "Gameplay/OSPlayerController.h"
#include "Camera/OSPlayerCameraManager.h"

void AOSPlayerCharacter::StartCameraShake(const FName& InCameraShakeID)
{
	this->GetController<AOSPlayerController>()->GetPlayerCameraManager()->StartCameraShakeByID(InCameraShakeID);
}

void AOSPlayerCharacter::StopCameraShake(const FName& InCameraShakeID)
{
	this->GetController<AOSPlayerController>()->GetPlayerCameraManager()->StopCameraShakeByID(InCameraShakeID);
}

