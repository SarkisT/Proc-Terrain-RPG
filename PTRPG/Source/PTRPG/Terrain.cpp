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

	//Get Player Location
	MyChar = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	ChunkDistance = NumberMeshesX * Distance;

	renderDistance = ChunkDistance * maxSpawns;
	
	//Save Chunk Distance
	ChunkDist = FVector(MyChar.X + ChunkDistance, MyChar.Y + ChunkDistance, MyChar.Z);

	//Center first Block
	//InitChunk = FVector(MyChar.X, MyChar.Y, MyChar.Z - 6000);
	
	InitChunk = FVector(MyChar.X - (ChunkDist.X / 2), MyChar.Y - (ChunkDist.Y / 2), MyChar.Z - 6000);

	//MyChar = FVector(MyChar.X - (ChunkDist.X/2), MyChar.Y - (ChunkDist.Y / 2), MyChar.Z - 6000);

	GEngine->AddOnScreenDebugMessage(-1, 25, FColor::Silver, FString::Printf(TEXT("Initial Block Location is : %s "), *InitChunk.ToString()));

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
		//GEngine->AddOnScreenDebugMessage(-1, 25.0f, FColor::Blue, FString::Printf(TEXT("%i "), p));
		//GEngine->AddOnScreenDebugMessage(-1, 25.0f, FColor::Red, result);
		

		p++;


	}

	GI = Cast<UMyGI>(GetGameInstance());

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
		GetWorld()->SpawnActor<AActor>(WaterCollider, FVector(InitChunk.X, InitChunk.Y, (InitChunk.Z - 300 + (offSetHeight * waterLevel))), FRotator::ZeroRotator); //"Swim" Zone
		break;
	case 2://Tundra
		offSetHeight += 4000;
		offSetZoom += 10000;
		waterLevel = seed.FRandRange(-0.6, -0.35);

		GetWorld()->SpawnActor<AActor>(WaterCollider, FVector(InitChunk.X, InitChunk.Y, (InitChunk.Z - 300 + (offSetHeight * waterLevel))), FRotator::ZeroRotator); //"Swim" Zone
		
		break;
	case 3://Plains

		GetWorld()->SpawnActor<AActor>(WaterCollider, FVector(InitChunk.X, InitChunk.Y, (InitChunk.Z - 300 + (offSetHeight * waterLevel))), FRotator::ZeroRotator); //"Swim" Zone

		
		break;
	case 4://Islands
		waterLevel = waterLevel + 0.5f;

		GetWorld()->SpawnActor<AActor>(WaterCollider, FVector(InitChunk.X, InitChunk.Y, (InitChunk.Z - 300 + (offSetHeight* waterLevel))) , FRotator::ZeroRotator); //"Swim" Zone
		
		
		break;
	}


	//Spawn initial Chunks.

	

	//NE
	//GenerateChunk(FVector(InitChunk.X + (1 * ChunkDistance), InitChunk.Y + (1 * ChunkDistance), InitChunk.Z), 1, 1);
	//E
	//GenerateChunk(FVector(InitChunk.X + (0 * ChunkDistance), InitChunk.Y + (1 * ChunkDistance), InitChunk.Z), 0, 1);
	//SE
	//GenerateChunk(FVector(InitChunk.X + (-1 * ChunkDistance), InitChunk.Y + (1 * ChunkDistance), InitChunk.Z), -1, 1);
	//N
	//GenerateChunk(FVector(InitChunk.X + (1 * ChunkDistance), InitChunk.Y + (0 * ChunkDistance), InitChunk.Z), 1, 0);

	//MIDDLE
	GenerateChunk(FVector(InitChunk.X, InitChunk.Y, InitChunk.Z), 0, 0);

	//S
	//GenerateChunk(FVector(InitChunk.X + (-1 * ChunkDistance), InitChunk.Y + (0 * ChunkDistance), InitChunk.Z), -1, 0);
	//NW
	//GenerateChunk(FVector(InitChunk.X + (1 * ChunkDistance), InitChunk.Y + (-1 * ChunkDistance), InitChunk.Z), 1, -1);
	//W
	//GenerateChunk(FVector(InitChunk.X + (0 * ChunkDistance), InitChunk.Y + (-1 * ChunkDistance), InitChunk.Z), 0, -1);
	//SW
	//GenerateChunk(FVector(InitChunk.X + (-1 * ChunkDistance), InitChunk.Y + (-1 * ChunkDistance), InitChunk.Z), -1, -1);


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



	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Initial seed chosen: %i  , but we chose this seed %i , with the offset of : %i , Height of : %i , ZOOM : %i "), seed.GetInitialSeed(), seed.GetCurrentSeed(), offSet, offSetHeight, offSetZoom));

	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("BIOME :  %i , Water level : %f "), biome, waterLevel ));

	//InitChunk = FVector(InitChunk.X, InitChunk.Y, InitChunk.Z);
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Number of cubes instanced! :  %i  "), CubeInstance->GetInstanceCount()));

	//AllCubes[2]->ClearInstances();

	//CubeLocations.Empty();

	prevX = CoordX;
	prevY = CoordY;

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Black, FString::Printf(TEXT("EMPTY WATER")));

	

	spawned = true;

	//AllCubes[0].Removeinstance
	//DeleteChunk(0, 0);
	//DeleteChunk(1, 0);
	
}

