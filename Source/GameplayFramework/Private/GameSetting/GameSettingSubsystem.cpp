// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSetting/GameSettingSubsystem.h"

#include "Audio/AudioManagerSubsystem.h"

void UGameSettingSubsystem::Initialize(FSubsystemCollectionBase& InCollection)
{

}

void UGameSettingSubsystem::SaveSetting()
{
	this->SaveConfig();
}

void UGameSettingSubsystem::LoadSetting()
{
	this->LoadConfig();
}

void UGameSettingSubsystem::ApplySetting()
{
	UAudioManagerSubsystem* AudioManager = UGameInstance::GetSubsystem<UAudioManagerSubsystem>(this->GetGameInstance());

	// 初始化全局音量。
	AudioManager->SetSoundMixClassOverride("GlobalSound", this->GlobalVolume);
	// 初始化背景音乐音量。
	AudioManager->SetSoundMixClassOverride("BGM", this->BGMVolume);
	// 初始化音效音量。
	AudioManager->SetSoundMixClassOverride("SFX", this->SFXVolume);

}
