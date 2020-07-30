// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerrainChunk.h"
#include "MyGI.h"
#include "Engine/Classes/Components/StaticMeshComponent.h"
#include "Engine/Classes/Components/InstancedStaticMeshComponent.h"
#include "Terrain.generated.h"

class UMaterialInstanceDynamic;

UCLASS()
class PTRPG_API ATerrain : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATerrain();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seed")
		int customSeed = 0; //Using this to change seed inside BP

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerrainSize")
		int NumberMeshesX = 64;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerrainSize")
		int NumberMeshesY = 64;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerrainSize")
		int NumberMeshesZ = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerrainSize")
		int Distance = 250;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerrainSize")
		int GenerationDistance;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Biome")
	int biome;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Render")
		int maxSpawns = 7;

	int offSet;
	int offSetHeight;
	int offSetZoom;

	float perlinVAL;
	float waterLevel;

	int CoordX;
	int CoordY;

	int prevX;
	int prevY;

	int ChunkDistance;

	bool bChunk = false;

	int renderDistance;

	UPROPERTY(EditAnywhere, Category = "DeathZone")
		TSubclassOf<AActor> DeathZone;

	UPROPERTY(VisibleAnywhere, Category = "TerrainChunk")
		TSubclassOf<ATerrainChunk> Chunk;
	
		ATerrainChunk* SpawnedChunk;

		UMyGI* GI;

		TArray<UInstancedStaticMeshComponent*> ChunkInstance;



	FRandomStream seed;	//Game seed!

	FVector MyChar;

	FVector InitChunk;

	FVector ChunkDist;

	TArray<FVector> ChunkSpawns;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BluePrintCallable)
		void GenerateChunk(FVector ChunkLocation, int X, int Y);

};
