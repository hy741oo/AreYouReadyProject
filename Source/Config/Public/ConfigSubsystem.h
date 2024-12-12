// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "GameFramework/PlayerInput.h"

#include "ConfigSubsystem.generated.h"

class UAYRUserWidget;

// Data Table��Data Asset���ʲ���Ҫ�õ��Ľṹ�塣

// ���ݱ���нṹ���ࡣ
USTRUCT()
struct FAYRTableRowBase : public FTableRowBase
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Meta = (DisplayPriority = 1))
	FName Description;
#endif
};

// ����ӳ�������ı��нṹ��
USTRUCT(BlueprintType)
struct FPlayerInputMappingTableRow : public FAYRTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UInputMappingContext* InputMappingContext = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 Priority = -1;

};

// ��ҿ�������Ϣ��
USTRUCT(BlueprintType)
struct FPlayerControllerInfoTableRow : public FAYRTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName PlayerInputMappingID;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName PlayerUIInputMappingID;
};

// UI���水���ṹ�塣
USTRUCT(BlueprintType)
struct FUIInputMapping
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName ActionName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<FInputActionKeyMapping> InputActions;
};

// UI����󶨵İ�����Ϣ��
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



