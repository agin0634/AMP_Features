#pragma once

#include "CoreMinimal.h"
#include "URGScanMask.h"
#include "Components/SphereComponent.h"
#include "URGScanMaskSphere.generated.h"


UCLASS(Blueprintable)
class URGWRAPPER_API AURGScanMaskSphere : public AURGScanMask
{
	GENERATED_BODY()

public:

	AURGScanMaskSphere();

	virtual bool IsInside(const FVector &In) override;

protected:

	UPROPERTY(VisibleDefaultsOnly)
		USphereComponent* Sphere;
};