// Called every frame
void ATerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UWorld* world = GetWorld();


	MyChar = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	
	GI->Player = MyChar;

	CoordX = (MyChar.X / ChunkDistance);
	CoordY = (MyChar.Y / ChunkDistance);

	GEngine->AddOnScreenDebugMessage(-1, 0.03, FColor::Orange, FString::Printf(TEXT("MYCHAR : %f "), MyChar.X));

	GEngine->AddOnScreenDebugMessage(-1, 0.03, FColor::Orange, FString::Printf(TEXT("Coord X : %i "), CoordX));
	GEngine->AddOnScreenDebugMessage(-1, 0.03, FColor::Orange, FString::Printf(TEXT("Coord Y : %i "), CoordY));


	//Manage Chunks, won't add duplicate chunks, maybe
	if (prevX != CoordX) {
		GEngine->AddOnScreenDebugMessage(-1, 25, FColor::Emerald, FString::Printf(TEXT("Changed X ")));

		for (int i = -2; i < 3; i++) {
			for (FVector spawns : GI->SpawnedChunks) {

				//XY

				//00 01 02 03 04 05
				//10 11 12 13 14 15
				//20 21 22 23 24 25
				//30 31 [32] 33 34 35
				//40 41 42 43 44 45
				//50 51 52 53 54 55
				if (FVector(InitChunk.X + (CoordX * ChunkDistance), InitChunk.Y + (CoordY * ChunkDistance) + (i * ChunkDistance), InitChunk.Z) == spawns) {

					//GEngine->AddOnScreenDebugMessage(-1, 25, FColor::Red, FString::Printf(TEXT("EEEEEEEEEEEEEE")));

					bChunk = true;
				}

			}

			if (bChunk == false) {
				GenerateChunk(FVector(InitChunk.X + (CoordX * ChunkDistance), InitChunk.Y + (CoordY * ChunkDistance) + (i * ChunkDistance), InitChunk.Z), CoordX, CoordY + i);
			}


			bChunk = false;

		}

		
		prevX = CoordX;

	}

	if (prevY != CoordY) {
		GEngine->AddOnScreenDebugMessage(-1, 25, FColor::Orange, FString::Printf(TEXT("Changed Y ")));

		for (int i = -2; i < 3; i++) {

		

			for (FVector spawns : GI->SpawnedChunks) {

				if (FVector(InitChunk.X + (CoordX * ChunkDistance) + (i * ChunkDistance), InitChunk.Y + (CoordY * ChunkDistance), InitChunk.Z) == spawns) {
					bChunk = true;

					//GEngine->AddOnScreenDebugMessage(-1, 25, FColor::Yellow, FString::Printf(TEXT("FOUND DooPE ")));

				}

			}

			if (bChunk == false) {
				GenerateChunk(FVector(InitChunk.X + (CoordX * ChunkDistance) + (i * ChunkDistance), InitChunk.Y + (CoordY * ChunkDistance), InitChunk.Z), CoordX + i, CoordY);
			}

			bChunk = false;
		
		}

		prevY = CoordY;

	}

	//GEngine->AddOnScreenDebugMessage(-1, 0.03, FColor::Blue, FString::Printf(TEXT("Player is : %s "), *MyChar.ToString()));

	//if (MyChar.X > (((CoordX + 1) * 25000) - 5000) + InitChunk.X) {

	//Perhaps Overwrite InitChunk?

	//InitChunk = FVector(InitChunk.X + 12500, InitChunk.Y, InitChunk.Z);


	/*
	if ((MyChar.X - ChunkDistance) > ((CoordX + 1) * ChunkDistance)) {
		CoordX++;
		N++;
		if (N > 1) {		
			GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Black, FString::Printf(TEXT("NORTH")));

			//GenerateChunk(FVector(InitChunk.X + (CoordX * ChunkDistance), InitChunk.Y + ((CoordY + 1) * ChunkDistance), InitChunk.Z), CoordX, CoordY + 1);
			GenerateChunk(FVector(InitChunk.X + (CoordX * ChunkDistance), InitChunk.Y + (CoordY * ChunkDistance), InitChunk.Z), CoordX, CoordY);
			//GenerateChunk(FVector(InitChunk.X + (CoordX * ChunkDistance), InitChunk.Y + ((CoordY - 1) * ChunkDistance), InitChunk.Z), CoordX, CoordY - 1);
			N = 1;
			E = 0;
			W = 0;
			S = -1;
		}
		
	}
	else if ((MyChar.X - ChunkDistance) < (CoordX * ChunkDistance) && (MyChar.X - ChunkDistance) > ((CoordX - 2) * ChunkDistance)) {
		CoordX--;
		S++;
		if (S > 1) {
			//GenerateChunk(FVector(InitChunk.X + (CoordX * 25000), InitChunk.Y + (CoordY * 25000), InitChunk.Z), CoordX, CoordY);
			GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Black, FString::Printf(TEXT("SOUTH")));

			//GenerateChunk(FVector(InitChunk.X + (CoordX * ChunkDistance), InitChunk.Y + ((CoordY + 1) * ChunkDistance), InitChunk.Z), CoordX, CoordY + 1);
			GenerateChunk(FVector(InitChunk.X + (CoordX * ChunkDistance), InitChunk.Y + (CoordY * ChunkDistance), InitChunk.Z), CoordX, CoordY);
			//GenerateChunk(FVector(InitChunk.X + (CoordX * ChunkDistance), InitChunk.Y + ((CoordY - 1) * ChunkDistance), InitChunk.Z), CoordX, CoordY - 1);
			N = -1;
			E = 0;
			W = 0;
			S = 1;
			
		}
		
	}
	*/

	/*
	if ((MyChar.Y - ChunkDistance) > ((CoordY + 1) * ChunkDistance) + InitChunk.Y) {
		CoordY++;
		E++;

		if (E > 1) {
			GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Black, FString::Printf(TEXT("EAST")));


			//GenerateChunk(FVector(InitChunk.X + ((CoordX + 1) * ChunkDistance), InitChunk.Y + (CoordY * ChunkDistance), InitChunk.Z), CoordX + 1, CoordY);
			GenerateChunk(FVector(InitChunk.X + (CoordX * ChunkDistance), InitChunk.Y + (CoordY * ChunkDistance), InitChunk.Z), CoordX, CoordY);
			//GenerateChunk(FVector(InitChunk.X + ((CoordX - 1) * ChunkDistance), InitChunk.Y + (CoordY * ChunkDistance), InitChunk.Z), CoordX - 1, CoordY);
			N = 0;
			S = 0;
			E = 1;
			W = -1;
		}
		
	}
	else if ((MyChar.Y - ChunkDistance) < (CoordY * ChunkDistance) + InitChunk.Y && (MyChar.Y - ChunkDistance) >((CoordY - 2) * ChunkDistance)) {
		CoordY--;
		W++;
		if (W > 1) {
			//GenerateChunk(FVector(InitChunk.X + (CoordX * 25000), InitChunk.Y + (CoordY * 25000), InitChunk.Z), CoordX, CoordY);
			GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Black, FString::Printf(TEXT("WEST")));


			//GenerateChunk(FVector(InitChunk.X + ((CoordX + 1) * ChunkDistance), InitChunk.Y + (CoordY * ChunkDistance), InitChunk.Z), CoordX + 1, CoordY);
			GenerateChunk(FVector(InitChunk.X + (CoordX * ChunkDistance), InitChunk.Y + (CoordY * ChunkDistance), InitChunk.Z), CoordX, CoordY);
			//GenerateChunk(FVector(InitChunk.X + ((CoordX - 1) * ChunkDistance), InitChunk.Y + (CoordY * ChunkDistance), InitChunk.Z), CoordX - 1, CoordY);
			N = 0;
			S = 0;
			W = 1;
			E = -1;
		}
	}

	*/
	
	//GEngine->AddOnScreenDebugMessage(-1, 0.03, FColor::Blue, FString::Printf(TEXT("Player is : %s "), *MyChar.ToString()));

	//GEngine->AddOnScreenDebugMessage(-1, 0.03, FColor::Blue, FString::Printf(TEXT("Player is : %f "), ((CoordX + 1) * ChunkDistance) + InitChunk.X));

	//GEngine->AddOnScreenDebugMessage(-1, 0.03, FColor::Green, FString::Printf(TEXT("COORDS, X : %i , Y : %i "), CoordX, CoordY));

	//GEngine->AddOnScreenDebugMessage(-1, 0.03, FColor::Red, FString::Printf(TEXT("COORDS, N : %i , E : %i , W : %i , S : %i "), N, E , W , S));

	

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
	
	//GEngine->AddOnScreenDebugMessage(-1, 25, FColor::Silver, FString::Printf(TEXT("Initial Block Location is : %s "), *SpawnLocation.ToString()));

}

