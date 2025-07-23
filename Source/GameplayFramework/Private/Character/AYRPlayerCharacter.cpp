// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AYRPlayerCharacter.h"

#include "AYRPlayerController.h"
#include "Camera/AYRPlayerCameraManager.h"

void AAYRPlayerCharacter::StartCameraShake(const FName& InCameraShakeID)
{
	this->GetController<AAYRPlayerController>()->GetPlayerCameraManager()->StartCameraShakeByID(InCameraShakeID);
}

void AAYRPlayerCharacter::StopCameraShake(const FName& InCameraShakeID)
{
	this->GetController<AAYRPlayerController>()->GetPlayerCameraManager()->StopCameraShakeByID(InCameraShakeID);
}

