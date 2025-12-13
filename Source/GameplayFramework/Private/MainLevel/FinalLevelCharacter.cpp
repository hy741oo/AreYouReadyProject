// Fill out your copyright notice in the Description page of Project Settings.


#include "MainLevel/FinalLevelCharacter.h"

void AFinalLevelCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 隐藏ID卡提示图标。
	this->HideIDCardIcons();
}

