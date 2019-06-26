#pragma once

#include "CoreMinimal.h"
#include "AggregatedPoint.generated.h"

USTRUCT(BlueprintType)
struct FAggregatedPoint
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FVector WorldLocation;

	UPROPERTY(BlueprintReadOnly)
	int Size;

	UPROPERTY(BlueprintReadOnly)
	int Index;

};