// Fill out your copyright notice in the Description page of Project Settings.


#include "MainLevel/MainLevelPlayerController.h"

AMainLevelPlayerController::AMainLevelPlayerController(const FObjectInitializer& InObjectInitializer)
	:Super(InObjectInitializer)
{

}

void AMainLevelPlayerController::BeginPlay()
{
	// 初始化输入系数。
	this->InputPitchScale = 1.f;
	this->InputYawScale = 1.f;
	this->InputRollScale = 1.f;
}

