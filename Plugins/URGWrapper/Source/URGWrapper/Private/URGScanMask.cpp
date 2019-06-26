#include "URGScanMask.h"
#include "DrawDebugHelpers.h"

AURGScanMask::AURGScanMask()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;
}

void AURGScanMask::BeginPlay()
{
	Super::BeginPlay();

}

void AURGScanMask::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AURGScanMask::EndPlay(const EEndPlayReason::Type endplayReason)
{
}