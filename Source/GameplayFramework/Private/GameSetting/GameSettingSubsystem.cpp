// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSetting/GameSettingSubsystem.h"

void UGameSettingSubsystem::SaveSetting()
{
	this->SaveConfig();
}

void UGameSettingSubsystem::LoadSetting()
{
	this->LoadConfig();
}
