// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "GameFramework/PlayerInput.h"
#include "Fonts/SlateFontInfo.h"

#include "GameConfigSubsystem.generated.h"

class UAYRUserWidget;

DECLARE_LOG_CATEGORY_CLASS(LogGameConfigSubsystem, Log, All);

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

// 关卡信息。
USTRUCT(BlueprintType)
struct FLevelData : public FAYRTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<UWorld> Level;
};

// 按键图标信息。
USTRUCT(BlueprintType)
struct FButtonIconDataTableRow : public FAYRTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UTexture2D* IconBaseTexture = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bUseIconText = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Meta = (EditCondition = "bUseIconText == true"))
	FText IconText;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Meta = (EditCondition = "bUseIconText == true"))
	FSlateFontInfo IconTextFontInfo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Meta = (EditCondition = "bUseIconText == false"))
	UTexture2D* IconTexture = nullptr;
};

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UGameConfigSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	// 存储项目中的数据表。
	UPROPERTY()
	TMap<FName, UDataTable*> LoadedDataTables;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 通过ID获取对应的表行结构。
	template<typename TableRowType>
	bool GetDataTableRowFromID(const FName& InRowName, TableRowType*& OutTableRow)
	{
		if (ensureAlways(this->LoadedDataTables.Contains(TableRowType::StaticStruct()->GetFName())))
		{
			UDataTable** LoadedDataTable = this->LoadedDataTables.Find(TableRowType::StaticStruct()->GetFName());
			if (ensureAlways(LoadedDataTable && *LoadedDataTable))
			{
				OutTableRow = (*LoadedDataTable)->FindRow<TableRowType>(InRowName, TableRowType::StaticStruct()->GetFName().ToString());
				if (OutTableRow)
				{
					UE_LOG(LogGameConfigSubsystem, Log, TEXT("Find row \"%s\" in \"%s\""), *InRowName.ToString(), *(TableRowType::StaticStruct()->GetFName().ToString()));
					return true;
				}
				else
				{
					UE_LOG(LogGameConfigSubsystem, Warning, TEXT("Can't find row \"%s\" in \"%s\""), *InRowName.ToString(), *(TableRowType::StaticStruct()->GetFName().ToString()));
				}
			}
			else
			{
				UE_LOG(LogGameConfigSubsystem, Warning, TEXT("Can't find TableRow Struct: \"%s\""), *(TableRowType::StaticStruct()->GetFName().ToString()));
			}
		}
		else
		{
			UE_LOG(LogGameConfigSubsystem, Warning, TEXT("LoadedDataTables does not contain TableRow struct: \"%s\""), *(TableRowType::StaticStruct()->GetFName().ToString()));
		}
		return false;
	}

	// 检测指定ID的表行结构是否存在。
	template<typename TableRowType>
	bool HasDataTableRowFromID(const FName& InRowName)
	{
		TableRowType* TempTableRow = nullptr;
		this->GetDataTableRowFromID(InRowName, TempTableRow);
		return TempTableRow != nullptr;
	}
};



