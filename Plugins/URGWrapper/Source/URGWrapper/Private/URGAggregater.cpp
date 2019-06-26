// Fill out your copyright notice in the Description page of Project Settings.

#include "URGAggregater.h"


// Sets default values for this component's properties
UURGAggregater::UURGAggregater()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UURGAggregater::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void UURGAggregater::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (ScanDataProvider != nullptr)
	{
		TArray<FURGData> ScanData = ScanDataProvider->Execute_GetScanDataArray(ObjScanDataProvider, true);
		AggregatedPoints = AggregateScanPoints(ScanData);

		if (bDetectMove)
		{
			if (PreviousPoints.Num() == 0)
			{
				for (int i = 0; i < AggregatedPoints.Num(); i++)
				{
					AggregatedPoints[i].Index = i;
					PreviousPoints.Emplace(AggregatedPoints[i]);
					OnPointEnter.Broadcast(AggregatedPoints[i]);
				}
			}
			else
			{
				CalculateMoving(AggregatedPoints);
			}
		}
		else
		{
			for (int i = 0; i < PreviousPoints.Num(); i++)
			{
				OnPointLeave.Broadcast(PreviousPoints[i]);
			}

			PreviousPoints.Empty();
		}
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UURGAggregater::SetScanDataProvider(TScriptInterface<IURGScanDataProvider> Provider)
{
	ObjScanDataProvider = Provider.GetObject();
	ScanDataProvider = Cast<IURGScanDataProvider>(ObjScanDataProvider);
}

const TArray<FAggregatedPoint>& UURGAggregater::GetAggregatedPoints()
{
	return AggregatedPoints;
}

TArray<FAggregatedPoint> UURGAggregater::AggregateScanPoints(const TArray<FURGData> &ScanData)
{
	UE_LOG(LogTemp, Warning, TEXT("Must override UURGAggregater::AggregatePoints"));
	return TArray<FAggregatedPoint>{};
}

void UURGAggregater::CalculateMoving(TArray<FAggregatedPoint> &CurrentPoints)
{
	// update index of current group by comparing distance from previous groups
	int LastIndex = PreviousPoints.Last().Index + 1;
	TArray<FAggregatedPoint*> AssignedPoints;

	for (int i = PreviousPoints.Num() - 1; i >= 0; i--)
	{
		FAggregatedPoint* ClosestPoint = FindClosestPoint(&PreviousPoints[i], CurrentPoints);
		if (ClosestPoint == nullptr)
		{
			// The previous point was gone
			OnPointLeave.Broadcast(PreviousPoints[i]);
			PreviousPoints.RemoveAt(i);
		}
		else
		{
			// some one just tracked this point
			if (AssignedPoints.Contains(ClosestPoint))
			{
				OnPointLeave.Broadcast(PreviousPoints[i]);
				PreviousPoints.RemoveAt(i);
			}
			else
			{
				// The previous is moving, assign correct index to current point
				AssignedPoints.Emplace(ClosestPoint);

				ClosestPoint->Index = PreviousPoints[i].Index;
				OnPointMove.Broadcast(*ClosestPoint, PreviousPoints[i]);
				PreviousPoints[i] = *ClosestPoint;
			}
		}
	}

	// find out new points
	for (int i = 0; i < CurrentPoints.Num(); i++)
	{
		if (CurrentPoints[i].Index < 0)
		{
			CurrentPoints[i].Index = LastIndex;
			LastIndex += 1;
			OnPointEnter.Broadcast(CurrentPoints[i]);
			PreviousPoints.Emplace(CurrentPoints[i]);
		}
	}
}

FAggregatedPoint* UURGAggregater::FindClosestPoint(const FAggregatedPoint* CurrentPoint, TArray<FAggregatedPoint>& CompareGroup)
{
	FVector Position = CurrentPoint->WorldLocation;

	float SqrMaxDistance = MaxMoveDistance * MaxMoveDistance;
	float MinDistance = FLT_MAX;
	int MinIndex = -1;

	int GroupNum = CompareGroup.Num();
	for (int i = 0; i < GroupNum; i++)
	{
		float SqrDistance = FVector::DistSquared(Position, CompareGroup[i].WorldLocation);
		if (SqrDistance < SqrMaxDistance && SqrDistance < MinDistance)
		{
			MinDistance = SqrDistance;
			MinIndex = i;
		}
	}

	if (MinIndex < 0)
		return nullptr;

	return &CompareGroup[MinIndex];
}