#include "URGAnchorManager.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


AURGAnchorManager::AURGAnchorManager()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
}

void AURGAnchorManager::BeginPlay()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AURGConnector::StaticClass(), Actors);

	for (auto& Elem : Actors)
	{
		URGs.Emplace(Cast<AURGConnector>(Elem));
	}

	Super::BeginPlay();
}

void AURGAnchorManager::Tick(float DeltaTime)
{
	if (LocatingAnchor != nullptr)
	{
		int URGNum = URGs.Num();

		FString Message;
		float HighestScore = -1.0f;

		for (int i = 0; i < URGNum; i++)
		{
			AURGConnector* URG = URGs[i];

			if (URG->bDoing)
			{
				if (ScanProgress.Contains(URG) == false)
				{
					ScanProgress.Add(URG, 0.0f);
				}

				ScanProgress[URG] = ScanProgress[URG] + GetURGScore(URG);

				if (ScanProgress[URG] > HighestScore)
				{
					HighestScore = FMath::Clamp(ScanProgress[URG], 0.0f, RequiredScore);
				}

				if (bShowDebugMessage)
				{
					Message += FString::Printf(TEXT("%s:%f "), *URG->Com, ScanProgress[URG]);
				}

				if (ScanProgress[URG] >= RequiredScore)
				{
					UURGReadWriteFile::WritePositionData(LocatingAnchor, URG);
					URG->SetActorTransform(LocatingAnchor->GetActorTransform());
					OnFoundURG.Broadcast(LocatingAnchor, URG);
					StopFindingURG();
					break;
				}
			}
		}

		if (LocatingAnchor != nullptr)
		{
			OnFindingScoreChange.Broadcast(LocatingAnchor, HighestScore);
		}

		if (bShowDebugMessage)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
		}
	}

	Super::Tick(DeltaTime);
}

int AURGAnchorManager::ConnectAllURGs()
{
	TArray<FString> Ports = AURGConnector::FindPorts();
	int PortNum = Ports.Num();
	int URGNum = URGs.Num();

	if (PortNum <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Connect to all URGs failed, there isn't any ports connected"));
		return 0;
	}

	if (URGNum <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Connect to all URGs failed, can't find any URGConnector"));
		return 0;
	}

	if (PortNum != URGNum)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mismatch of the lidars and URGConnectors; %d lidars, %d URGConnectors"), PortNum, URGNum);
	}

	for (int i = 0; i < PortNum && i < URGNum; i++)
	{
		if (URGs[i]->bDoing)
		{
			UE_LOG(LogTemp, Error, 
				TEXT("URGConnector %s is already connected, do not start URGConnector individually. Connections should be managed by the AnchorManager"), 
				*URGs[i]->Com);
			DisconnectAllURGs();
			return 0;
		}

		URGs[i]->Com = Ports[i];
		URGs[i]->StartScan();
	}

	UURGReadWriteFile::ReadPositionData(Anchors, URGs);

	return FMath::Min(PortNum, URGNum);
}

void AURGAnchorManager::DisconnectAllURGs()
{
	int URGNum = URGs.Num();

	for (int i = 0; i < URGNum; i++)
	{
		URGs[i]->StopScanning();
	}

	StopFindingURG();
}

void AURGAnchorManager::StopFindingURG()
{
	LocatingAnchor = nullptr;
	LastFrameData.Empty();
	ScanProgress.Empty();
}

bool AURGAnchorManager::StartToFindURGForAnchorIndex(int AnchorIndex)
{
	if (LocatingAnchor != nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("It's locating another anchor now, don't find URG in the same time"));
		return false;
	}

	if (Anchors.IsValidIndex(AnchorIndex) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("%d is an invalid index for Anchors"), AnchorIndex);
		return false;
	}

	if (Anchors[AnchorIndex] == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Anchors[%d] is empty"), AnchorIndex);
		return false;
	}

	LocatingAnchor = Anchors[AnchorIndex];
	return true;
}

bool AURGAnchorManager::StartToFindURGForAnchor(AActor* Anchor)
{
	if (LocatingAnchor != nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("It's locating another anchor now, don't find URG in the same time"));
		return false;
	}

	if (Anchors.Contains(Anchor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("The target actor isn't included in the anchor list"));
		return false;
	}

	if (Anchor == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("The target actor is empty"));
		return false;
	}

	LocatingAnchor = Anchor;
	return true;
}

float AURGAnchorManager::GetURGScore(AURGConnector* URG)
{
	TArray<FURGData> ScanData = URG->Execute_GetScanDataArray(URG, true);
	TArray<FURGData> ValidData;
	int PointNum = ScanData.Num();
	float TotalScore = 0.0f;

	if (LastFrameData.Contains(URG) == false)
	{
		for (int i = 0; i < PointNum; i++)
		{
			if (ScanData[i].Length <= AcceptableDistance)
			{
				ValidData.Emplace(ScanData[i]);
			}
		}

		LastFrameData.Add(URG, ValidData);
		return 0.0f;
	}
	

	float SqrAccpectDistance = AcceptableDistance * AcceptableDistance;
	float SqrThreshold = ValidThreshold * ValidThreshold;
	
	for (int i = 0; i < PointNum; i++)
	{
		if (ScanData[i].Length > AcceptableDistance)
			continue;

		ValidData.Emplace(ScanData[i]);

		if (ValidThreshold <= 0.0f)
		{
			TotalScore += GetLengthScore(ScanData[i].Length);
			continue;
		}

		// Compare distance
		TArray<FURGData>& LastFrame = LastFrameData[URG];
		int LastNum = LastFrame.Num();
		bool bIsCloseToLastFrame = false;

		for (int p = 0; p < LastNum; p++)
		{
			if (FVector::DistSquared(ScanData[i].WorldLocation, LastFrame[p].WorldLocation) <= SqrThreshold)
			{
				bIsCloseToLastFrame = true;
				break;
			}
		}

		if (bIsCloseToLastFrame == false)
		{
			TotalScore += GetLengthScore(ScanData[i].Length);
		}
	}

	LastFrameData[URG] = ValidData;
	return TotalScore;
}

float AURGAnchorManager::GetLengthScore(float Length)
{
	float Base = 1.0f - (Length / AcceptableDistance);
	return Base * Base;
}