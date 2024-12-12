// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "GameFramework/PlayerInput.h"

#include "ConfigSubsystem.generated.h"

class UAYRUserWidget;

// Data Table、Data Asset等资产需要用到的结构体。

// 数据表表行结构基类。
USTRUCT()
struct FAYRTableRowBase : public FTableRowBase
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Meta = (DisplayPriority = 1))
	FName Description;
#endif
};

// 输入映射上下文表行结构。
USTRUCT(BlueprintType)
struct FPlayerInputMappingTableRow : public FAYRTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UInputMappingContext* InputMappingContext = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 Priority = -1;

};

// 玩家控制器信息。
USTRUCT(BlueprintType)
struct FPlayerControllerInfoTableRow : public FAYRTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName PlayerInputMappingID;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName PlayerUIInputMappingID;
};

// UI界面按键结构体。
USTRUCT(BlueprintType)
struct FUIInputMapping
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName ActionName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<FInputActionKeyMapping> InputActions;
};

// UI界面绑定的按键信息。
USTRUCT(BlueprintType)
struct FPlayerUIInputMappingTableRow : public FAYRTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FUIInputMapping> UIInputActions;
};

/**
 * 
 */
UCLASS()
class CONFIG_API UConfigSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	TMap<const FName, TMap<FName, uint8*>> LoadedDataTables;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	template<typename TableRowType>
	bool GetDataTableRowFromID(const FName& InRowName, TableRowType*& OutTableRow)
	{
		if (this->LoadedDataTables.Contains(TableRowType::StaticStruct()->GetFName()))
		{
			TMap<FName, uint8*>* Temp = this->LoadedDataTables.Find(TableRowType::StaticStruct()->GetFName());
			if (Temp->Contains(InRowName))
			{
				if (uint8** RowStruct = Temp->Find(InRowName))
				{
					ensureAlways(OutTableRow = (TableRowType*)*RowStruct);
					return true;
				}
			}
		}
		return false;
	}

};



