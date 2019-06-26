#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScanData.h"
#include "URGScanDataProvider.h"
#include "URGScanMask.h"
#include "URGDataFilterComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class URGWRAPPER_API UURGDataFilterComponent : public UActorComponent, public IURGScanDataProvider
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UURGDataFilterComponent();

	// Interface IURGScanDataProvider
	virtual TArray<FURGData> GetScanDataArray_Implementation(bool bFilterInvalid = true) override;

protected:

	IURGScanDataProvider* Owner;

	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawDebug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		FColor ValidColor = FColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		FColor InvalidColor = FColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		float DebugSize = 5.0f;

	UPROPERTY(EditAnywhere)
		TArray<AURGScanMask*> WhiteList;

	UPROPERTY(EditAnywhere)
		TArray<AURGScanMask*> BlackList;
};

