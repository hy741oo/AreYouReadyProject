// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameConfigSubsystem.h"
#include "Camera/CameraComponent.h"

#include "AYRCameraComponent.generated.h"

class UCameraComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogAYRCameraComponent, Log, All);

USTRUCT(BlueprintType)
struct FCameraInfoTableRow : public FAYRTableRowBase
{
	GENERATED_BODY()

	FCameraInfoTableRow()
	{
		this->bConstrainAspectRatio = false;
		this->bUsePawnControlRotation = false;
		this->bLockToHMD = true;
		this->bUseFieldOfViewForLOD = true;
	}

	// 投射类型：透视和正交。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TEnumAsByte<ECameraProjectionMode::Type> ProjectionMode = ECameraProjectionMode::Perspective;

	// 视野。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (ClampMin = "0.001", ClampMax = "360.0", Units = deg, EditCondition = "ProjectionMode == ECameraProjectionMode::Perspective"))
	float FieldOfView = 90.f;

	// 正交宽度。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (EditCondition = "ProjectionMode == ECameraProjectionMode::OrthoGraphic"))
	float OrthoWidth = 512.f;

	// 正交近剪裁平面距离。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (EditCondition = "ProjectionMode == ECameraProjectionMode::OrthoGraphic"))
	float OrthoNearClipPlane = .0f;

	// 正交远剪裁平面距离。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (EditCondition = "ProjectionMode == ECameraProjectionMode::OrthoGraphic"))
	float OrthoFarClipPlane = WORLD_MAX;

	// 长宽比。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (ClampMin = "0.1", ClampMax = "100.0", EditCondition = "bConstrainAspectRatio"))
	float AspectRatio = 1.777778f;

	// 是否固定长宽比。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	uint8 bConstrainAspectRatio : 1;

	// 如果该Camera被放置到一个Pawn上，是否使用Pawn的Controller的Rotation。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	uint8 bUsePawnControlRotation : 1;

	// 后处理混合权重。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (UIMin = "0.0", UIMax = "1.0"))
	float PostProcessBlendWeight = 1.f;

	// 是否锁定到HMD上。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	uint8 bLockToHMD : 1;

	// 是否使用FOV来计算LOD。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	uint8 bUseFieldOfViewForLOD : 1;

	// 后期处理设置。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPostProcessSettings PostProcessSettings;
};

/**
 * 新摄像机组件。
 * 
 * 该组件通过配表的方式存储摄像机信息，并添加在摄像机信息之间插值实现诸如“FOV渐变”、“视角切换”等功能。
 */
UCLASS( ClassGroup=(Custom), Meta=(BlueprintSpawnableComponent) )
class GAMEPLAYFRAMEWORK_API UAYRCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAYRCameraComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float InDeltaTime, ELevelTick InTickType, FActorComponentTickFunction* InThisTickFunction) override;

private:
	// 当玩家在Actor的构造函数里调用InitCameraInfo时，由于GameConfigSubsystem可能没有初始化从而造成Init失败，这里做一个初始化Name的记录，然后在BeginPlay里检测该名字是否等于NAME_NONE，不等于的话说明当前摄像机是在构造函数里进行初始化，所以BeginPlay里需要“实际”执行一次初始化。
	FName InitCameraInfoName = NAME_None;

protected:
	// 初始相机信息。用于记录相机混合前的信息。
	FCameraInfoTableRow* OriginalCameraInfo = nullptr;

	// 目标相机信息，用于和当前相机信息混合执行渐变类型的逻辑。
	FCameraInfoTableRow* TargetCameraInfo = nullptr;

	// 正在进行混合。
	uint8 bIsBlending : 1;

	// 混合已经执行的秒数。
	float ElapsedSeconds = .0f;

	// 混合持续时间。
	float TargetBlendTime = -1.f;
protected:
	// 获取插值后的摄像机信息。
	FCameraInfoTableRow GetLerpedCameraInfo(const FCameraInfoTableRow& InStart, const FCameraInfoTableRow& InEnd, float InAlpha);

	// 获取当前相机的信息。
	FCameraInfoTableRow GetCurrentCameraParams() const;

public:
	// 获取CameraInfoID指定的摄像机信息。
	FCameraInfoTableRow* GetCameraInfoByID(const FName& InCameraInfoID);

	// 使用CameraInfoID来设置相机参数。
	UFUNCTION(BlueprintCallable, Category = "AYRCamera")
	void SetCameraByCameraInfoID(const FName& InCameraInfoID);

	// 使用CameraInfo来设置相机参数。
	UFUNCTION(BlueprintCallable, Category = "AYRCamera")
	void SetCameraByCameraInfo(const FCameraInfoTableRow& InCameraInfo);

	// 设置相机参数为当前相机信息。
	UFUNCTION(BlueprintCallable, Category = "AYRCamera")
	void SetCameraByOriginalCameraInfo();

	// 设置相机参数为目标相机信息。
	UFUNCTION(BlueprintCallable, Category = "AYRCamera")
	void SetCameraByTargetCameraInfo();

	// 初始化摄像机信息。
	UFUNCTION(BlueprintCallable, Category = "AYRCamera")
	void InitCameraInfo(const FName& InCameraInfoID);

	// 切换摄像机信息到指定的CameraInfoID。当InTargetBlendTime <= 0时则不启用摄像机过渡，而是直接切换过去。
	UFUNCTION(BlueprintCallable, Category = "AYRCamera")
	void ChangeCameraTo(const FName& InCameraInfoID, float InTargetBlendTime = .2f);
};
