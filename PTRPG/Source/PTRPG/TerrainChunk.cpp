// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainChunk.h"
#include "Engine.h"

// Sets default values
ATerrainChunk::ATerrainChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Grass = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Grass"));
	Grass->SetupAttachment(RootComponent);
	Grass->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
	//Grass->RegisterComponent();


	Sand = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Sand"));
	Sand->SetupAttachment(Grass);
	Sand->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
	//Sand->RegisterComponent();

	Sand2 = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Sand2"));
	Sand2->SetupAttachment(Sand);
	Sand2->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);

	//Sand2->RegisterComponent();

	Stone = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Stone"));
	Stone->SetupAttachment(Sand2);
	Stone->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);

	//Stone->RegisterComponent();

	Snow = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Snow"));
	Snow->SetupAttachment(Stone);
	Snow->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);

	//Snow->RegisterComponent();

	Water = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Water"));
	Water->SetupAttachment(Snow);
	Water->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);

	//Water->RegisterComponent();
	
}


// Called when the game starts or when spawned
void ATerrainChunk::BeginPlay()
{
	Super::BeginPlay();

	GI = Cast<UMyGI>(GetGameInstance());

	customSeed = GI->customSeed;
	NumberMeshesX = GI->NumberMeshesX;
	NumberMeshesY = GI->NumberMeshesY;
	NumberMeshesZ = GI->NumberMeshesZ;

	CoordX = GI->CoordX;
	CoordY = GI->CoordY;

	offSet = GI->offSet;
	offSetHeight = GI->offSetHeight;
	offSetZoom = GI->offSetZoom;

	renderDistance = GI->renderDistance;
	Distance = GI->Distance;

	biome = GI->biome;

	InitChunk = GI->InitChunk;

	waterLevel = GI->waterLevel;

	enemySpawnRate = FMath::RandRange(2200, 4000);
	
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("Spawn rate %i"), enemySpawnRate));

	Comps = GetComponentsByClass(UInstancedStaticMeshComponent::StaticClass());

	this->GetComponents(Comps);

	TArray<UActorComponent*> CompTest = GetComponentsByClass(UInstancedStaticMeshComponent::StaticClass());

	for (auto v : CompTest) {

		CubeInstance = Cast<UInstancedStaticMeshComponent>(v);

		if (CubeInstance != nullptr || v != NULL) {

			AllCubes.Add(CubeInstance);

		}

	}

	seed.Initialize(customSeed);


	if (biome == 0) {
		biome = seed.RandRange(1, 4);
	}

	switch (biome) {
	case 1://Desert
		for (int i = 0; i < NumberMeshesX; i++)//X number of Meshes to spawn
		{
			for (int j = 0; j < NumberMeshesY; j++)//Y number of Mesh to spawn
			{
				for (int k = 0; k < NumberMeshesZ; k++) {

					cubeCount++;

					//Spawning location for our tile.
					FVector TileLocation = FVector(InitChunk.X + (i * Distance), InitChunk.Y + (j * Distance), InitChunk.Z + (k * Distance));

					perlinVAL = FMath::PerlinNoise3D(FVector(TileLocation.X, TileLocation.Y, TileLocation.Z + offSet) / offSetZoom);


					//GetWorld().

					SpawnDesert(CoordX, CoordY, TileLocation, FRotator::ZeroRotator);
				}
			}
		}
		break;
	case 2://Tundra
		for (int i = 0; i < NumberMeshesX; i++)//X number of Meshes to spawn
		{
			for (int j = 0; j < NumberMeshesY; j++)//Y number of Mesh to spawn
			{
				for (int k = 0; k < NumberMeshesZ; k++) {
					cubeCount++;

					//Spawning location for our tile.
					FVector TileLocation = FVector(InitChunk.X + (i * Distance), InitChunk.Y + (j * Distance), InitChunk.Z + (k * Distance));

					perlinVAL = FMath::PerlinNoise3D(FVector(TileLocation.X, TileLocation.Y, TileLocation.Z + offSet) / offSetZoom);



					SpawnTundra(CoordX, CoordY, TileLocation, FRotator::ZeroRotator);
				}
			}
		}
		break;
	case 3://Plains
		for (int i = 0; i < NumberMeshesX; i++)//X number of Meshes to spawn
		{
			for (int j = 0; j < NumberMeshesY; j++)//Y number of Mesh to spawn
			{
				for (int k = 0; k < NumberMeshesZ; k++) {
					cubeCount++;
					//Spawning location for our tile.
					FVector TileLocation = FVector(InitChunk.X + (i * Distance), InitChunk.Y + (j * Distance), InitChunk.Z + (k * Distance));

					perlinVAL = FMath::PerlinNoise3D(FVector(TileLocation.X, TileLocation.Y, TileLocation.Z + offSet) / offSetZoom);

					Spawn(CoordX, CoordY, TileLocation, FRotator::ZeroRotator);
				}
			}
		}
		break;
	case 4://Islands

		for (int i = 0; i < NumberMeshesX; i++)//X number of Meshes to spawn
		{
			for (int j = 0; j < NumberMeshesY; j++)//Y number of Mesh to spawn
			{
				for (int k = 0; k < NumberMeshesZ; k++) {
					cubeCount++;
					//Spawning location for our tile.

					FVector TileLocation = FVector(InitChunk.X + (i * Distance), InitChunk.Y + (j * Distance), InitChunk.Z + (k * Distance));

					perlinVAL = FMath::PerlinNoise3D(FVector(TileLocation.X, TileLocation.Y, TileLocation.Z + offSet) / offSetZoom);

					SpawnIslands(CoordX, CoordY, TileLocation, FRotator::ZeroRotator);
				}
			}
		}
		break;
	}

}

