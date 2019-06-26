// Fill out your copyright notice in the Description page of Project Settings.

#include "AggregateDBSCAN.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UAggregateDBSCAN::UAggregateDBSCAN()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAggregateDBSCAN::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAggregateDBSCAN::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


TArray<FAggregatedPoint> UAggregateDBSCAN::AggregateScanPoints(const TArray<FURGData> &ScanData)
{
	int ScanNum = ScanData.Num();
	VisitedArray.Empty();
	TArray<Cluster> Clusters;
	TArray<FAggregatedPoint> Result;

	// Start DBSCAN
	for (int i = 0; i < ScanNum; i++)
	{
		const FURGData* P = &ScanData[i];

		if (VisitedArray.Contains(P))
			continue;

		VisitedArray.Emplace(P);
		TArray<const FURGData*> NeighborPts = RegionQuery(P, ScanData);
		if (NeighborPts.Num() < MinPts)
		{
			// P is a noise point
		}
		else
		{
			Clusters.Emplace();
			ExpandCluster(P, ScanData, NeighborPts, Clusters.Last(), Clusters);
		}
	}

	int ClusterNum = Clusters.Num();

	for (int i = 0; i < ClusterNum; i++)
	{
		int Size = Clusters[i].Points.Num();
		if (Size < MinGroupNum)
			continue;

		int f = FMath::RoundToInt(((float)i / (float)ClusterNum) * 255.0f);
		FColor Color( f, f, f, 1 );

		// Find centroid and size
		FVector Sum(0, 0, 0);
		
		for (int p = 0; p < Size; p++)
		{
			Sum += Clusters[i].Points[p]->WorldLocation;
			if (bDrawDebug)
				DrawDebugPoint(GetWorld(), Clusters[i].Points[p]->WorldLocation, 5.0f, Color);
		}

		FAggregatedPoint Aggregated;
		Aggregated.WorldLocation = Sum / (float)Size;
		Aggregated.Size = Size;
		Aggregated.Index = -1;

		Result.Add(Aggregated);

		if (bDrawDebug)
			DrawDebugSphere(GetWorld(), Aggregated.WorldLocation, Size / 2, 16, Color, false, -1.0f, '\000', CentroidThickness);
	}

	return Result;
}

void UAggregateDBSCAN::ExpandCluster(const FURGData* Point, const TArray<FURGData> &ScanData, TArray<const FURGData*> NeighborPts, Cluster &C, TArray<Cluster> &Clusters)
{
	C.Add(Point);

	for (int i = 0; i < NeighborPts.Num(); i++)
	{
		const FURGData* P2 = NeighborPts[i];

		if (VisitedArray.Contains(P2) == false)
		{
			VisitedArray.Add(P2);
			TArray<const FURGData*> NeighborPts2 = RegionQuery(P2, ScanData);
			if (NeighborPts2.Num() >= MinPts)
			{
				NeighborPts.Append(NeighborPts2);
			}
		}

		// if P2 is not yet a member of any cluster
		bool bInCluster = false;
		for (int j = 0; j < Clusters.Num(); j++)
		{
			if (Clusters[j].Contains(P2))
			{
				bInCluster = true;
				break;
			}
		}

		if (bInCluster == false)
		{
			C.Add(P2);
		}
	}
}

TArray<const FURGData*> UAggregateDBSCAN::RegionQuery(const FURGData* Point, const TArray<FURGData> &ScanData)
{
	FVector P = Point->WorldLocation;
	float SqrEps = EPS * EPS;
	TArray<const FURGData*> Result;
	int Num = ScanData.Num();

	for (int i = 0; i < Num; i++)
	{
		if (FVector::DistSquared(P, ScanData[i].WorldLocation) <= SqrEps)
		{
			Result.Emplace(&ScanData[i]);
		}
	}

	return Result;
}