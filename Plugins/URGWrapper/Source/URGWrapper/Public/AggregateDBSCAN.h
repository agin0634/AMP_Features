// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "URGAggregater.h"
#include "AggregateDBSCAN.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class URGWRAPPER_API UAggregateDBSCAN : public UURGAggregater
{
	GENERATED_BODY()

public:	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (ClampMin = "0.0", ClampMax = "5.0", UIMin = "0.0", UIMax = "5.0"))
	float CentroidThickness = 0.0f;

	// Sets default values for this component's properties
	UAggregateDBSCAN();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	struct Cluster
	{
	public:
		TArray<const FURGData*> Points;

		void Add(const FURGData* Point)
		{
			Points.Emplace(Point);
		}

		bool Contains(const FURGData* Point)
		{
			return Points.Contains(Point);
		}
	};

	/**
	 * Larger: Easier to group
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EPS = 10.0f;

	/**
	 * The minimum number of points required to form a dense region
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MinPts = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MinGroupNum = 5;

	TArray<const FURGData*> VisitedArray;

	// Called when the game starts
	virtual void BeginPlay() override;

	virtual TArray<FAggregatedPoint> AggregateScanPoints(const TArray<FURGData> &ScanData) override;
	
	void ExpandCluster(const FURGData* Point, const TArray<FURGData> &ScanData, TArray<const FURGData*> NeighborPts, Cluster &C, TArray<Cluster> &Clusters);

	TArray<const FURGData*> RegionQuery(const FURGData* Point, const TArray<FURGData> &ScanData);
};
