#pragma once

#include "CoreMinimal.h"
#include "URGScanMask.h"
#include "Components/BoxComponent.h"
#include "URGScanMaskBox.generated.h"


UCLASS(Blueprintable)
class URGWRAPPER_API AURGScanMaskBox : public AURGScanMask
{
	GENERATED_BODY()

public:

	AURGScanMaskBox();

	virtual bool IsInside(const FVector &In) override;

protected:

	UPROPERTY(VisibleDefaultsOnly)
		UBoxComponent* Box;
};

