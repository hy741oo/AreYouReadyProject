// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSetting/GameSettingSubsystem.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Culture.h"

#include "Audio/AudioSubsystem.h"

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
	UAudioSubsystem* AudioManager = UGameInstance::GetSubsystem<UAudioSubsystem>(this->GetGameInstance());

	// 初始化全局音量。
	AudioManager->SetSoundMixClassOverride("GlobalSound", this->GlobalVolume);
	// 初始化背景音乐音量。
	AudioManager->SetSoundMixClassOverride("BGM", this->BGMVolume);
	// 初始化音效音量。
	AudioManager->SetSoundMixClassOverride("SFX", this->SFXVolume);

	// 设置语言。
	if (this->CurrentGameCulture.IsEmpty())
	{
		this->CurrentGameCulture = FInternationalization::Get().GetCurrentCulture()->GetName();
	}
	FInternationalization::Get().SetCurrentCulture(this->CurrentGameCulture);
}
