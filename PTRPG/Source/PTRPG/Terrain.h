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
		int DistanceHeight;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Biome")
	int biome;

	UPROPERTY(EditAnywhere, Category = "WaterLevel")
		TSubclassOf<AActor> WaterCollider;

	UPROPERTY(VisibleAnywhere, Category = "TerrainChunk")
		TSubclassOf<ATerrainChunk> Chunk;
	
		ATerrainChunk* SpawnedChunk;

		ATerrainChunk* FinalChunk;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Instance")
		UInstancedStaticMeshComponent* CubeInstance;

		UMyGI* GI;

		TArray<UInstancedStaticMeshComponent*> ChunkInstance;



		
	//UInstancedStaticMeshComponent* CubeInstance = Cast<UInstancedStaticMeshComponent>(v);


	FRandomStream seed;	//Game seed!

	FVector TestVect;

	bool spawned = false;

	int offSet;
	int offSetHeight;
	int offSetZoom;

	float perlinVAL;
	float waterLevel;

	int cubeCount;
	int sandCount;
	int sandCount2;
	int grassCount;
	int stoneCount;
	int snowCount;
	int waterCount;

	int CoordX;
	int CoordY;

	int prevX;
	int prevY;

	int ChunkDistance;

	int N;
	int E;
	int W;
	int S;

	bool bChunk = false;

	int renderDistance;

	UPROPERTY(EditAnywhere,BluePrintReadWrite, Category = "Render")
	int maxSpawns = 7;

	int chunkCount;

	FVector ActorVect;

	FVector MyChar;

	FVector InitChunk;

	FVector ChunkDist;

	TArray<FVector> ChunkSpawns;

	UMaterialInterface* SandMat;
	UMaterialInterface* StoneMat;
	UMaterialInterface* SnowMat;
	UMaterialInterface* GrassMat;

	TArray<FTransform*> CubeLocations;

	FTransform CubeTransform;

	int chunkId;

	TArray<int> meshInstances;

	//Get X val of chunk
	TMap<int, TMap<int, TArray<int>>> ChunkXMap;

	//Get Y val of chunk, Remove instances.
	TMap<int, TArray<int>> ChunkYMap;

	TMap<int, TArray<int>> ChunkTestMap;

	TArray<int> ChunkValMap;
	

	TArray<UInstancedStaticMeshComponent*> AllCubes;

	FVector MovingActor;

	UMaterialInstanceDynamic* DynamicMat;

	UFUNCTION(BluePrintCallable)
		void Spawn(int X, int Y, FVector SpawnLocation, FRotator SpawnRotation);

	UFUNCTION(BluePrintCallable)
		void SpawnDesert(int X, int Y, FVector SpawnLocation, FRotator SpawnRotation);

	UFUNCTION(BluePrintCallable)
		void SpawnTundra(int X, int Y, FVector SpawnLocation, FRotator SpawnRotation);

	UFUNCTION(BluePrintCallable)
		void SpawnIslands(int X, int Y, FVector SpawnLocation, FRotator SpawnRotation);

	UFUNCTION(BluePrintCallable)
		void GenerateChunk(FVector ChunkLocation, int X, int Y);

	UFUNCTION(BluePrintCallable)
		void DeleteChunk(int x, int y);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
