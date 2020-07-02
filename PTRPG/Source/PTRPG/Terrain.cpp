// Fill out your copyright notice in the Description page of Project Settings.


#include "Terrain.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include <Engine.h>

// Sets default values
ATerrain::ATerrain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	

}

// Called when the game starts or when spawned
void ATerrain::BeginPlay()
{
	Super::BeginPlay();


	TArray<UActorComponent*> Comps = GetComponentsByClass(UInstancedStaticMeshComponent::StaticClass());


	//THIS WORKS!!!!!!!
	int p = 0;
	for (auto v : Comps) {
		
		//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, FString::Printf(TEXT("HELLO , FOUND INSTANCE MESH COMPONENT")));
		CubeInstance = Cast<UInstancedStaticMeshComponent>(v);

		AllCubes.Add(CubeInstance);

		//if (CubeInstance != nullptr) {
			//CubeInstance->SetSimulatePhysics(true);
			//CubeInstance->AddInstanceWorldSpace(TestTransform);

			// GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, FString::Printf(TEXT("ADDED INSTANCE")));
		//}

		FString result = v->GetName();
		GEngine->AddOnScreenDebugMessage(-1, 25.0f, FColor::Blue, FString::Printf(TEXT("%i "), p));
		GEngine->AddOnScreenDebugMessage(-1, 25.0f, FColor::Red, result);
		

		p++;


	}

	//Grass 0
	//Sand 1
	//Sand2 2
	//Stone 3
	//Snow 4
	//Water 5

	FRotator TileRotation = FRotator::ZeroRotator;

	if (customSeed != 0) {
		seed.Initialize(customSeed);
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Seed FOUND")));
	}
	else {
		seed.Initialize(FMath::RandRange(0, 50000));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("SEED RANDOMISED")));
	}
	
	//Random between 3000- 15000

	//Desert, plains, tundra, islands.

	

	offSet = seed.RandRange(250, 100000);
	offSetHeight = seed.RandRange(3250,8000);//How high blocks can go 700-7000
	offSetZoom = seed.RandRange(10000, 20000);//How much hills should we have, lower numbers = more bumps. 7000-12000 desert.
	waterLevel = seed.FRandRange(-0.6, -0.15);
	//Desert Height = 3250-7000, offsetzoom 7000-12000,  Islands = height 3800-5300, zoom 7000-15000
	ActorVect = FVector(GetActorLocation());

	MovingActor = ActorVect;

	if (biome == 0) {
		biome = seed.RandRange(1, 4);
	}

	switch (biome) {
	case 1://Desert
		offSetZoom += 5000;
		offSetHeight += 1000;
		waterLevel = waterLevel - 0.25f;

		GetWorld()->SpawnActor<AActor>(WaterCollider, FVector(ActorVect.X, ActorVect.Y, (ActorVect.Z - 300 + (offSetHeight * waterLevel))), FRotator::ZeroRotator); //"Swim" Zone

		for (int i = 0; i < NumberMeshesX; i++)//X number of Meshes to spawn
		{
			for (int j = 0; j < NumberMeshesY; j++)//Y number of Mesh to spawn
			{
				for (int k = 0; k < NumberMeshesZ; k++) {
					//Spawning location for our tile.
					FVector TileLocation = FVector(ActorVect.X + (i * Distance), ActorVect.Y + (j * Distance), ActorVect.Z + (k * Distance));

					CubeLocations.Add(TileLocation);


					perlinVAL = FMath::PerlinNoise3D(FVector(TileLocation.X, TileLocation.Y, TileLocation.Z + offSet) / offSetZoom);
					//if (perlinVAL <= 0.10f) {
					//	perlinVAL = FMath::RandRange(0.09f,0.1f);
					//}

					//Spawning Method
					SpawnDesert(i, j, TileLocation, TileRotation);
				}
			}
		}
		break;
	case 2://Tundra
		offSetHeight += 4000;
		offSetZoom += 10000;
		waterLevel = seed.FRandRange(-0.6, -0.35);

		GetWorld()->SpawnActor<AActor>(WaterCollider, FVector(ActorVect.X, ActorVect.Y, (ActorVect.Z - 300 + (offSetHeight * waterLevel))), FRotator::ZeroRotator); //"Swim" Zone


		for (int i = 0; i < NumberMeshesX; i++)//X number of Meshes to spawn
		{
			for (int j = 0; j < NumberMeshesY; j++)//Y number of Mesh to spawn
			{
				for (int k = 0; k < NumberMeshesZ; k++) {
					//Spawning location for our tile.
					FVector TileLocation = FVector(ActorVect.X + (i * Distance), ActorVect.Y + (j * Distance), ActorVect.Z + (k * Distance));

					CubeLocations.Add(TileLocation);


					perlinVAL = FMath::PerlinNoise3D(FVector(TileLocation.X, TileLocation.Y, TileLocation.Z + offSet) / offSetZoom);
					//if (perlinVAL <= 0.10f) {
					//	perlinVAL = FMath::RandRange(0.09f,0.1f);
					//}

					//Spawning Method
					SpawnTundra(i, j, TileLocation, TileRotation);
				}
			}
		}
		break;
	case 3://Plains

		GetWorld()->SpawnActor<AActor>(WaterCollider, FVector(ActorVect.X, ActorVect.Y, (ActorVect.Z - 300 + (offSetHeight * waterLevel))), FRotator::ZeroRotator); //"Swim" Zone

		for (int i = 0; i < NumberMeshesX; i++)//X number of Meshes to spawn
		{
			for (int j = 0; j < NumberMeshesY; j++)//Y number of Mesh to spawn
			{
				for (int k = 0; k < NumberMeshesZ; k++) {
					//Spawning location for our tile.
					FVector TileLocation = FVector(ActorVect.X + (i * Distance), ActorVect.Y + (j * Distance), ActorVect.Z + (k * Distance));

					CubeLocations.Add(TileLocation);


					perlinVAL = FMath::PerlinNoise3D(FVector(TileLocation.X, TileLocation.Y, TileLocation.Z + offSet) / offSetZoom);
					//if (perlinVAL <= 0.10f) {
					//	perlinVAL = FMath::RandRange(0.09f,0.1f);
					//}

					//Spawning Method
					Spawn(i, j, TileLocation, TileRotation);
				}
			}
		}
		break;
	case 4://Islands
		waterLevel = waterLevel + 0.5f;

		GetWorld()->SpawnActor<AActor>(WaterCollider, FVector(ActorVect.X, ActorVect.Y, (ActorVect.Z - 300 + (offSetHeight* waterLevel))) , FRotator::ZeroRotator); //"Swim" Zone
		
		
		for (int i = 0; i < NumberMeshesX; i++)//X number of Meshes to spawn
		{
			for (int j = 0; j < NumberMeshesY; j++)//Y number of Mesh to spawn
			{
				for (int k = 0; k < NumberMeshesZ; k++) {
					//Spawning location for our tile.
					FVector TileLocation = FVector(ActorVect.X + (i * Distance), ActorVect.Y + (j * Distance), ActorVect.Z + (k * Distance));

					CubeLocations.Add(TileLocation);


					perlinVAL = FMath::PerlinNoise3D(FVector(TileLocation.X, TileLocation.Y, TileLocation.Z + offSet) / offSetZoom);
					//if (perlinVAL <= 0.10f) {
					//	perlinVAL = FMath::RandRange(0.09f,0.1f);
					//}

					//Spawning Method
					SpawnIslands(i, j, TileLocation, TileRotation);
				}
			}
		}
		break;
	}

	//	TArray<UChildActorComponent> children;

		//for each (UChildActorComponent v in ATerrain)
		//{

		//}



		//ATerrain->GetComponents<UChildActorComponent>(children);


		//Perlin can't be changed, therefore we will be using the 3d space to give us different perlin generations.
		//TestVect = FVector2D(26.0f, 25.2);



		//Just Debug Text, showing what seed was chosen.

	 //SandMat = AllCubes[0]->GetMaterial(0);
	 //StoneMat = AllCubes[0]->GetMaterial(1);
	// SnowMat = AllCubes[0]->GetMaterial(2);
	 //GrassMat = AllCubes[0]->GetMaterial(3);




	// AllCubes[1]->SetMaterial(0, SandMat);
	// AllCubes[2]->SetMaterial(0, StoneMat);
	// AllCubes[3]->SetMaterial(0, SnowMat);
	// AllCubes[4]->SetMaterial(0, GrassMat);



	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Initial seed chosen: %i  , but we chose this seed %i , with the offset of : %i , Height of : %i , ZOOM : %i "), seed.GetInitialSeed(), seed.GetCurrentSeed(), offSet, offSetHeight, offSetZoom));

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("BIOME :  %i , Water level : %f "), biome, waterLevel ));


	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Number of cubes instanced! :  %i  "), CubeInstance->GetInstanceCount()));


	spawned = true;
	
}

