#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "URGConnector.h"
#include "URGScanMask.generated.h"


UCLASS(Blueprintable)
class URGWRAPPER_API AURGScanMask : public AActor
{
	GENERATED_BODY()

public:

	AURGScanMask();

	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type endplayReason) override;

	virtual bool IsInside(const FVector &In) { return false; }

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly)
		USceneComponent* Root;
};

