// Fill out your copyright notice in the Description page of Project Settings.


#include "Terrain.h"
#include <Engine.h>

// Sets default values
ATerrain::ATerrain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

}

// Called when the game starts or when spawned
void ATerrain::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> Comps = GetComponentsByClass(UInstancedStaticMeshComponent::StaticClass());

	//Get Player Location
	MyChar = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	//Distance per chunk
	ChunkDistance = NumberMeshesX * Distance;

	//Distance where chunks will despawn
	renderDistance = ChunkDistance * maxSpawns;

	//Save Chunk Distance
	ChunkDist = FVector(MyChar.X + ChunkDistance, MyChar.Y + ChunkDistance, MyChar.Z);

	//Center first chunk
	InitChunk = FVector(MyChar.X - (ChunkDist.X / 2), MyChar.Y - (ChunkDist.Y / 2), MyChar.Z - 6000);

	GI = Cast<UMyGI>(GetGameInstance());

	FRotator TileRotation = FRotator::ZeroRotator;

	if (customSeed != 0) {
		seed.Initialize(customSeed);
	}
	else {
		customSeed = FMath::RandRange(0, 50000);
		seed.Initialize(customSeed);
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("New Seed : %i"), customSeed));
	}
	
	
	offSet = seed.RandRange(250, 100000);
	offSetHeight = seed.RandRange(3250,8000);//How high blocks can go 700-7000
	offSetZoom = seed.RandRange(10000, 20000);//How much hills should we have, lower numbers = more bumps. 7000-12000 desert.
	waterLevel = seed.FRandRange(-0.6, -0.15);
	biome = GI->biome;

	if (biome <= 0 || biome > 4) {
		biome = seed.RandRange(1, 4);
	}

	switch (biome) {
	case 1://Desert
		offSetZoom += 5000;
		offSetHeight += 1000;
		waterLevel = waterLevel - 0.25f;

		GetWorld()->SpawnActor<AActor>(DeathZone, FVector(InitChunk.X, InitChunk.Y, InitChunk.Z - 6000), FRotator::ZeroRotator);

		break;
	case 2://Tundra
		offSetHeight += 4000;
		offSetZoom += 10000;
		waterLevel = seed.FRandRange(-0.6, -0.35);
		GetWorld()->SpawnActor<AActor>(DeathZone, FVector(InitChunk.X, InitChunk.Y, InitChunk.Z - 6000), FRotator::ZeroRotator);

		break;
	case 3://Normal
		GetWorld()->SpawnActor<AActor>(DeathZone, FVector(InitChunk.X, InitChunk.Y, InitChunk.Z - 6000), FRotator::ZeroRotator);

		break;
	case 4://Islands
		waterLevel = waterLevel + 0.5f;

		GetWorld()->SpawnActor<AActor>(DeathZone, FVector(InitChunk.X, InitChunk.Y, InitChunk.Z - 6000), FRotator::ZeroRotator);
	
		break;
	}

	//Spawn initial Chunks.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//NE
	GenerateChunk(FVector(InitChunk.X + (1 * ChunkDistance), InitChunk.Y + (1 * ChunkDistance), InitChunk.Z), 1, 1);
	//E
	GenerateChunk(FVector(InitChunk.X + (0 * ChunkDistance), InitChunk.Y + (1 * ChunkDistance), InitChunk.Z), 0, 1);
	//SE
	GenerateChunk(FVector(InitChunk.X + (-1 * ChunkDistance), InitChunk.Y + (1 * ChunkDistance), InitChunk.Z), -1, 1);
	//N
	GenerateChunk(FVector(InitChunk.X + (1 * ChunkDistance), InitChunk.Y + (0 * ChunkDistance), InitChunk.Z), 1, 0);
	//MIDDLE
	GenerateChunk(FVector(InitChunk.X, InitChunk.Y, InitChunk.Z), 0, 0);
	//S
	GenerateChunk(FVector(InitChunk.X + (-1 * ChunkDistance), InitChunk.Y + (0 * ChunkDistance), InitChunk.Z), -1, 0);
	//NW
	GenerateChunk(FVector(InitChunk.X + (1 * ChunkDistance), InitChunk.Y + (-1 * ChunkDistance), InitChunk.Z), 1, -1);
	//W
	GenerateChunk(FVector(InitChunk.X + (0 * ChunkDistance), InitChunk.Y + (-1 * ChunkDistance), InitChunk.Z), 0, -1);
	//SW
	GenerateChunk(FVector(InitChunk.X + (-1 * ChunkDistance), InitChunk.Y + (-1 * ChunkDistance), InitChunk.Z), -1, -1);

	GenerateChunk(FVector(InitChunk.X + (2 * ChunkDistance), InitChunk.Y + (-2 * ChunkDistance), InitChunk.Z), 2, -2);
	GenerateChunk(FVector(InitChunk.X + (1 * ChunkDistance), InitChunk.Y + (-2 * ChunkDistance), InitChunk.Z), 1, -2);
	GenerateChunk(FVector(InitChunk.X + (0 * ChunkDistance), InitChunk.Y + (-2 * ChunkDistance), InitChunk.Z), 0, -2);
	GenerateChunk(FVector(InitChunk.X + (-1 * ChunkDistance), InitChunk.Y + (-2 * ChunkDistance), InitChunk.Z), -1, -2);
	GenerateChunk(FVector(InitChunk.X + (-2* ChunkDistance), InitChunk.Y + (-2 * ChunkDistance), InitChunk.Z), -2, -2);

	GenerateChunk(FVector(InitChunk.X + (-2 * ChunkDistance), InitChunk.Y + (-1 * ChunkDistance), InitChunk.Z), -2, -1);
	GenerateChunk(FVector(InitChunk.X + (-2 * ChunkDistance), InitChunk.Y + (0 * ChunkDistance), InitChunk.Z), -2, 0);
	GenerateChunk(FVector(InitChunk.X + (-2 * ChunkDistance), InitChunk.Y + (1 * ChunkDistance), InitChunk.Z), -2, 1);
	GenerateChunk(FVector(InitChunk.X + (-2 * ChunkDistance), InitChunk.Y + (2 * ChunkDistance), InitChunk.Z), -2, 2);

	GenerateChunk(FVector(InitChunk.X + (-1 * ChunkDistance), InitChunk.Y + (2 * ChunkDistance), InitChunk.Z), -1, 2);
	GenerateChunk(FVector(InitChunk.X + (0 * ChunkDistance), InitChunk.Y + (2 * ChunkDistance), InitChunk.Z), 0, 2);
	GenerateChunk(FVector(InitChunk.X + (1 * ChunkDistance), InitChunk.Y + (2 * ChunkDistance), InitChunk.Z), 1, 2);
	GenerateChunk(FVector(InitChunk.X + (2 * ChunkDistance), InitChunk.Y + (2 * ChunkDistance), InitChunk.Z), 2, 2);

	GenerateChunk(FVector(InitChunk.X + (2 * ChunkDistance), InitChunk.Y + (1 * ChunkDistance), InitChunk.Z), 2, 1);
	GenerateChunk(FVector(InitChunk.X + (2 * ChunkDistance), InitChunk.Y + (0 * ChunkDistance), InitChunk.Z), 2, 0);
	GenerateChunk(FVector(InitChunk.X + (2 * ChunkDistance), InitChunk.Y + (-1 * ChunkDistance), InitChunk.Z), 2, -1);
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	prevX = CoordX;
	prevY = CoordY;
}

