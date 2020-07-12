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


		int customSeed; 
	
		int NumberMeshesX;
		int NumberMeshesY;
		int NumberMeshesZ;

		int CoordX;
		int CoordY;

		TArray<int> allX;
		TArray<int> allY;

		int offSet;
		int offSetHeight;
		int offSetZoom;

		float waterLevel;

		//Distance before we delete  chunks
		int renderDistance;

		int Distance;

		int biome;
		
		//SpawnLocation for Chunks
		FVector InitChunk;

		TArray<FVector> SpawnedChunks;

		//Where is player
		FVector Player;

};
