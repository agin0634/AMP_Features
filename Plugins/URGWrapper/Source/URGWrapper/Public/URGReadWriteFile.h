// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/Paths.h"
#include "URGConnector.h"
#include "Misc/FileHelper.h"
#include "URGReadWriteFile.generated.h"

/**
 * 
 */
UCLASS()
class URGWRAPPER_API UURGReadWriteFile : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static bool LoadTxtToArray(FString FileName, TArray<FString> & SaveText);

	UFUNCTION(BlueprintCallable)
	static bool SaveArrayToTxt(TArray<FString> SaveText, FString FileName);

	static void ReadPositionData(TArray<AActor*> Anchors, TArray<AURGConnector*> URGs);
	
	static void WritePositionData(AActor* Anchor, AURGConnector* URG);

private:

	static bool SplitURGData(TArray<FString> &SaveText, TArray<FString> &Data_Anchors, TArray<FString> &Data_URGs, FString FileName);

};
