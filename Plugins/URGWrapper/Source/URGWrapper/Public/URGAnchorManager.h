/*
 * AnchorManager is designed to manage all URG connections automatically
 * Every anchors in the AnchorManager maps to a lidar in the real world
 * It provides a calibration flow to help user to setup all lidars without needing to know the COM numbers of the lidars
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "URGConnector.h"
#include "URGReadWriteFile.h"
#include "URGAnchorManager.generated.h"

UCLASS(Blueprintable)
class URGWRAPPER_API AURGAnchorManager : public AActor
{
	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FURGMatchDelegate, AActor*, Anchor, AURGConnector*, URG);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FURGMatchScoreDelegate, AActor*, Anchor, float, HighestScore);

	UPROPERTY(BlueprintAssignable)
	FURGMatchDelegate OnFoundURG;

	UPROPERTY(BlueprintAssignable)
	FURGMatchScoreDelegate OnFindingScoreChange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowDebugMessage = false;

	AURGAnchorManager();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/**
	 * Connect to all URGs
	 * @return		Number of connected URGs
	 */
	UFUNCTION(BlueprintCallable)
	int ConnectAllURGs();

	UFUNCTION(BlueprintCallable)
	void DisconnectAllURGs();

	UFUNCTION(BlueprintCallable)
	bool StartToFindURGForAnchorIndex(int AnchorIndex);

	UFUNCTION(BlueprintCallable)
	bool StartToFindURGForAnchor(AActor* Anchor);

	UFUNCTION(BlueprintCallable)
	void StopFindingURG();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locating")
	float RequiredScore = 50.0f;

	/**
	 * Scan points who're within the acceptable distance (in CM) will be calculated whlie finding URG
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locating")
	float AcceptableDistance = 100.0f;

	/**
	 * Scan points who're within the accpetable distance and 'move' over the threshold (in CM) from the last frame will be calculated
	 * If ValidThreshold is less or equals to 0 means the AnchorManager won't filter out static points like wall, ground
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locating")
	float ValidThreshold = 5.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> Anchors;

	UPROPERTY(BlueprintReadOnly)
	TArray<AURGConnector*> URGs;

	AActor* LocatingAnchor;

	TMap<AURGConnector*, TArray<FURGData>> LastFrameData;
	TMap<AURGConnector*, float> ScanProgress;

	float GetURGScore(AURGConnector* URG);

	float GetLengthScore(float Length);

};

