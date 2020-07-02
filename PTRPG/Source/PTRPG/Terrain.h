// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	UPROPERTY(EditAnywhere, Category = "WaterLevel")
		TSubclassOf<AActor> WaterCollider;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Instance")
		UInstancedStaticMeshComponent* CubeInstance;

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


	FVector ActorVect;

	UMaterialInterface* SandMat;
	UMaterialInterface* StoneMat;
	UMaterialInterface* SnowMat;
	UMaterialInterface* GrassMat;

	TArray<FVector> CubeLocations;

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
