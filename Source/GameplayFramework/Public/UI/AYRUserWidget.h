// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UISubsystem.h"

#include "AYRUserWidget.generated.h"

// UI��ѹջʱ��ԭ��
UENUM(BlueprintType)
namespace ELeaveReason
{
	enum Type
	{
		LR_NewWidgetEntered UMETA(DisplayName = NewWidgetEntered),
		LR_BePopped UMETA(DisplayName = BePoped)
	};
}


/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UAYRUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// UI��ϵͳ��
	UUISubsystem* UISubsystem = nullptr;

protected:
	virtual void NativeConstruct() override;

public:
	// ��ǰUI��UIջ�е��±ꡣ
	int32 StackIndex = -1;
	
public:
	// ����Widget��ΪWidgetջջ��Ԫ�أ�����ӻ��Widget���ϵ�Widget���Ƴ���ʱ���á�
	virtual void OnEnterThisWidget(APlayerController* PlayerController, const FUIStackInfo* UIStackInfo);
	// ��ͼ�汾��
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnEnterThisWidget(APlayerController* PlayerController, const FUIStackInfo UIStackInfo);

	// ����Widget���ٳ�ΪWidgetջջ��Ԫ��ʱ���á�
	virtual void OnLeaveThisWidget(ELeaveReason::Type LeaveReason);
	// ��ͼ�汾��
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnLeaveThisWidget(ELeaveReason::Type LeaveReason);

	virtual void SetInputPriority(int32 Priority = 0);

	virtual void SetIsStopAction(bool bShouldBlock = false);
};
