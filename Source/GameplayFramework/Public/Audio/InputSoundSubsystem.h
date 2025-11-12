// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameConfigSubsystem.h"

#include "InputSoundSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInputSoundSubsystem, Log, All);

class UAudioManagerSubsystem;

USTRUCT(BlueprintType)
struct FInputSoundTableRow : public FAYRTableRowBase
{
	GENERATED_BODY()

	// 按键和音效的映射。每个按键映射一个音效。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FKey, FName> KeySoundMap;

	// 是否阻挡按键往下传递。在按键音效栈顶中如果没有查找到当前按键对应的音效时，是否需要往栈的下一层寻找，true为不继续寻找，false则继续寻找。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bHandled = true;

	// 优先级。值越大则越在按键音效栈顶层。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 Priority = 0;
};

USTRUCT(BlueprintType)
struct FInputSoundDataHandle
{
	GENERATED_BODY()

	int32 CurrentID = -1;
};

struct FInputSoundStackData
{
	FInputSoundTableRow* DataTableRow = nullptr;

	int32 UniqueID = -1;

	bool bMute = false;
};

/**
 * 输入音效子系统。实现当用户按下对应按键时直接播放音效的功能，从而取代需要在UI上播放输入按键音效。
 * 注：并不保证比在UI上播放音效的方案效率高，该类仅作一次尝试，需要实践才直到是否合适。
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UInputSoundSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	// 按键音效栈。
	TArray<FInputSoundStackData> InputSoundStack;

	// 音频子系统。用于播放音效。
	UAudioManagerSubsystem* AudioManager = nullptr;

	// GMS回调事件的句柄。
	FGMSListenerHandle GMSHandle;

	// 唯一标识符。用于在句柄上识别Input Sound数据。
	int32 UniqueID = -1;
private:
	// 接受到GMS发送过来的处理输入按键的回调。
	virtual void OnHandledInputKey(UGMSMessageBase* InMessage);
	
public:
	// 初始化，注册回调函数。
	virtual void Initialize(FSubsystemCollectionBase& InCollection) override;

	virtual void Deinitialize() override;

	// 入栈输入按键音效数据。
	UFUNCTION(BlueprintCallable, Category = "Input Sound Subsystem")
	bool PushInputSoundData(FName InInputSdoundID, FInputSoundDataHandle& InHandle, bool bInMute = false);

	UFUNCTION(BlueprintCallable, Category = "Input Sound Subsystem")
	void PopInputSoundData(UPARAM(Ref)FInputSoundDataHandle& InHandle);

	// 静音InHandle指定的输入音效。
	UFUNCTION(BlueprintCallable, Category = "Input SoundSubsystem")
	void MuteInputSound(const FInputSoundDataHandle& InHandle);

	// 取消静音InHandle指定的输入音效。
	UFUNCTION(BlueprintCallable, Category = "Input SoundSubsystem")
	void UnMuteInputSound(const FInputSoundDataHandle& InHandle);
};