// Called every frame
void ATerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UWorld* world = GetWorld();

	//Using this if I want a movable terrain
	/*
	if (spawned) {
		MovingActor = FVector(MovingActor.X, MovingActor.Y + 25, MovingActor.Z);

		SetActorLocation(MovingActor);
	}
	

	//Update locations of cube
	for (int i = 0; i < (CubeInstance->GetInstanceCount() - 1); i++) {

		perlinVAL = FMath::PerlinNoise3D(FVector(CubeLocations[i].X, (CubeLocations[i].Y + MovingActor.Y), CubeLocations[i].Z) / 4040);
		CubeInstance->UpdateInstanceTransform(i, FTransform(FVector(CubeLocations[i].X, CubeLocations[i].Y, CubeLocations[i].Z + (2000 * perlinVAL))), false, true, true);

	}
	*/

	//		perlinVAL = FMath::PerlinNoise3D(FVector(CubeLocations[i].X, CubeLocations[i].Y, CubeLocations[i].Z) / 4040);





	//TActorIterator<UInstancedStaticMeshComponent>



	//StaticMesh

	//GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::Blue, FString::Printf(TEXT("Perlin value %f"), perlinVAL));

	//GEngine->AddOnScreenDebugMessage(-1, 0.05, FColor::Silver, FString::Printf(TEXT("X %f , Y %f, Z %f"), ActorVect.X, ActorVect.Y, ActorVect.Z));


}

