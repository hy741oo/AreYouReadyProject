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
	TArray<FString> ForceScanPaths;
	Algo::Transform(
		GetDefault<UAYRSettings>()->DataTableDirectory,
		DataTablePaths,
		[&ForceScanPaths](const FDirectoryPath& Path) -> FName
		{
			ForceScanPaths.Add(Path.Path);
			return FName(*Path.Path);
		}
	);
	Filter.PackagePaths = DataTablePaths;
	Filter.bRecursiveClasses = true;
	Filter.bRecursivePaths = true;
	TArray<FAssetData> SearchedAssets;

	// 由于子系统初始化时AR模块可能还没有初始化完毕，所以在这里我们需要强制优先加载数据表目录。
	AR.ScanPathsSynchronous(ForceScanPaths, true);

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
#if (!UE_BUILD_SHIPPING || UE_BUILD_TEST)
				// 手动将每个加载的数据表的DisplayRowName用RowName填充，供以后项目调试时可以查询当前表行结构是哪个RowName。
				TArray<FName> RowNames = DT->GetRowNames();
				for (const FName& RowName : RowNames)
				{
					if (FAYRTableRowBase* TableRow = DT->FindRow<FAYRTableRowBase>(RowName, TEXT("GameConfigSubsystem Initialize")))
					{
						TableRow->DisplayRowName = RowName;
					}
				}
#endif
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
		for (const FString& Path : ForceScanPaths)
		{
			UE_LOG(LogGameConfigSubsystem, Warning, TEXT("%s"), *Path);
		}
	}

	// 存储输入按钮图标数据资产。
	this->InputIconDataAsset = GetDefault<UAYRSettings>()->InputIconData.LoadSynchronous();

}

bool UGameConfigSubsystem::GetCurrentInputIconData(const FName& InRowName, const FInputIconDataTableRow*& OutInputIconDataTableRow, const FSlateBrush*& OutIconBrush) const
{
	UAYRGameInstance* GI = Cast<UAYRGameInstance>(this->GetGameInstance());
	bool bIsFind = this->GetInputIconData(InRowName, GI->GetCurrentInputDeviceType(), OutInputIconDataTableRow, OutIconBrush);

	return bIsFind;
}

bool UGameConfigSubsystem::GetInputIconData(const FName& InRowName, EInputDeviceType::Type InInputDeviceType, const FInputIconDataTableRow*& OutInputIconDataTableRow, const FSlateBrush*& OutIconBrush) const
{
	// 获取输入设备按键数据表行结构。
	bool bIconIsFind = false;
	bool bTableRowIsFind = this->GetDataTableRowFromID(InRowName, OutInputIconDataTableRow);
	if (bTableRowIsFind)
	{

		// 获取输入按键对应的图标信息。
		if (const FKey* Key = OutInputIconDataTableRow->InputKeys.Find(InInputDeviceType))
		{
			if (this->InputIconDataAsset)
			{
				if (FSlateBrush* Brush = this->InputIconDataAsset->InputIconData.Find(*Key))
				{
					OutIconBrush = Brush;
					bIconIsFind = true;
				}
			}
		}
	}

	return bTableRowIsFind && bIconIsFind;
}


