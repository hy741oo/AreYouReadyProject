// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Camera/OSCameraComponent.h"
#include "GameConfigSubsystem.h"

DEFINE_LOG_CATEGORY(LogOSCameraComponent);

// Sets default values for this component's properties
UOSCameraComponent::UOSCameraComponent(const FObjectInitializer& InObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	this->bIsBlending = false;
}


// Called when the game starts
void UOSCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (this->InitCameraInfoName != NAME_None)
	{
		this->InitCameraInfo(this->InitCameraInfoName);
		this->InitCameraInfoName = NAME_None;
	}
	
}


// Called every frame
void UOSCameraComponent::TickComponent(float InDeltaTime, ELevelTick InTickType, FActorComponentTickFunction* InThisTickFunction)
{
	Super::TickComponent(InDeltaTime, InTickType, InThisTickFunction);

	if (this->bIsBlending)
	{
		if (this->TargetCameraInfo)
		{
			this->ElapsedSeconds += InDeltaTime;
			if (this->ElapsedSeconds >= this->TargetBlendTime)
			{
				this->SetCameraByTargetCameraInfo();
				this->ElapsedSeconds = .0f;
				this->bIsBlending = false;
				this->OriginalCameraInfo = this->TargetCameraInfo;
				this->TargetCameraInfo = nullptr;
			}
			else
			{
				FCameraInfoTableRow CameraInfo = this->GetLerpedCameraInfo(*this->OriginalCameraInfo, *this->TargetCameraInfo, this->ElapsedSeconds / this->TargetBlendTime);
				this->SetCameraByCameraInfo(CameraInfo);
			}
		}
		else
		{
			this->bIsBlending = false;
			UE_LOG(LogOSCameraComponent, Warning, TEXT("Camera is blending, but the TargetCameraInfo is null!"));
		}
	}
}

void UOSCameraComponent::InitCameraInfo(const FName& InCameraInfoID)
{
	UWorld* World = this->GetWorld();
	// 如果当前World为空，则无法通过World获取GameInstance类型的子系统，我们需要把初始化的ID记录下来，然后在BeginPlay里执行真正的初始化。
	if (!World)
	{
		this->InitCameraInfoName = InCameraInfoID;
		return;
	}

	if (FCameraInfoTableRow* CameraInfo = this->GetCameraInfoByID(InCameraInfoID))
	{
		this->OriginalCameraInfo = CameraInfo;

		this->SetCameraByOriginalCameraInfo();
	}
}

FCameraInfoTableRow* UOSCameraComponent::GetCameraInfoByID(const FName& InCameraInfoID)
{
	UGameConfigSubsystem* GameConfig = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetWorld()->GetGameInstance());
	FCameraInfoTableRow* CameraInfo = nullptr;
	GameConfig->GetDataTableRowFromID<FCameraInfoTableRow>(InCameraInfoID, CameraInfo);
	return CameraInfo;
}

void UOSCameraComponent::SetCameraByCameraInfoID(const FName& InCameraInfoID)
{
	if (FCameraInfoTableRow* CameraInfo = this->GetCameraInfoByID(InCameraInfoID))
	{
		this->SetCameraByCameraInfo(*CameraInfo);
	}
}

void UOSCameraComponent::SetCameraByCameraInfo(const FCameraInfoTableRow& InCameraInfo)
{
	this->ProjectionMode = InCameraInfo.ProjectionMode;
	this->FieldOfView = InCameraInfo.FieldOfView;
	this->OrthoWidth = InCameraInfo.OrthoWidth;
	this->OrthoNearClipPlane = InCameraInfo.OrthoNearClipPlane;
	this->OrthoFarClipPlane = InCameraInfo.OrthoFarClipPlane;
	this->AspectRatio = InCameraInfo.AspectRatio;
	this->bConstrainAspectRatio = InCameraInfo.bConstrainAspectRatio;
	this->bUsePawnControlRotation = InCameraInfo.bUsePawnControlRotation;
	this->PostProcessBlendWeight = InCameraInfo.PostProcessBlendWeight;
	this->bLockToHmd = InCameraInfo.bLockToHMD;
	this->bUseFieldOfViewForLOD = InCameraInfo.bUseFieldOfViewForLOD;
	this->PostProcessSettings = InCameraInfo.PostProcessSettings;
}

