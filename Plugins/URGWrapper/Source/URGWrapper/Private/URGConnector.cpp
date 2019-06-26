#include "URGConnector.h"
#include "Urg_driver.h"
#include "math_utilities.h"
#include "iostream"
#include "Connection_information.h"
#include "DrawDebugHelpers.h"

using namespace std;
using namespace qrk;

class FScanTask : public FNonAbandonableTask
{

private:

	AURGConnector* Connector;
	Urg_driver Urg;
	FCriticalSection CriticalSection;

	vector<long> UrgDistanceData;
	FString Com;

public:

	FScanTask(AURGConnector* connector, FString com)
	{
		Connector = connector;
		Com = com;
	}

	~FScanTask()
	{
		Connector = nullptr;
		
		// end connection
		Urg.stop_measurement();
		Urg.close();

		UE_LOG(LogTemp, Warning, TEXT("URG %s STOPPED"), *Com);
	}

	bool IsConnectorValid()
	{
		if (IsValid(Connector) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Connector is invalid"));
			return false;
		}

		if (Connector->bDoing == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Connector isn't doing"));
			return false;
		}

		return true;
	}


	void DoWork()
	{
		if (IsConnectorValid() == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Connector pointer is invalid"));
			return;
		}

		Urg.open(TCHAR_TO_ANSI(*Com));
		Urg.set_scanning_parameter(Urg.deg2step(-Connector->ScanAngle), Urg.deg2step(Connector->ScanAngle), Connector->SkinScan);
		Urg.start_measurement(Urg_driver::Distance, Urg_driver::Infinity_times, Connector->SkinScan);

		long TimeStamp = 0;

		while (IsConnectorValid())
		{
			if (Urg.is_open() == false)
			{
				UE_LOG(LogTemp, Warning, TEXT("URG %s is not open, stop the worker"), *Com);
				if (IsValid(Connector))
				{
					Connector->bDoing = false;
				}
				break;
			}

			Urg.get_distance(UrgDistanceData, &TimeStamp);

			if (IsConnectorValid() == false)
			{
				break;
			}

			CriticalSection.Lock();
			UpdateLocationData();
			CriticalSection.Unlock();
		}
	}

	void UpdateLocationData()
	{
		if (IsConnectorValid() == false)
		{
			return;
		}

		if (Connector->ScanDataArray.Num() != UrgDistanceData.size())
		{
			Connector->ScanDataArray.SetNum(UrgDistanceData.size());
			UE_LOG(LogTemp, Warning, TEXT("URG %s Resize DataArray to %d"), *Com, Connector->ScanDataArray.Num());
		}

		float Scale = Connector->Scale;
		long MinDistance = (long)(Connector->MinDistance * 10.0f);
		FTransform ActorTransform = Connector->GetActorTransform();
		FVector Position;

		for (size_t i = 0; i < UrgDistanceData.size(); i++)
		{
			double Radian = Urg.index2rad(i);
			long Length = UrgDistanceData[i];
			float LengthInCM = Length * 0.1f;

			Connector->ScanDataArray[i].Length = LengthInCM;

			if (Length <= MinDistance)
			{
				Connector->ScanDataArray[i].bIsValid = false;
				continue;
			}
			else
			{
				Connector->ScanDataArray[i].bIsValid = true;
			}

			// to cm
			Position.X = (long)(Length * 0.1f * cos(Radian)) * Scale;
			Position.Y = (long)(Length * -0.1f * sin(Radian)) * Scale;
			Position.Z = 0.0f;

			// to world
			Connector->ScanDataArray[i].WorldLocation = ActorTransform.TransformPosition(Position);
		}
	}

	/*
	Need this function
	example: https://wiki.unrealengine.com/Using_AsyncTasks
	 */
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FScanTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};

AURGConnector::AURGConnector()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

}

void AURGConnector::BeginPlay()
{
	Super::BeginPlay();
}

void AURGConnector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AURGConnector::EndPlay(const EEndPlayReason::Type endplayReason)
{
	StopScanning();
}

void AURGConnector::StartScan()
{
	bDoing = true;
	ScanTask = (new FAutoDeleteAsyncTask<FScanTask>(this, Com));
	ScanTask->StartBackgroundTask();
}

void AURGConnector::StopScanning()
{
	bDoing = false;
	ScanTask = nullptr;
}

TArray<FURGData> AURGConnector::GetScanDataArray_Implementation(bool bFilterInvalid)
{
	TArray<FURGData> Result;
	int PointNum = ScanDataArray.Num();

	if (bFilterInvalid)
	{
		for (int i = 0; i < PointNum; i++)
		{
			if (ScanDataArray[i].bIsValid)
			{
				Result.Emplace(ScanDataArray[i]);
			}
		}
	}
	else
	{
		Result = ScanDataArray;
	}

	if (bDrawDebug)
	{
		UWorld* World = GetWorld();
		int Num = Result.Num();

		for (int i = 0; i < Num; i++)
		{
			DrawDebugPoint(World, Result[i].WorldLocation, DebugSize, DebugColor);
		}
	}

	return Result;
}

TArray<FString> AURGConnector::FindPorts()
{
	TArray<FString> Result;
	Urg_driver Urg;
	std::vector<int> is_urg_ports;
	vector<string> ports = Urg.find_ports(is_urg_ports);

	for (size_t i = 0; i < ports.size(); i++)
	{
		if (is_urg_ports[i])
		{
			Result.Add(ports[i].c_str());
		}
	}

	return Result;
}
