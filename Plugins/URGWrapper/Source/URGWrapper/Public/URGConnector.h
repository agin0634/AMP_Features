#pragma once

#include "CoreMinimal.h"
#include "Urg_driver.h"
#include "math_utilities.h"
#include "iostream"
#include <thread>
#include "GameFramework/Actor.h"
#include "ScanData.h"
#include "URGScanDataProvider.h"
#include "Core/Public/Async/AsyncWork.h"
#include "URGConnector.generated.h"

class FScanTask;

UCLASS(Blueprintable)
class URGWRAPPER_API AURGConnector : public AActor, public IURGScanDataProvider
{
	GENERATED_BODY()

public:

	friend class FScanTask;

	AURGConnector();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type endplayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Com = "COM3";

	UPROPERTY(EditDefaultsOnly)
	int SkinScan = 1;

	// The real scan angle will be -ScanAngle ~ ScanAngle
	UPROPERTY(EditDefaultsOnly)
	int ScanAngle = 90;

	UPROPERTY(BlueprintReadWrite)
	float Scale = 1.0f;

	UPROPERTY(EditAnywhere)
	float MinDistance;

	UPROPERTY(BlueprintReadOnly)
	bool bDoing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawDebug = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	FColor DebugColor = FColor::Yellow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float DebugSize = 5.0f;

	UFUNCTION(BlueprintCallable)
	void StartScan();

	UFUNCTION(BlueprintCallable)
	void StopScanning();

	// Interface IURGScanDataProvider
	virtual TArray<FURGData> GetScanDataArray_Implementation(bool bFilterInvalid = true) override;

	UFUNCTION(BlueprintCallable, Category="URG")
	static TArray<FString> FindPorts();
	
private:

	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* Root;

	FAutoDeleteAsyncTask<FScanTask>* ScanTask = nullptr;

	TArray<FURGData> ScanDataArray;

};

