// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyGI.h"
#include "Engine/Classes/Components/StaticMeshComponent.h"
#include "Engine/Classes/Components/InstancedStaticMeshComponent.h"
#include "TerrainChunk.generated.h"

UCLASS()
class PTRPG_API ATerrainChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATerrainChunk();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seed")
		int customSeed = 0; //Using this to change seed inside BP

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerrainSize")
		int NumberMeshesX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerrainSize")
		int NumberMeshesY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerrainSize")
		int NumberMeshesZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerrainSize")
		int Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerrainSize")
		int DistanceHeight;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Biome")
		int biome;

	UMyGI* GI;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		class UInstancedStaticMeshComponent* Grass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		class UInstancedStaticMeshComponent* Sand;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		class UInstancedStaticMeshComponent* Sand2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		class UInstancedStaticMeshComponent* Stone;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		class UInstancedStaticMeshComponent* Snow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
		class UInstancedStaticMeshComponent* Water;

	TArray<UActorComponent*> Comps;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite)
	FVector playerLoc;



	int renderDistance;






	UPROPERTY(EditAnywhere, Category = "WaterLevel")
		TSubclassOf<AActor> WaterCollider;

	//UPROPERTY(EditAnywhere, Category = "TerrainSpawner")
	//	TSubclassOf<ATerrain> TerrainGenerator;

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

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite)
	int CoordX;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite)
	int CoordY;

	int N;
	int E;
	int W;
	int S;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite)
	FVector InitChunk;

	FRandomStream seed;	//Game seed!

	
	TArray<UInstancedStaticMeshComponent*> AllCubes;

	UFUNCTION(BluePrintCallable)
		void Spawn(int X, int Y, FVector SpawnLocation, FRotator SpawnRotation);

	UFUNCTION(BluePrintCallable)
		void SpawnDesert(int X, int Y, FVector SpawnLocation, FRotator SpawnRotation);

	UFUNCTION(BluePrintCallable)
		void SpawnTundra(int X, int Y, FVector SpawnLocation, FRotator SpawnRotation);

	UFUNCTION(BluePrintCallable)
		void SpawnIslands(int X, int Y, FVector SpawnLocation, FRotator SpawnRotation);

	UFUNCTION(BluePrintCallable)
		void GenerateChunk();

	UInstancedStaticMeshComponent* CubeInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
