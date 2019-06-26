#include "URGDataFilterComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UURGDataFilterComponent::UURGDataFilterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UURGDataFilterComponent::BeginPlay()
{
	// ...
	Owner = Cast<IURGScanDataProvider>(GetOwner());

	// Make sure there is no nullptr in black list and white list
	for (int i = WhiteList.Num() - 1; i >= 0; i--)
	{
		if (WhiteList[i] == nullptr)
		{
			WhiteList.RemoveAt(i);
		}
	}

	for (int i = BlackList.Num() - 1; i >= 0; i--)
	{
		if (BlackList[i] == nullptr)
		{
			BlackList.RemoveAt(i);
		}
	}

	Super::BeginPlay();
}


TArray<FURGData> UURGDataFilterComponent::GetScanDataArray_Implementation(bool bFilterInvalid)
{
	TArray<FURGData> ScanData;

	if (Owner == nullptr)
	{
		return ScanData;
	}

	ScanData = Owner->Execute_GetScanDataArray(GetOwner(), true);
	int Num = ScanData.Num();
	int WhiteNum = WhiteList.Num();
	int BlackNum = BlackList.Num();

	UWorld* World = GetWorld();

	for (int i = Num - 1; i >= 0; i--)
	{
		FURGData* Data = &ScanData[i];

		bool bInWhiteList = false;
		for (int w = 0; w < WhiteNum; w++)
		{
			if (WhiteList[w]->IsInside(Data->WorldLocation))
			{
				bInWhiteList = true;
				break;
			}
		}

		if (bInWhiteList)
		{
			if (bDrawDebug)
			{
				DrawDebugPoint(World, Data->WorldLocation, DebugSize, ValidColor);
			}

			continue;
		}

		if (bDrawDebug)
		{
			DrawDebugPoint(World, Data->WorldLocation, DebugSize, ValidColor);
		}

		for (int b = 0; b < BlackNum; b++)
		{
			if (BlackList[b]->IsInside(Data->WorldLocation))
			{
				if (bDrawDebug)
				{
					DrawDebugPoint(World, Data->WorldLocation, DebugSize, InvalidColor);
				}

				ScanData.RemoveAt(i);
				break;
			}
		}

	}

	return ScanData;
}