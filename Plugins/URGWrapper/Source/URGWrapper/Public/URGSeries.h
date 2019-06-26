#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScanData.h"
#include "URGScanDataProvider.h"
#include "URGConnector.h"
#include "URGDataFilterComponent.h"
#include "URGSeries.generated.h"


UCLASS(Blueprintable)
class URGWRAPPER_API AURGSeries : public AActor, public IURGScanDataProvider
{
	GENERATED_BODY()

public:

	AURGSeries();

	virtual void Tick(float DeltaTime) override;

	// Interface IURGScanDataProvider
	virtual TArray<FURGData> GetScanDataArray_Implementation(bool bFilterInvalid = true) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<AURGConnector*> URGs;

	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleDefaultsOnly)
		USceneComponent* Root;
};

