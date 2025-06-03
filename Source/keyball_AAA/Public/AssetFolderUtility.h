#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/Engine.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/DataTable.h" // For UDataTable
#include "KeyboardDataStruct.h" // Updated to include the new struct
#include "AssetFolderUtility.generated.h"

USTRUCT(BlueprintType)
struct FAssetInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Asset Info")
    FString AssetName;

    UPROPERTY(BlueprintReadOnly, Category = "Asset Info")
    FString AssetPath;

    UPROPERTY(BlueprintReadOnly, Category = "Asset Info")
    FString AssetClass;

    UPROPERTY(BlueprintReadOnly, Category = "Asset Info")
    TSoftObjectPtr<UObject> AssetReference;

    FAssetInfo()
    {
        AssetName = "";
        AssetPath = "";
        AssetClass = "";
        AssetReference = nullptr;
    }
};

/**
 * Editor-only Blueprint function library for reading asset folders
 */
UCLASS()
class KEYBALL_AAA_API UAssetFolderUtility : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Get all .uasset files from a folder path
     * @param FolderPath - The folder path to search (e.g., "/Game/MyAssets/Textures")
     * @param bRecursive - Whether to search subfolders recursively
     * @param AssetClassFilter - Optional class filter (e.g., "Texture2D", leave empty for all types)
     * @return Array of asset information structs
     */
    UFUNCTION(BlueprintCallable, Category = "Asset Folder Utility", meta = (DevelopmentOnly = "true"))
    static TArray<FAssetInfo> GetAssetsFromFolder(
        const FString& FolderPath,
        bool bRecursive = true,
        const FString& AssetClassFilter = ""
    );

    /**
     * Get all asset paths from a folder (simple string array version)
     * @param FolderPath - The folder path to search
     * @param bRecursive - Whether to search subfolders recursively
     * @return Array of asset paths as strings
     */
    UFUNCTION(BlueprintCallable, Category = "Asset Folder Utility", meta = (DevelopmentOnly = "true"))
    static TArray<FString> GetAssetPathsFromFolder(
        const FString& FolderPath,
        bool bRecursive = true
    );

    /**
     * Get soft object references from a folder (for loading at runtime)
     * @param FolderPath - The folder path to search
     * @param bRecursive - Whether to search subfolders recursively
     * @param AssetClass - The UClass to filter by (optional)
     * @return Array of soft object pointers
     */
    UFUNCTION(BlueprintCallable, Category = "Asset Folder Utility", meta = (DevelopmentOnly = "true"))
    static TArray<TSoftObjectPtr<UObject>> GetSoftReferencesFromFolder(
        const FString& FolderPath,
        bool bRecursive = true,
        UClass* AssetClass = nullptr
    );

    /**
     * Update an existing row in a Data Table with new data
     * @param DataTable - The Data Table to modify
     * @param RowData - The new data to write to the row (FKeyboardDataStruct)
     * @return True if the update was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Asset Folder Utility", meta = (DevelopmentOnly = "true"))
    static bool UpdateDtRow(UDataTable* DataTable, const FKeyboardDataStruct& RowData);
};