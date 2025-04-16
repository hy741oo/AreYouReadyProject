// Fill out your copyright notice in the Description page of Project Settings.


#include "GameConfigSubsystem.h"

#include "AssetRegistry/IAssetRegistry.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Algo/Transform.h"
#include "AYRFunctionLibrary.h"

void UGameConfigSubsystem::Initialize(FSubsystemCollectionBase& InCollection)
{
	Super::Initialize(InCollection);

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AR = AssetRegistryModule.Get();
	FARFilter Filter;
	Filter.ClassNames.Add(UDataTable::StaticClass()->GetFName());
	TArray<FName> DataTablePaths;
	Algo::Transform(
		GetDefault<UAYRSettings>()->DataTableDirectory,
		DataTablePaths,
		[](const FDirectoryPath& Path) -> FName
		{
			return FName(*Path.Path);
		}
	);
	Filter.PackagePaths = DataTablePaths;
	Filter.bRecursiveClasses = true;
	Filter.bRecursivePaths = true;
	TArray<FAssetData> SearchedAssets;

	// 某些**未知原因**项目在Standalone模式或Launch时会搜索不到Configs文件夹下的数据表，因此需要强制重新搜索一次。
	TArray<FString> RescanPaths;
	Algo::Transform(
		DataTablePaths,
		RescanPaths,
		[](const FName& InName) -> FString
		{
			return InName.ToString();
		}
	);
	AR.ScanPathsSynchronous(RescanPaths, true);

	if (AR.GetAssets(Filter, SearchedAssets))
	{
		// 如果搜索结果个数为0，则直接报错。
		int32 AssetsCount = SearchedAssets.Num();
		if (AssetsCount)
		{
			UE_LOG(LogGameConfigSubsystem, Log, TEXT("Searched Assets count is: %d"), AssetsCount);
		}
		else
		{
			UE_LOG(LogGameConfigSubsystem, Error, TEXT("Searched Assets count is: %d"), AssetsCount);
		}

		for (const FAssetData& SearchedAsset : SearchedAssets)
		{
			if (SearchedAsset.IsValid())
			{
				UDataTable* DT = CastChecked<UDataTable>(SearchedAsset.GetAsset());
				UDataTable*& DataTable = this->LoadedDataTables.Add(DT->GetRowStruct()->GetFName());
				DataTable = DT;
				UE_LOG(LogGameConfigSubsystem, Log, TEXT("Load DataTable: %s"), *(DataTable->GetFName().ToString()));
			}
			else
			{
				UE_LOG(LogGameConfigSubsystem, Warning, TEXT("Asset is invalid, asset path: %s"), *SearchedAsset.ObjectPath.ToString());
			}
		}
	}
	else
	{
		UE_LOG(LogGameConfigSubsystem, Warning, TEXT("Asset Registry get assets failed."));
		for (const FString& Path : RescanPaths)
		{
			UE_LOG(LogGameConfigSubsystem, Warning, TEXT("%s"), *Path);
		}
	}
}

bool UGameConfigSubsystem::GetCurrentInputIconData(const FName InRowName, FInputIconDataTableRow& OutInputIconDataTableRow, FSlateBrush& OutIconBrush) const
{
	UAYRGameInstance* GI = Cast<UAYRGameInstance>(this->GetGameInstance());
	bool bIsFind = this->GetInputIconData(InRowName, GI->GetCurrentInputDeviceType(), OutInputIconDataTableRow, OutIconBrush);

	return bIsFind;
}

bool UGameConfigSubsystem::GetInputIconData(const FName InRowName, EInputDeviceType::Type InInputDeviceType, FInputIconDataTableRow& OutInputIconDataTableRow, FSlateBrush& OutIconBrush) const
{
	// 获取输入设备按键数据表行结构。
	const FInputIconDataTableRow* Row = nullptr;
	bool bIconIsFind = false;
	bool bTableRowIsFind = this->GetDataTableRowFromID(InRowName, Row);
	if (bTableRowIsFind)
	{
		OutInputIconDataTableRow = *Row;

		// 获取输入按键对应的图标信息。
		if (const FKey* Key = Row->InputKeys.Find(InInputDeviceType))
		{
			if (UInputIconDataAsset* DataAsset = GetDefault<UAYRSettings>()->InputIconData.LoadSynchronous())
			{
				if (FSlateBrush* Brush = DataAsset->InputIconData.Find(*Key))
				{
					OutIconBrush = *Brush;
					bIconIsFind = true;
				}
			}
		}
	}

	return bTableRowIsFind && bIconIsFind;
}

