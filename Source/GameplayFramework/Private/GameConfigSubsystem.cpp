// Fill out your copyright notice in the Description page of Project Settings.


#include "GameConfigSubsystem.h"

#include "AssetRegistry/IAssetRegistry.h"
#include "AssetRegistry/AssetRegistryModule.h"

void UGameConfigSubsystem::Initialize(FSubsystemCollectionBase& InCollection)
{
	Super::Initialize(InCollection);

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AR = AssetRegistryModule.Get();
	FARFilter Filter;
	FName SearchingPath = TEXT("/Game/AYR/Configs");
	Filter.ClassNames.Add(UDataTable::StaticClass()->GetFName());
	Filter.PackagePaths.Add(SearchingPath);
	Filter.bRecursiveClasses = true;
	Filter.bRecursivePaths = true;
	TArray<FAssetData> SearchedAssets;

	// 某些**未知原因**项目在Standalone模式或Launch时会搜索不到Configs文件夹下的数据表，因此需要强制重新搜索一次。
	TArray<FString> RescanPaths;
	RescanPaths.Add(SearchingPath.ToString());
	AR.ScanPathsSynchronous(RescanPaths, true);

	if (AR.GetAssets(Filter, SearchedAssets))
	{
		// 如果搜索结果个数为0，则直接报错。
		int32 AssetsCount = SearchedAssets.Num();
		if (AssetsCount)
		{
			UE_LOG(LogGameConfigSubsystem, Display, TEXT("Searched Assets count is: %d"), AssetsCount);
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
				UE_LOG(LogGameConfigSubsystem, Display, TEXT("Load DataTable: %s"), *(DataTable->GetFName().ToString()));
			}
			else
			{
				UE_LOG(LogGameConfigSubsystem, Warning, TEXT("Asset is invalid, asset path: %s"), *SearchedAsset.ObjectPath.ToString());
			}
		}
	}
	else
	{
		UE_LOG(LogGameConfigSubsystem, Warning, TEXT("Asset Registry get assets failed, seaching path: %s"), *SearchingPath.ToString());
	}
}

