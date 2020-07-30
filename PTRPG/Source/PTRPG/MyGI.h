// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGI.generated.h"

/**
 * 
 */
UCLASS()
class PTRPG_API UMyGI : public UGameInstance
{
	GENERATED_BODY()

public:

		UPROPERTY(EditAnywhere, BluePrintReadWrite)
		int customSeed;

		UPROPERTY(EditAnywhere, BluePrintReadWrite)
			int playerLevel;
	
		int NumberMeshesX;
		int NumberMeshesY;
		int NumberMeshesZ;

		int CoordX;
		int CoordY;

		int offSet;
		int offSetHeight;
		int offSetZoom;

		float waterLevel;

		//Distance before we delete  chunks
		int renderDistance;

		int Distance;

		UPROPERTY(EditAnywhere, BluePrintReadWrite)
		int biome;
		
		//SpawnLocation for Chunks
		FVector InitChunk;

		TArray<FVector> SpawnedChunks;

		//Where is player
		FVector Player;

};
