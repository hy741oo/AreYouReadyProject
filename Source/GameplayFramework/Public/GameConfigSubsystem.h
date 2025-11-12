// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "GameFramework/PlayerInput.h"
#include "Fonts/SlateFontInfo.h"
#include "AYRInputProcessor.h"
#include "AYRGameInstance.h"

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
	// 用于描述行信息。
	UPROPERTY(EditDefaultsOnly, Category = "Editor", Meta = (DisplayPriority = 0))
	FName Description;
#endif

#if (!UE_BUILD_SHIPPING || UE_BUILD_TEST)
	// 显示用的行名称，用于代码调试是表明该数据的RowName。
	FName DisplayRowName;
#endif
};

// 数据资产类型基类。
UCLASS()
class UAYRDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Meta = (DisplayPriority = 1))
	FName Description;
#endif
};

// 玩家控制器信息。
USTRUCT(BlueprintType)
struct FPlayerControllerInfoTableRow : public FAYRTableRowBase
{
	GENERATED_BODY()

	// 摄像机管理器最大Pitch。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Camera Manager")
	float CameraManagerLimitPitchMax = 89.9f;

	// 摄像机管理器最小Pitch。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Camera Manager")
	float CameraManagerLimitPitchMin = -89.9f;

	// 摄像机管理器最大Yaw。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Camera Manager")
	float CameraManagerLimitYawMax = 359.9f;

	// 摄像机管理器最小Yaw。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Camera Manager")
	float CameraManagerLimitYawMin = 0.f;

	// Pitch输入系数。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input")
	float InputPitchScale = 1.f;

	// Yaw输入系数。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input")
	float InputYawScale = 1.f;

	// Roll输入系数。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input")
	float InputRollScale = 1.f;
};

// 关卡信息。
USTRUCT(BlueprintType)
struct FLevelData : public FAYRTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<UWorld> Level;
};

// 按键信息。
USTRUCT(BlueprintType)
struct FInputIconDataTableRow : public FAYRTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "General")
	TMap<TEnumAsByte<EInputDeviceType::Type>, FKey> InputKeys;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "General")
	bool bCollapseOnIconHidden = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Text")
	bool bUseIconHintText = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Text", Meta = (EditCondition = "bUseIconHintText == true"))
	FText IconText;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Text", Meta = (EditCondition = "bUseIconHintText == true"))
	FSlateColor ColorAndOpacity = FLinearColor::White;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Text", Meta = (EditCondition = "bUseIconHintText == true"))
	FSlateFontInfo IconTextFontInfo;

	FInputIconDataTableRow()
	{
		UObject* FontObject = LoadObject<UObject>(nullptr, TEXT("Font'/Engine/EngineFonts/Roboto.Roboto'"));
		IconTextFontInfo = FSlateFontInfo(FontObject, 20);
	}
		
};

// 按键图标信息。
UCLASS(BlueprintType)
class UInputIconDataAsset : public UAYRDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<FKey, FSlateBrush> InputIconData;

	UFUNCTION(BlueprintCallable)
	bool GetInputIconDataByKey(FKey InKey, FSlateBrush& OutIconBrush) const
	{
		const FSlateBrush* IconBrush = nullptr;
		IconBrush = this->InputIconData.Find(InKey);
		if (IconBrush)
		{
			OutIconBrush = *IconBrush;
			return true;
		}
		return false;
	}
};

UCLASS(Config = Game, DefaultConfig)
class UAYRSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// 带文件夹选择器的数据表存储位置。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Config, Category = "Game Config DataTable", Meta = (ContentDir))
	TArray<FDirectoryPath> DataTableDirectory;

	// 输入按键Icon的数据资产。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Config, Category = "Input")
	TSoftObjectPtr<UInputIconDataAsset> InputIconData;

public:
	virtual FName GetContainerName() const override
	{
		return "Project";
	}

	virtual FName GetCategoryName() const override
	{
		return "AYR Project";
	}

#if WITH_EDITOR
	virtual FText GetSectionText() const override
	{
		return FText::FromString("AYR Settings");
	}
#endif
};

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UGameConfigSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class UAYRFunctionLibrary;
	
private:
	// 存储项目中的数据表。需要管理GC。
	UPROPERTY()
	TMap<FName, UDataTable*> LoadedDataTables;

	// 存储案件图标表格。需要管理GC。
	UPROPERTY()
	UInputIconDataAsset* InputIconDataAsset = nullptr;

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
					UE_LOG(LogGameConfigSubsystem, Verbose, TEXT("Find row \"%s\" in \"%s\""), *InRowName.ToString(), *(TableRowType::StaticStruct()->GetFName().ToString()));
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

	// 常量版本。
	template<typename TableRowType>
	bool GetDataTableRowFromID(const FName& InRowName, const TableRowType*& OutTableRow) const
	{
		if (ensureAlways(this->LoadedDataTables.Contains(TableRowType::StaticStruct()->GetFName())))
		{
			UDataTable* const* LoadedDataTable = this->LoadedDataTables.Find(TableRowType::StaticStruct()->GetFName());
			if (ensureAlways(LoadedDataTable && *LoadedDataTable))
			{
				OutTableRow = (*LoadedDataTable)->FindRow<TableRowType>(InRowName, TableRowType::StaticStruct()->GetFName().ToString());
				if (OutTableRow)
				{
					UE_LOG(LogGameConfigSubsystem, Verbose, TEXT("Find row \"%s\" in \"%s\""), *InRowName.ToString(), *(TableRowType::StaticStruct()->GetFName().ToString()));
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

	// 获取指定输入设备按键图标信息。
	bool GetInputIconData(const FName& InRowName, EInputDeviceType::Type InInputDeviceType, const FInputIconDataTableRow*& OutInputIconDataTableRow, const FSlateBrush*& OutIconBrush) const;

	// 获取当前输入设备按键图标信息。
	bool GetCurrentInputIconData(const FName& InRowName, const FInputIconDataTableRow*& OutInputIconDataTableRow, const FSlateBrush*& OutIconBrush) const;
};



