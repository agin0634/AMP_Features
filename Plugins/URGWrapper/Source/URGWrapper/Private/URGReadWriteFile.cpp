// Fill out your copyright notice in the Description page of Project Settings.


#include "URGReadWriteFile.h"

bool UURGReadWriteFile::LoadTxtToArray(FString FileName, TArray<FString>& SaveText)
{
	if (!*(FPaths::ProjectSavedDir() + FileName))
	{
		return false;
	}

	return FFileHelper::LoadFileToStringArray(SaveText, *(FPaths::ProjectSavedDir() + FileName));
}

bool UURGReadWriteFile::SaveArrayToTxt(TArray<FString> SaveText, FString FileName)
{
	return FFileHelper::SaveStringArrayToFile(SaveText, *(FPaths::ProjectSavedDir() + FileName));
}

bool UURGReadWriteFile::SplitURGData(TArray<FString> &SaveText, TArray<FString>& Data_Anchors, TArray<FString>& Data_URGs, FString FileName)
{
	if (LoadTxtToArray(FileName, SaveText))
	{
		// Split SaveData to Anchor and URG data
		for (int32 i = 0; i < SaveText.Num(); i++)
		{
			FString left, right;
			if (SaveText[i].Split(TEXT(":"), &left, &right))
			{
				Data_Anchors.Add(left);
				Data_URGs.Add(right);
			}
		}
		return true;
	}
	return false;
}

void UURGReadWriteFile::ReadPositionData(TArray<AActor*> Anchors, TArray<AURGConnector*> URGs)
{
	TArray<FString> SaveData;
	TArray<FString> Data_Anchor;
	TArray<FString> Data_URG;
	TArray<AActor*> Anchors_tmp = Anchors;
	TArray<AURGConnector*> URGs_tmp = URGs;

	if (SplitURGData(SaveData, Data_Anchor, Data_URG, "URGPositionData.txt")) 
	{
		for (int i = 0; i < URGs_tmp.Num(); i++)
		{
			int32 number = -1;

			// Search URG number in SaveData
			for (int j = 0; j < Data_URG.Num(); j++)
			{
				if (UKismetSystemLibrary::GetDisplayName(URGs_tmp[i]) == Data_URG[j])
				{
					number = j;
					break;
				}
			}

			if (number != -1)
			{
				bool bIsAnchorDataValid = false;

				// Check if Anchor is valid or not
				// If Valid, set URG to Anchor's transform
				for (int k = 0; k < Anchors_tmp.Num(); k++)
				{
					if (Data_Anchor[number] == UKismetSystemLibrary::GetDisplayName(Anchors_tmp[k]))
					{
						URGs_tmp[i]->SetActorTransform(Anchors_tmp[k]->GetActorTransform());
						bIsAnchorDataValid = true;
					}
				}

				if (!bIsAnchorDataValid)
				{
					UE_LOG(LogTemp, Warning, TEXT("%s -> No Anchor data found"), *UKismetSystemLibrary::GetDisplayName(URGs_tmp[i]));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("%s -> No URG data found"), *UKismetSystemLibrary::GetDisplayName(URGs_tmp[i]));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Position data file found"));
	}
}

void UURGReadWriteFile::WritePositionData(AActor* Anchor, AURGConnector* URG)
{
	TArray<FString> SaveData;
	TArray<FString> Data_Anchor;
	TArray<FString> Data_URG;
	FString CurrentAnchorName = UKismetSystemLibrary::GetDisplayName(Anchor);
	FString CurrentURGName = UKismetSystemLibrary::GetDisplayName(URG);

	if (SplitURGData(SaveData, Data_Anchor, Data_URG, "URGPositionData.txt"))
	{
		bool bIsURGDataFound = false;

		for (int32 i = 0; i < Data_URG.Num(); i++)
		{
			if (CurrentURGName == Data_URG[i]) 
			{
				SaveData[i] = CurrentAnchorName + ":" + CurrentURGName;
				bIsURGDataFound = true;
				break;
			}
		}

		if (!bIsURGDataFound)
		{
			SaveData.Add(CurrentAnchorName + ":" + CurrentURGName);
		}

	}
	else
	{
		SaveData.Add(CurrentAnchorName + ":" + CurrentURGName);
	}

	SaveArrayToTxt(SaveData, "URGPositionData.txt");
}