void ATerrain::SpawnIslands(int X, int Y, FVector SpawnLocation, FRotator SpawnRotation)
{
	//CubeTransform = GetInstanceComponents().
																				//      + 125
	//CubeTransform = FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * waterLevel)));

	if (perlinVAL <= waterLevel) {//Water
		AllCubes[5]->AddInstance(CubeTransform);
	}

	

	if (perlinVAL <= 0.1f) {//Sand
		sandCount++;

		if (sandCount % 97 <= 1) {
			AllCubes[2]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
		else {
			AllCubes[1]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
	}
	else if (perlinVAL > 0.1 && perlinVAL <= 0.4) {//Sand 2
	
		sandCount2++;

		if (sandCount2 % 67 <= 1) {
			AllCubes[1]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
		else {
			AllCubes[2]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
	}
	else if (perlinVAL > 0.4 && perlinVAL <= 0.6) {//Grass

		grassCount++;

		if (grassCount % 725 <= 2) {
			AllCubes[3]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
		else {
			AllCubes[0]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
	}
	else if(perlinVAL > 0.6){//Stone
		
		stoneCount++;

		if (stoneCount % 600 <= 2) {
			AllCubes[0]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));

			
		}
		else {
			AllCubes[3]->AddInstance(FTransform(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + (offSetHeight * perlinVAL))));
		}
		
	}
}

void ATerrain::GenerateChunk(FVector ChunkLocation, int X, int Y)
{
 	chunkId++;

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
	//Maybe make this game instance only;
	
	//SpawnedChunk = GetWorld()->SpawnActor<ATerrainChunk>(FVector(ChunkLocation.X + (X * Distance), ChunkLocation.Y + (Y * Distance), ChunkLocation.Z), FRotator::ZeroRotator, spawnParams);

	//We have the subobject but components spawn without any static mesh or material. Use this to get blueprint values.
	SpawnedChunk = Cast<ATerrainChunk>(GetWorld()->SpawnActor(Chunk.Get()));

	

	//THIS WORKED OMG

	//GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Black, FString::Printf(TEXT("SPAWNED")));
	
	//GetWorld()->SpawnActor<ATerrainChunk>(FVector(ChunkLocation.X + (X * Distance), ChunkLocation.Y + (Y * Distance), ChunkLocation.Z), FRotator::ZeroRotator, spawnParams);
	
	//SpawnedChunk->InitializeComponents();
	
	//SpawnedChunk = Cast<ATerrainChunk>(Chunk.Get());

	//SpawnedChunk = Cast<ATerrainChunk>(GetWorld()->SpawnActor<ATerrainChunk>(FVector(ChunkLocation.X + (X * Distance), ChunkLocation.Y + (Y * Distance), ChunkLocation.Z), FRotator::ZeroRotator));

	

	
	
	
	

	//UGameplayStatics::FinishSpawningActor(SpawnedChunk, FTransform(ChunkLocation));

	//SpawnedChunk = GetWorld()->SpawnActor<ATerrainChunk>(FVector(ChunkLocation.X + (X * Distance), ChunkLocation.Y + (Y * Distance), ChunkLocation.Z), FRotator::ZeroRotator);


	//SpawnedChunk = Cast<ATerrainChunk>(GetWorld()->SpawnActor(Chunk.Get()));

	//GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Yellow, FString::Printf(TEXT("SPAWNED")));

	
	
	//Migh use HashMaps.
	switch (biome) {
	case 1://Desert
		for (int i = 0; i < NumberMeshesX; i++)//X number of Meshes to spawn
		{
			for (int j = 0; j < NumberMeshesY; j++)//Y number of Mesh to spawn
			{
				for (int k = 0; k < NumberMeshesZ; k++) {
					//Spawning location for our tile.
					FVector TileLocation = FVector(ChunkLocation.X + (i * Distance), ChunkLocation.Y + (j * Distance), ChunkLocation.Z + (k * Distance));

					//CubeLocations.Add(TileLocation);


					perlinVAL = FMath::PerlinNoise3D(FVector(TileLocation.X, TileLocation.Y, TileLocation.Z + offSet) / offSetZoom);


					//GetWorld().

					//SpawnedChunk->SpawnDesert(X, Y, TileLocation, FRotator::ZeroRotator);
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
					//Spawning location for our tile.
					FVector TileLocation = FVector(ChunkLocation.X + (i * Distance), ChunkLocation.Y + (j * Distance), ChunkLocation.Z + (k * Distance));

					//CubeLocations.Add(TileLocation);


					perlinVAL = FMath::PerlinNoise3D(FVector(TileLocation.X, TileLocation.Y, TileLocation.Z + offSet) / offSetZoom);

					

					//SpawnedChunk->SpawnTundra(X, Y, TileLocation, FRotator::ZeroRotator);
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
					//Spawning location for our tile.
					FVector TileLocation = FVector(ChunkLocation.X + (i * Distance), ChunkLocation.Y + (j * Distance), ChunkLocation.Z + (k * Distance));

					//CubeLocations.Add(TileLocation);


					perlinVAL = FMath::PerlinNoise3D(FVector(TileLocation.X, TileLocation.Y, TileLocation.Z + offSet) / offSetZoom);

					//SpawnedChunk->Spawn(X, Y, TileLocation, FRotator::ZeroRotator);
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
					//Spawning location for our tile.
					FVector TileLocation = FVector(ChunkLocation.X + (i * Distance), ChunkLocation.Y + (j * Distance), ChunkLocation.Z + (k * Distance));

					//CubeLocations.Add(TileLocation);


					perlinVAL = FMath::PerlinNoise3D(FVector(TileLocation.X, TileLocation.Y, TileLocation.Z + offSet) / offSetZoom);
					
					//SpawnedChunk->SpawnIslands(X, Y, TileLocation, FRotator::ZeroRotator);
				}
			}
		}		
		break;
	}

	/*
	
	//Track all the instances of current chunk
	meshInstances.Add(sandCount);
	meshInstances.Add(sandCount2);
	meshInstances.Add(grassCount);
	meshInstances.Add(stoneCount);
	meshInstances.Add(snowCount);
	meshInstances.Add(waterCount);

	//Reset for next chunk
	sandCount = 0;
	sandCount2 = 0;
	grassCount = 0;
	stoneCount = 0;
	snowCount = 0;
	waterCount = 0;

	//Add it to chunk
	ChunkYMap.Add(Y, meshInstances);
	ChunkXMap.Add(X, ChunkYMap);

	meshInstances.Empty();
	//Reset for next chunk

	*/


	

}

void ATerrain::DeleteChunk(int x, int y)
{
	//ChunkValMap = ChunkYMap.FindAndRemoveChecked;


	/*
	//Find X, Get Y Map

	if (ChunkXMap.Find(x) != nullptr) {
		ChunkTestMap = ChunkXMap.FindChecked(x);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Black, FString::Printf(TEXT("NULL CHUNK DATA X")));

	}
	


	//Found Y Map, Getting values depending on Y
	if (ChunkTestMap.Find(y) != nullptr) {
		ChunkValMap = ChunkTestMap.FindAndRemoveChecked(y);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Black, FString::Printf(TEXT("NULL CHUNK DATA Y")));


	}



	//Get all Cube types
	for (int i = 0; i < 6; i++) {
		//Remove the number of Instances of the specific cube
		for (ChunkValMap[i]; ChunkValMap[i] > 0; ChunkValMap[i]--) {

			auto de = AllCubes[i];

			de->RemoveInstance(0);

			//AllCubes[i].RemoveInstance(0);
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 25, FColor::Silver, FString::Printf(TEXT("Chunk Deleted ")));


	ChunkValMap.Empty();
	ChunkTestMap.Empty();

	*/
	
}