void ATerrain::Spawn(int X, int Y, FVector SpawnLocation, FRotator SpawnRotation)
{
	FActorSpawnParameters spawnParams;//Default params


	if (perlinVAL <= waterLevel) {//Water
		AllCubes[5]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + 125 + (offSetHeight * waterLevel))));
	}


	//AllCubes[1]->PostDuplicate()

	if (perlinVAL <= -0.3f) {//Sand
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT(" 1")));
		//AllCubes[1]->SetMaterial(0, SandMat);
		sandCount++;

		if (sandCount % 47 <= 1) {
			AllCubes[2]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
		else {
			AllCubes[1]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}

	}
	else if (perlinVAL > -0.3 && perlinVAL <= -0.1) {//Sand 2
	   //GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT(" 2")));
	   //AllCubes[2]->SetMaterial(0, StoneMat);
		
		sandCount2++;

		if (sandCount2 % 22 <= 1) {
			AllCubes[1]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
		else {
			AllCubes[2]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
		//AllCubes[2]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
	}
	else if (perlinVAL > -0.1 && perlinVAL <= 0.3) {//Grass
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT(" 2")));
		//AllCubes[2]->SetMaterial(0, StoneMat);

		grassCount++;

		if (grassCount % 725 <= 2) {
			AllCubes[3]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
		else {
			AllCubes[0]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
		//AllCubes[0]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
	}
	else if (perlinVAL > 0.3 && perlinVAL <= 0.6) {//Stone
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT(" 3")));
		//AllCubes[3]->SetMaterial(0, SnowMat);

		stoneCount++;

		if (stoneCount % 600 <= 2) {
			AllCubes[4]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
		else {
			AllCubes[3]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}

		//AllCubes[3]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
	}
	else {//Snow
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT(" 4")));
		//AllCubes[4]->SetMaterial(0, GrassMat);

		snowCount++;

		if (snowCount % 75 <= 1) {
			AllCubes[3]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
		else {
			AllCubes[4]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
		
		//AllCubes[4]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
	}


	//CubeInstance->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
	
	/*
	//Height of the terrain
	if (perlinVAL <= 0.2f) {
		AllCubes[0]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
	}
	else if (perlinVAL > 0.2 && perlinVAL <= 0.6) {
		AllCubes[1]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
	}
	else if (perlinVAL > 0.6 && perlinVAL <= 0.85) {
		AllCubes[2]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
	}
	else {
		AllCubes[3]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
	}
	*/
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Silver, FString::Printf(TEXT("Perlin value %f"), perlinVAL));

	//AllCubes.Add(CubeInstance);
	//CubeInstance.Addinst

	//SpawnedObject = GetWorld()->SpawnActor<ABasicFloor>(BaseFloor, SpawnLocation, SpawnRotation, spawnParams);//Tile we spawning



	//GetWorld()->SpawnActor<AStaticMeshActor>(BaseFloor, SpawnLocation, SpawnRotation, spawnParams);//Tile we spawning


}

void ATerrain::SpawnDesert(int X, int Y, FVector SpawnLocation, FRotator SpawnRotation)
{
	if (perlinVAL <= waterLevel) {//Water
		AllCubes[5]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + 125 + (offSetHeight * waterLevel))));
	}

	
	if(perlinVAL >=  (waterLevel - 0.04) && perlinVAL <= (waterLevel + 0.01)){//Grass   (Oasis)
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

void ATerrain::SpawnTundra(int X, int Y, FVector SpawnLocation, FRotator SpawnRotation)
{

	if (perlinVAL <= waterLevel) {//Water
		AllCubes[5]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + 125 + (offSetHeight * waterLevel))));
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
	else if(perlinVAL > 0.45f) {//Snow
		
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

void ATerrain::SpawnIslands(int X, int Y, FVector SpawnLocation, FRotator SpawnRotation)
{
	if (perlinVAL <= waterLevel) {//Water
		AllCubes[5]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + 125 + (offSetHeight * waterLevel))));
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
	else if(perlinVAL > 0.6){//Stone
		
		stoneCount++;

		if (stoneCount % 600 <= 2) {
			AllCubes[0]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
		else {
			AllCubes[3]->AddInstanceWorldSpace(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}

	}
}

