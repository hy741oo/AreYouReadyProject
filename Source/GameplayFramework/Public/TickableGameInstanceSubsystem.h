// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TickableGameInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UTickableGameInstanceSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override {}

	// 判断是否Tick。防止CDO也执行Tick，从而一个Tick执行两次。
	virtual bool IsTickable() const override
	{
		return !this->IsTemplate();
	}

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UTickableGameInstanceSubsystem, STATGROUP_Tickables);
	}
};