// Called every frame
void ATerrainChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	playerLoc = GI->Player;

	int k;
	if (playerLoc.X > (InitChunk.X + renderDistance + 500) || playerLoc.X < (InitChunk.X - renderDistance - 500) || playerLoc.Y > (InitChunk.Y + renderDistance + 500) || playerLoc.Y < (InitChunk.Y - renderDistance - 500)) {
		int i = 0;
		for (FVector spawnLoc : GI->SpawnedChunks) {
			
			if (spawnLoc == InitChunk) {
				k = i;
				break;
			}
			i++;
		}

		GI->SpawnedChunks.RemoveAt(k);
		Destroy();
	}
}

void ATerrainChunk::Spawn(int X, int Y, FVector SpawnLocation, FRotator SpawnRotation)
{
	if (perlinVAL <= waterLevel) {//Water
		AllCubes[5]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + 125 + (offSetHeight * waterLevel))));
	}

	if (cubeCount % enemySpawnRate < 1) {
		
		AMyEnemy* SpawnedEnemy = Cast<AMyEnemy>(GetWorld()->SpawnActor(Enemy.Get()));

		SpawnedEnemy->SetActorLocation(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + 700 + (offSetHeight * perlinVAL)));
			
	}

	if (perlinVAL <= -0.3f) {//Sand
		sandCount++;

		if (sandCount % 47 <= 1) {
			AllCubes[2]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
		else {
			AllCubes[1]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}

	}
	else if (perlinVAL > -0.3 && perlinVAL <= -0.1) {//Sand 2
		sandCount2++;

		if (sandCount2 % 22 <= 1) {
			AllCubes[1]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
		else {
			AllCubes[2]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
	}
	else if (perlinVAL > -0.1 && perlinVAL <= 0.3) {//Grass
	
		grassCount++;

		if (grassCount % 725 <= 2) {
			AllCubes[3]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
		else {
			AllCubes[0]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
	}
	else if (perlinVAL > 0.3 && perlinVAL <= 0.6) {//Stone
		stoneCount++;

		if (stoneCount % 600 <= 2) {
			AllCubes[4]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
		else {
			AllCubes[3]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}

	}
	else {//Snow

		snowCount++;

		if (snowCount % 75 <= 1) {
			AllCubes[3]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
		else {
			AllCubes[4]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
;
	}

}

void ATerrainChunk::SpawnDesert(int X, int Y, FVector SpawnLocation, FRotator SpawnRotation)
{
	if (perlinVAL <= waterLevel) {//Water
		AllCubes[5]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + 125 + (offSetHeight * waterLevel))));
	}

	if (cubeCount % enemySpawnRate == 0) {
		
		AMyEnemy* SpawnedEnemy = Cast<AMyEnemy>(GetWorld()->SpawnActor(Enemy.Get()));

		SpawnedEnemy->SetActorLocation(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + 700 + (offSetHeight * perlinVAL)));
	}

	if (perlinVAL >= (waterLevel - 0.04) && perlinVAL <= (waterLevel + 0.01)) {//Grass   (Oasis)
		AllCubes[0]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
	}
	else if (perlinVAL > (waterLevel + 0.02) && perlinVAL <= -0.15f) {//Sand
		sandCount++;

		if (sandCount % 59 < 1) {
			AllCubes[2]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
		else {
			AllCubes[1]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
	}
	else if (perlinVAL > -0.15f && perlinVAL <= 0.3f) {//Sand 2
		sandCount2++;

		if (sandCount2 % 181 < 1) {
			AllCubes[1]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
		else {
			AllCubes[2]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
	}
	else {
		sandCount++;

		if (sandCount % 111 < 1) {
			AllCubes[2]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
		else {
			AllCubes[1]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
	}

}

void ATerrainChunk::SpawnTundra(int X, int Y, FVector SpawnLocation, FRotator SpawnRotation)
{
	if (perlinVAL <= waterLevel) {//Water

		AllCubes[5]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + 125 + (offSetHeight * waterLevel))));
	}

	if (cubeCount % enemySpawnRate == 0) {
		
		AMyEnemy* SpawnedEnemy = Cast<AMyEnemy>(GetWorld()->SpawnActor(Enemy.Get()));

		SpawnedEnemy->SetActorLocation(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + 700 + (offSetHeight * perlinVAL)));

	}

	if (perlinVAL >= -0.5f && perlinVAL <= -0.2f) {//Grass 66% Stone 34%

		grassCount++;

		if (grassCount % 37 < 1) {
			AllCubes[0]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
		else {
			AllCubes[3]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
	}
	else if (perlinVAL > -0.2f && perlinVAL <= 0.1f) {//Stone 66% Grass 33%

		stoneCount++;

		if (stoneCount % 173 < 1) {
			AllCubes[0]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
		else {
			AllCubes[3]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}

	}
	else if (perlinVAL > 0.1f && perlinVAL <= 0.3f) {//Snow, Stones

		snowCount++;

		if (snowCount % 37 < 1) {
			AllCubes[4]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
		else {
			AllCubes[3]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
	}
	else if (perlinVAL > 0.3f && perlinVAL <= 0.45f) {//Stone

		stoneCount++;

		if (stoneCount % 13 < 1) {
			AllCubes[4]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
		else {
			AllCubes[3]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
	}
	else if (perlinVAL > 0.45f) {//Snow

		snowCount++;

		if (snowCount % 113 < 1) {
			AllCubes[3]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
		else {
			AllCubes[4]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}

	}
	else {//Stone, Grass
		stoneCount++;

		if (stoneCount % 7 <= 2) {
			AllCubes[0]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
		else {
			AllCubes[3]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
	}


}

void ATerrainChunk::SpawnIslands(int X, int Y, FVector SpawnLocation, FRotator SpawnRotation)
{
	if (perlinVAL <= waterLevel) {//Water
		AllCubes[5]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + 125 + (offSetHeight * waterLevel))));

	}

	if (cubeCount % enemySpawnRate == 0) {
		
		AMyEnemy* SpawnedEnemy = Cast<AMyEnemy>(GetWorld()->SpawnActor(Enemy.Get()));


		SpawnedEnemy->SetActorLocation(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + 700 + (offSetHeight * perlinVAL)));
	}

	if (perlinVAL <= 0.1f) {//Sand
		sandCount++;

		if (sandCount % 97 <= 1) {
			AllCubes[2]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
		else {
			AllCubes[1]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
	}
	else if (perlinVAL > 0.1 && perlinVAL <= 0.4) {//Sand 2

		sandCount2++;

		if (sandCount2 % 67 <= 1) {
			AllCubes[1]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
		else {
			AllCubes[2]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
	}
	else if (perlinVAL > 0.4 && perlinVAL <= 0.6) {//Grass

		grassCount++;

		if (grassCount % 725 <= 2) {
			AllCubes[3]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
		else {
			AllCubes[0]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}
	}
	else if (perlinVAL > 0.6) {//Stone

		stoneCount++;

		if (stoneCount % 600 <= 2) {
			
			AllCubes[0]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));


		}
		else {
			AllCubes[3]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

		}

	}
}