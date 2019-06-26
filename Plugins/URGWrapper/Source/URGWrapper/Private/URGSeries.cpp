#include "URGSeries.h"
#include "DrawDebugHelpers.h"

AURGSeries::AURGSeries()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;
}

void AURGSeries::BeginPlay()
{
	Super::BeginPlay();

}

void AURGSeries::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TArray<FURGData> AURGSeries::GetScanDataArray_Implementation(bool bFilterInvalid)
{
	TArray<FURGData> ScanData;

	for (AURGConnector* URG : URGs)
	{
		ScanData.Append(URG->Execute_GetScanDataArray(URG, bFilterInvalid));
	}

	return ScanData;
}
