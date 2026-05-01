// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "Subsystem/GameConfigSubsystem.h"
#include "Kismet/GameplayStatics.h"

#include "PlayerInputSubsystem.generated.h"

class UInputMappingContext;

DECLARE_LOG_CATEGORY_EXTERN(LogPlayerInputSubsystem, Log, All);

// Input Action数据表。
USTRUCT(BlueprintType)
struct FPlayerInputActionTableRow : public FAYRTableRowBase
{
	GENERATED_BODY()

	// Input Action资产。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UInputAction* InputAction = nullptr;

	// Input Action的触发方式。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	ETriggerEvent TriggerEvent = ETriggerEvent::Triggered;
};

// Input Mapping Context数据表。
USTRUCT(BlueprintType)
struct FPlayerInputMappingContextTableRow : public FAYRTableRowBase
{
	GENERATED_BODY()

	// Input Mapping Context资产。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UInputMappingContext* InputMappingContext = nullptr;

	// 优先级。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 Priority = 0;

	// 等级。不同等级的Input Mapping Context被添加时，小于该等级值的所有Input Mapping Context都会被移除，只有当该等级的IMC全部被移除后，才会重新启用被移除的IMC。
	// 例如项目已经添加了两个等级2的IMC，然后又添加了一个等级3的IMC，这个时候等级2的两个IMC都会被移除掉，等后续等级3的IMC被移除后，两个等级2的IMC又会被重新添加进来。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (ClampMin = "0"))
	int32 Level = 0;
};

// 绑定Input Action后的句柄。
USTRUCT(BlueprintType)
struct FPlayerInputActionBindingHandle
{
	GENERATED_BODY()

	uint32 InputActionBingdingHandle = 0;
};

/**
 * 结合增强输入（Enhanced Input）系统实现Game和UI层级的输入管理。
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UPlayerInputSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
private:
	// 当前已经添加的IMC，根据Level进行分组。
	TMap<int32, TSet<FName>> AddedIMCIDs;

	// 当前系统中IMC的最高等级。
	int32 CurrentIMCLevel = -1;

public:
	// 绑定Enhanced Input Action。
	template<typename UObjectType>
	FPlayerInputActionBindingHandle BindPlayerInputAction(FName InInputActionID, UObjectType* InBindingObject, void(UObjectType::*InOnInputActionExecute)(const FInputActionInstance&))
	{
		FPlayerInputActionBindingHandle Handle;

		// 合法性检查。
		if (InInputActionID.IsNone() || InBindingObject == nullptr || InOnInputActionExecute == nullptr)
		{
			return Handle;
		}

		// 通过InputActionID获取对应的InputAction。
		UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetLocalPlayer()->GetGameInstance());
		FPlayerInputActionTableRow* InputActionTableRow = nullptr;
		if (Config->GetDataTableRowFromID<FPlayerInputActionTableRow>(InInputActionID, InputActionTableRow))
		{
			// 绑定InputAction。
			if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(this->GetLocalPlayer()->GetPlayerController(this->GetWorld())->InputComponent))
			{
				FEnhancedInputActionEventBinding& Binding = EIC->BindAction(InputActionTableRow->InputAction, InputActionTableRow->TriggerEvent, InBindingObject, InOnInputActionExecute);
				Handle.InputActionBingdingHandle = Binding.GetHandle();
			}
		}

		return Handle;
	}

	// 解绑Enhanced Input Action。
	bool UnbindPlayerInputAction(FPlayerInputActionBindingHandle& Handle);

	// 添加Enhanced Input Mapping Context。
	UFUNCTION(BlueprintCallable)
	void AddPlayerInputMappingContext(FName InInputMappingContextID);

	// 移除Enhanced Input Mapping Context。
	UFUNCTION(BlueprintCallable)
	void RemovePlayerInputMappingContext(FName InInputMappingContextID);

	// 判断当前InputActionInstance的值是否为bool类型的值。
	UFUNCTION(BlueprintCallable, Category = "Player Input")
	static bool GetBoolTypeInstanceValue(const FInputActionInstance& InInstance, bool& OutValue);

	// 判断当前InputActionInstance的值是否为Axis1D类型的值。
	UFUNCTION(BlueprintCallable, Category = "Player Input")
	static bool GetAxis1DTypeInstanceValue(const FInputActionInstance& InInstance, float& OutValue);

	// 判断当前InputActionInstance的值是否为Axis2D类型的值。
	UFUNCTION(BlueprintCallable, Category = "Player Input")
	static bool GetAxis2DTypeInstanceValue(const FInputActionInstance& InInstance, FVector2D& OutValue);

	// 判断当前InputActionInstance的值是否为Axis3D类型的值。
	UFUNCTION(BlueprintCallable, Category = "Player Input")
	static bool GetAxis3DTypeInstanceValue(const FInputActionInstance& InInstance, FVector& OutValue);

private:
	// 绑定InputAction蓝图版本。
	UFUNCTION(BlueprintCallable, Category = "Player Input|Input Action", Meta = (DisplayName = "Bind Player Input Action"))
	FPlayerInputActionBindingHandle K2_BindPlayerInputAction(FName InInputActionID, FEnhancedInputActionHandlerDynamicSignature InOnInputActionExecute);

	// 解绑InputAction蓝图版本。
	UFUNCTION(BlueprintCallable, Category = "Player Input|Input Action", Meta = (DisplayName = "Unbind Player Input Action"))
	bool K2_UnbindPlayerInputAction(UPARAM(ref) FPlayerInputActionBindingHandle& Handle);
};
