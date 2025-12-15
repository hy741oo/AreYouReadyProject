// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameSettingSubsystem.generated.h"

/**
 * 游戏设置相关子系统。音量、分辨率等相关设置。
 */
UCLASS(Config = GameSetting)
class GAMEPLAYFRAMEWORK_API UGameSettingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// 全局音量。
	UPROPERTY(BlueprintReadWrite, Category = "Audio", Config)
	float GlobalVolume = 1.f;

	// 背景音乐音量。
	UPROPERTY(BlueprintReadWrite, Category = "Audio", Config)
	float BGMVolume = 1.f;

	// 音效音量。
	UPROPERTY(BlueprintReadWrite, Category = "Audio", Config)
	float SFXVolume = 1.f;

	// 语言。
	UPROPERTY(BlueprintReadWrite, Category = "CurrentGameCulture", Config)
	FString CurrentGameCulture;

public:
	// 初始化。
	void Initialize(FSubsystemCollectionBase& InCollection);

	// 保存设置到/Game/Saved/Config/GameSetting.ini。
	UFUNCTION(BlueprintCallable, Category = "Game Setting")
	void SaveSetting();

	// 从本地的/Game/Saved/Config/GameSetting.ini读取设置。
	UFUNCTION(BlueprintCallable, Category = "Game Setting")
	void LoadSetting();

	// 应用设置。
	UFUNCTION(BlueprintCallable, Category = "Game Setting")
	void ApplySetting();
};

