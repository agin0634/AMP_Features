// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScanData.h"
#include "AggregatedPoint.h"
#include "URGScanDataProvider.h"
#include "URGAggregater.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class URGWRAPPER_API UURGAggregater : public UActorComponent
{
	GENERATED_BODY()

public:	

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAggregatedPointEvent, const FAggregatedPoint &, Point);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAggregatedPointUpdateEvent, const FAggregatedPoint &, Point, const FAggregatedPoint &, From);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DetectMoving")
	bool bDetectMove = false;

	/**
	 * Max accpect moving distance of a group point in one frame
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DetectMoving")
	float MaxMoveDistance = 20.0f;

	UPROPERTY(BlueprintAssignable, Category = "DetectMoving")
	FAggregatedPointEvent OnPointEnter;

	UPROPERTY(BlueprintAssignable, Category = "DetectMoving")
	FAggregatedPointEvent OnPointLeave;

	UPROPERTY(BlueprintAssignable, Category = "DetectMoving")
	FAggregatedPointUpdateEvent OnPointMove;

	// Sets default values for this component's properties
	UURGAggregater();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	const TArray<FAggregatedPoint>& GetAggregatedPoints();

	UFUNCTION(BlueprintCallable)
	void SetScanDataProvider(TScriptInterface<IURGScanDataProvider> Provider);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	IURGScanDataProvider* ScanDataProvider;
	UObject* ObjScanDataProvider;

	TArray<FAggregatedPoint> AggregatedPoints;

	TArray<FAggregatedPoint> PreviousPoints;

	virtual TArray<FAggregatedPoint> AggregateScanPoints(const TArray<FURGData> &ScanData);

	virtual void CalculateMoving(TArray<FAggregatedPoint> &CurrentPoints);

	FAggregatedPoint* FindClosestPoint(const FAggregatedPoint* CurrentPoint, TArray<FAggregatedPoint>& CompareGroup);
};