// Called every frame
void ATerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MyChar = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	
	GI->Player = MyChar;

	CoordX = (MyChar.X / ChunkDistance);
	CoordY = (MyChar.Y / ChunkDistance);

	//Manage Chunks, won't add duplicate chunks
	if (prevX < CoordX) {
		GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Emerald, FString::Printf(TEXT("Changed X ")));

		for (int i = -2; i < 3; i++) {
			for (FVector spawns : GI->SpawnedChunks) {				
				if (FVector(InitChunk.X + (CoordX * ChunkDistance) + (ChunkDistance * 2), InitChunk.Y + (CoordY * ChunkDistance) + (i * ChunkDistance), InitChunk.Z) == spawns) {
					bChunk = true;
				}

			}

			if (bChunk == false) {
				GenerateChunk(FVector(InitChunk.X + (CoordX * ChunkDistance) + (ChunkDistance * 2), InitChunk.Y + (CoordY * ChunkDistance) + (i * ChunkDistance), InitChunk.Z), CoordX, CoordY + i);
			}
			bChunk = false;
		}	
		prevX = CoordX;
	}

	if (prevY < CoordY) {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Orange, FString::Printf(TEXT("Changed Y ")));
		for (int i = -2; i < 3; i++) {
			for (FVector spawns : GI->SpawnedChunks) {

				if (FVector(InitChunk.X + (CoordX * ChunkDistance) + (i * ChunkDistance), InitChunk.Y + (CoordY * ChunkDistance) + (ChunkDistance * 2), InitChunk.Z) == spawns) {
					bChunk = true;
				}

			}

			if (bChunk == false) {
				GenerateChunk(FVector(InitChunk.X + (CoordX * ChunkDistance) + (i * ChunkDistance), InitChunk.Y + (CoordY * ChunkDistance) + (ChunkDistance * 2), InitChunk.Z), CoordX + i, CoordY);
			}

			bChunk = false;
		}
		prevY = CoordY;
	}

	if (prevX > CoordX) {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Emerald, FString::Printf(TEXT("Changed X ")));
		for (int i = -2; i < 3; i++) {
			for (FVector spawns : GI->SpawnedChunks) {

				//XY

				//00 01 02 03 04 05
				//10 11 12 13 14 15
				//20 21 22 23 24 25
				//30 31 [32] 33 34 35
				//40 41 42 43 44 45
				//50 51 52 53 54 55
				if (FVector(InitChunk.X + (CoordX * ChunkDistance) - (ChunkDistance * 2), InitChunk.Y + (CoordY * ChunkDistance) + (i * ChunkDistance), InitChunk.Z) == spawns) {
					bChunk = true;
				}

			}
			if (bChunk == false) {
				GenerateChunk(FVector(InitChunk.X + (CoordX * ChunkDistance) - (ChunkDistance * 2), InitChunk.Y + (CoordY * ChunkDistance) + (i * ChunkDistance), InitChunk.Z), CoordX, CoordY + i);
			}

			bChunk = false;
		}
		prevX = CoordX;
	}

	if (prevY > CoordY) {
		GEngine->AddOnScreenDebugMessage(-1, 25, FColor::Orange, FString::Printf(TEXT("Changed Y ")));
		for (int i = -2; i < 3; i++) {
			for (FVector spawns : GI->SpawnedChunks) {
				if (FVector(InitChunk.X + (CoordX * ChunkDistance) + (i * ChunkDistance), InitChunk.Y + (CoordY * ChunkDistance) - (ChunkDistance * 2), InitChunk.Z) == spawns) {
					bChunk = true;
				}
			}
			if (bChunk == false) {
				GenerateChunk(FVector(InitChunk.X + (CoordX * ChunkDistance) + (i * ChunkDistance), InitChunk.Y + (CoordY * ChunkDistance) - (ChunkDistance * 2), InitChunk.Z), CoordX + i, CoordY);
			}
			bChunk = false;
		}
		prevY = CoordY;
	}
}

void ATerrain::GenerateChunk(FVector ChunkLocation, int X, int Y)
{
	//Default is good
	FActorSpawnParameters spawnParams;

	GI->customSeed = customSeed;
	GI->NumberMeshesX = NumberMeshesX;
	GI->NumberMeshesY = NumberMeshesY;
	GI->NumberMeshesZ = NumberMeshesZ;
	GI->Distance = Distance;
	GI->renderDistance = renderDistance;
	GI->biome = biome;
	GI->waterLevel = waterLevel;
	GI->offSet = offSet;
	GI->offSetHeight = offSetHeight;
	GI->offSetZoom = offSetZoom;
	GI->CoordX = X;
	GI->CoordY = Y;
	GI->InitChunk = ChunkLocation;

	GI->SpawnedChunks.Add(ChunkLocation);

	//We have the subobject but components spawn without any static mesh or material. Use this to get blueprint values.
	SpawnedChunk = Cast<ATerrainChunk>(GetWorld()->SpawnActor(Chunk.Get()));

	//THIS WORKED
}