void UOSCameraComponent::SetCameraByOriginalCameraInfo()
{
	if (this->OriginalCameraInfo)
	{
		this->SetCameraByCameraInfo(*this->OriginalCameraInfo);
	}
}

void UOSCameraComponent::SetCameraByTargetCameraInfo()
{
	if (this->TargetCameraInfo)
	{
		this->SetCameraByCameraInfo(*this->TargetCameraInfo);
	}
	else
	{
		UE_LOG(LogOSCameraComponent, Warning, TEXT("You want set camera by target camera info, but there's no any TargetCameraInfo."));
	}
}

void UOSCameraComponent::ChangeCameraTo(const FName& InCameraInfoID, float InTargetBlendTime)
{
	if (FCameraInfoTableRow* CameraInfo = this->GetCameraInfoByID(InCameraInfoID))
	{
		// 相机过渡时如果触发了同一个相机过度，则不执行任何操作。
		if (CameraInfo == this->TargetCameraInfo)
		{
			return;
		}

		if (InTargetBlendTime > .0f)
		{
			// 如果相机过渡的新目标信息是相机原本的信息，则说明用户在相机过渡过程中需要执行“反过渡”操作，即回退相机的信息，反之则更新部分数据。
			if (CameraInfo != this->OriginalCameraInfo)
			{
				this->TargetBlendTime = InTargetBlendTime;
				this->ElapsedSeconds = .0f;
			}
			else
			{
				this->ElapsedSeconds = this->TargetBlendTime - this->ElapsedSeconds;
			}

			if (this->bIsBlending)
			{
				this->OriginalCameraInfo = this->TargetCameraInfo;
			}
			this->TargetCameraInfo = CameraInfo;
			this->bIsBlending = true;
		}
		else
		{
			if (this->bIsBlending)
			{
				this->bIsBlending = false;
				this->TargetBlendTime = false;
				this->TargetCameraInfo = nullptr;
				this->ElapsedSeconds = .0f;
			}
			this->SetCameraByCameraInfo(*CameraInfo);
			this->OriginalCameraInfo = CameraInfo;
		}
	}
}

FCameraInfoTableRow UOSCameraComponent::GetLerpedCameraInfo(const FCameraInfoTableRow& InStart, const FCameraInfoTableRow& InEnd, float InAlpha)
{
	FCameraInfoTableRow CameraInfo = InStart;

	CameraInfo.FieldOfView = FMath::Lerp(InStart.FieldOfView, InEnd.FieldOfView, InAlpha);

	return CameraInfo;
}

FCameraInfoTableRow UOSCameraComponent::GetCurrentCameraParams() const
{
	FCameraInfoTableRow CameraInfo;
	CameraInfo.ProjectionMode = this->ProjectionMode;
	CameraInfo.FieldOfView = this->FieldOfView;
	CameraInfo.OrthoWidth = this->OrthoWidth;
	CameraInfo.OrthoNearClipPlane = this->OrthoNearClipPlane;
	CameraInfo.OrthoFarClipPlane = this->OrthoFarClipPlane;
	CameraInfo.AspectRatio = this->AspectRatio;
	CameraInfo.bConstrainAspectRatio = this->bConstrainAspectRatio;
	CameraInfo.bUsePawnControlRotation = this->bUsePawnControlRotation;
	CameraInfo.PostProcessBlendWeight = this->PostProcessBlendWeight;
	CameraInfo.bLockToHMD = this->bLockToHmd;
	CameraInfo.bUseFieldOfViewForLOD = this->bUseFieldOfViewForLOD;
	CameraInfo.PostProcessSettings = this->PostProcessSettings;
	return CameraInfo;
}

