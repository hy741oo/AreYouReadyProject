// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableObjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAMEPLAYFRAMEWORK_API IInteractableObjectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
protected:
	// 是否可交互。可由子类进行更改，用来动态开关一个物体是否可交互。
	bool bIsInteractable = true;

	// 需要在PlayerHUD的交互UI上显示的输入图标Name。
	FName InputIconName = "Interaction";

public:
	// 检测是否可交互。
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable")
	bool IsInteractable();
	virtual bool IsInteractable_Implementation();

	// 交互实际执行的逻辑。
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interactable")
	void Interact();

	// 进入可交互状态。例如玩家进入一个按钮的可交互范围时用于显示交互按键。
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interactable")
	void EnterInteractableState();

	// 离开可交互状态。例如玩家离开一个按钮的可交互范围时用于隐藏交互按键。
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interactable")
	void LeaveInteractableState();

	// 设置是否可交互。
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable")
	void SetIsInteractable(bool bInIsInteractable);
	virtual void SetIsInteractable_Implementation(bool bInIsInteractable);

	// 设置要在PlayerHUD上显示的交互UI输入图标Name。
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable")
	void SetInputIconName(const FName InInputIconName);
	virtual void SetInputIconName_Implementation(const FName InInputIconName);

	// 获取交互UI需要显示的输入图标Name。
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable")
	FName GetInputIconName() const;
	virtual FName GetInputIconName_Implementation() const;

};
