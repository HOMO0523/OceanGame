#pragma once

#include "CoreMinimal.h"
#include "OceanResourceTypes.generated.h"

UENUM(BlueprintType)
enum class EOceanResourceType : uint8
{
	Wood UMETA(DisplayName = "Wood"),
	Scrap UMETA(DisplayName = "Scrap"),
	Food UMETA(DisplayName = "Food"),
	Water UMETA(DisplayName = "Water")
};

USTRUCT(BlueprintType)
struct FOceanResourceStack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Resources")
	EOceanResourceType ResourceType = EOceanResourceType::Wood;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean|Resources", meta = (ClampMin = "0"))
	int32 Amount = 0;
};
