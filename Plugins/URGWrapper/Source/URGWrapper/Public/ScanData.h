#pragma once

#include "CoreMinimal.h"
#include "ScanData.generated.h"

USTRUCT(BlueprintType)
struct FURGData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FVector WorldLocation;

	UPROPERTY(BlueprintReadOnly)
	bool bIsValid;

	/* Length in CM */
	UPROPERTY(BlueprintReadOnly)
	float Length;

};