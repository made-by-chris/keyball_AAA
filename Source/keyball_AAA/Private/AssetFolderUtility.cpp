#include "AssetFolderUtility.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Engine.h"
#include "UObject/SoftObjectPath.h"

TArray<FAssetInfo> UAssetFolderUtility::GetAssetsFromFolder(
    const FString& FolderPath, 
    bool bRecursive, 
    const FString& AssetClassFilter)
{
    TArray<FAssetInfo> AssetInfos;

#if WITH_EDITOR
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    FARFilter Filter;
    Filter.PackagePaths.Add(FName(*FolderPath));
    Filter.bRecursivePaths = bRecursive;
    
    if (!AssetClassFilter.IsEmpty())
    {
        Filter.ClassNames.Add(FName(*AssetClassFilter));
    }

    TArray<FAssetData> AssetDataArray;
    AssetRegistry.GetAssets(Filter, AssetDataArray);

    for (const FAssetData& AssetData : AssetDataArray)
    {
        FAssetInfo Info;
        Info.AssetName = AssetData.AssetName.ToString();
        Info.AssetPath = AssetData.ObjectPath.ToString();
        Info.AssetClass = AssetData.AssetClass.ToString();
        Info.AssetReference = TSoftObjectPtr<UObject>(FSoftObjectPath(AssetData.ObjectPath));
        
        AssetInfos.Add(Info);
    }

    AssetInfos.Sort([](const FAssetInfo& A, const FAssetInfo& B)
    {
        FString NameA = A.AssetName;
        FString NameB = B.AssetName;
        
        int32 LastUnderscoreA = NameA.Find(TEXT("_"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
        int32 LastUnderscoreB = NameB.Find(TEXT("_"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
        
        if (LastUnderscoreA != INDEX_NONE && LastUnderscoreB != INDEX_NONE)
        {
            FString NumStrA = NameA.Mid(LastUnderscoreA + 1);
            FString NumStrB = NameB.Mid(LastUnderscoreB + 1);
            
            int32 NumA = FCString::Atoi(*NumStrA);
            int32 NumB = FCString::Atoi(*NumStrB);
            
            return NumA < NumB;
        }
        
        return NameA < NameB;
    });

    UE_LOG(LogTemp, Log, TEXT("Found %d assets in folder: %s"), AssetInfos.Num(), *FolderPath);
#else
    UE_LOG(LogTemp, Warning, TEXT("GetAssetsFromFolder can only be used in editor builds"));
#endif

    return AssetInfos;
}

TArray<FString> UAssetFolderUtility::GetAssetPathsFromFolder(
    const FString& FolderPath, 
    bool bRecursive)
{
    TArray<FString> AssetPaths;

#if WITH_EDITOR
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    FARFilter Filter;
    Filter.PackagePaths.Add(FName(*FolderPath));
    Filter.bRecursivePaths = bRecursive;

    TArray<FAssetData> AssetDataArray;
    AssetRegistry.GetAssets(Filter, AssetDataArray);

    for (const FAssetData& AssetData : AssetDataArray)
    {
        AssetPaths.Add(AssetData.ObjectPath.ToString());
    }
#endif

    return AssetPaths;
}

TArray<TSoftObjectPtr<UObject>> UAssetFolderUtility::GetSoftReferencesFromFolder(
    const FString& FolderPath, 
    bool bRecursive, 
    UClass* AssetClass)
{
    TArray<TSoftObjectPtr<UObject>> SoftReferences;

#if WITH_EDITOR
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    FARFilter Filter;
    Filter.PackagePaths.Add(FName(*FolderPath));
    Filter.bRecursivePaths = bRecursive;
    
    if (AssetClass)
    {
        Filter.ClassNames.Add(AssetClass->GetFName());
    }

    TArray<FAssetData> AssetDataArray;
    AssetRegistry.GetAssets(Filter, AssetDataArray);

    for (const FAssetData& AssetData : AssetDataArray)
    {
        TSoftObjectPtr<UObject> SoftRef(FSoftObjectPath(AssetData.ObjectPath));
        SoftReferences.Add(SoftRef);
    }
#endif

    return SoftReferences;
}

