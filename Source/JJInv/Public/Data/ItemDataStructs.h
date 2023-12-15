// Copyright © Jodiah Jensen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "ItemDataStructs.generated.h"

UENUM()
enum class EItemQuality : uint8
{
	Useless,
	Shoddy,
	Common,
	Rare,
	Epic,
	Legendary,
	Mythic,
	Unique
};

UENUM()
enum class EItemType : uint8
{
	Junk,
	Tool,
	Armor,
	Weapon,
	Shield,
	Spell,
	Ammo,
	Consumable,
	Mundane
};

USTRUCT()
struct FItemStatistics
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float SellValue;

	UPROPERTY(EditAnywhere)
	float ArmorRating;

	UPROPERTY(EditAnywhere)
	float DamageValue;

	UPROPERTY(EditAnywhere)
	float RestorationAmount;
};

USTRUCT()
struct FItemTextData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FText Name;

	UPROPERTY(EditAnywhere)
	FText Description;

	UPROPERTY(EditAnywhere)
	FText InteractText;

	UPROPERTY(EditAnywhere)
	FText UsageText;

	UPROPERTY(EditAnywhere)
	FText WhenUsedText;
};

USTRUCT()
struct FItemNumericData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Weight;

	UPROPERTY(EditAnywhere)
	int32 MaxStackSize;

	UPROPERTY(EditAnywhere)
	bool bIsStackable;

	UPROPERTY(EditAnywhere, Category="Item Data")
	bool bIsQuestItem;
};

USTRUCT()
struct FItemAssetData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere)
	UStaticMesh* Mesh;
	
};

USTRUCT()
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Item Data")
	FName ItemID;
	
	UPROPERTY(EditAnywhere, Category="Item Data")
	EItemType ItemType;

	
	UPROPERTY(EditAnywhere, Category="Item Data")
	EItemQuality ItemQuality;

	UPROPERTY(EditAnywhere, Category="Item Data")
	FItemStatistics ItemStatistics;

	UPROPERTY(EditAnywhere, Category="Item Data")
	FItemTextData ItemTextData;

	UPROPERTY(EditAnywhere, Category="Item Data")
	FItemNumericData ItemNumericData;

	UPROPERTY(EditAnywhere, Category="Item Data")
	FItemAssetData ItemAssetData;
	
};
