// Fill out your copyright notice in the Description page of Project Settings.


#include "AAINPC.h"
#include "DrawDebugHelpers.h" // ue4 code to debug draw stuff
#include "MainStructs.h"
#include "Sbj_NPC_AI.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "HAL/PlatformProcess.h"
#include "HAL/ThreadSafeCounter.h"
#include "TheCorner/TheCornerGameMode.h"
#include "SubjectiveActor.h"


AAINPC::AAINPC()
{

	Counter = new int32(0);

}
void AAINPC::SpawnSubjectiveActorsWithTrait()
{
	for (int i = 0; i < NumSubjects; i++)
	{
		AActor *player = GetWorld()->SpawnActor(PlayerBlueprintType.Get());
	}
}

// Called when the game starts or when spawned
void AAINPC::SpawnSubjectiveActorWithTrait()
{
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();

	// Spawn the SubjectiveActor
	//GetWorld()->SpawnActor<ASbj_NPC_AI>(SpawnLocation, SpawnRotation);

	// Step 2. Wherever you need a UClass * reference for that blueprint (like SpawnActor)
	AActor *player = GetWorld()->SpawnActor(PlayerBlueprintType.Get());
	
	//actor->AttachToActor(GetOwner(),FAttachmentTransformRules::KeepRelativeTransform);
}


void AAINPC::BeginPlay()
{
	Super::BeginPlay();

	SpawnSubjectiveActorWithTrait();
	//SpawnSubjectsOnly();
	SpawnSubjectiveActorsWithTrait();
}

void AAINPC::SpawnSubjectsOnly()
{
	// We need to spawn some subjects initially to perform the logic on
	for (int i = 0; i < NumSubjects; i++)
	{
		// Create a new subject and save a refrence to its handle
		// An FSubjectHandle is the main way you're going to interface with Apparatus when adding/removing data from a subject
		FSubjectHandle NewSubject = SpawnSubject();

		// Create a new position trait for this subject
		// This is a custom made NON Apparatus struct that you will have to create for your own project
		// It is defined in MainStructs.h
		FSubjectPosition NewPosition;

		
		NewPosition.DisplayName = "Automated";
		NewPosition.Color = FColor::Green;
		// In addition, lets give our subject a destination so that it can randomly walk around
		// Much like our Position struct, this is another custom struct ...
		
		// Set its X/Y randomly
		NewPosition.Value =FVector(FMath::RandRange(-RandomWidth,RandomWidth), FMath::RandRange(-RandomWidth,RandomWidth), 0);;
	
		// Add the position trait to the subject
		// Calling .SetTrait also adds the trait if it doesn't already exist.
		NewSubject.SetTrait(NewPosition);

		// Add the destination trait to the subject
		FDestination NewDestination;
		NewSubject.SetTrait(NewDestination);

		// Thats it! We created a new subject, and gave it a random position / destination

		UE_LOG(LogTemp, Warning, TEXT("Spawning...: %i"), i);
	}
}

// Move subjects towards their destination
void AAINPC::MoveSubjects(const float DeltaTime)
{
	// Filter for all actors that have a movement component
	FFilter Filter = FFilter::Make<FSubjectPosition, FDestination>();
	TSharedRef<TChain<TChunkIt<FSolidSubjectHandle>, TBeltIt<FSolidSubjectHandle>>> Chain = EnchainSolid(Filter);
	
	Chain->OperateConcurrently(
		[&](FSolidSubjectHandle CurrentSubject, FSubjectPosition CurrentPos, FDestination Destination)
		{
			// Get distance from our destination
			float DistFromDestination = FVector::Dist(CurrentPos.Value, Destination.Value);

			
			// If we're currently far from our destination, then move toward it.
			if (DistFromDestination > MinDistanceFroMDestination) // MinDistanceFroMDestination decalred in .h file.
			{
				// The vector pointing toward destination
				FVector MoveVector = Destination.Value - CurrentPos.Value;

				// Normalize it so that we have a unit vector to work with
				MoveVector.Normalize();
				
				// Scale the direction by movespeed and delta seconds.
				// In addition, you could consider storing your subjects MoveSpeed in another trait, accessing it via ...
				// ... this filter, and setting your speed that way.
				MoveVector *= DeltaTime * MoveSpeed; // MoveSpeed decalred in .h file.

				// Lets update the position trait
				FSubjectPosition UpdatedPos; // Create a new Position trait
				UpdatedPos.Value = CurrentPos.Value + MoveVector; // Add the move vector onto our existing position
				UpdatedPos.DisplayName = CurrentPos.DisplayName;
				UpdatedPos.Color = CurrentPos.Color;
				CurrentSubject.SetTraitDeferred(UpdatedPos); // Set the new position trait on our subject

				//CurrentPos.Value = CurrentPos.Value + MoveVector;
			}

			// We're already close enough to have finished our move.
			else
			{
				// Create our finished moving struct...
				FFinishedMoving FinishedMove;

				// NOTE, that this Struct doesn't actually contain any data. We're just using it as an on/off trait to find Subjects.
				// In this way, we can use Filter.Include, and Filter.Exclude to construct Mechanics that operate over specifically flagged subjects

				// Add the new trait to our subject
				// NOTE - Calling "Set Trait" will either ADD a given trait or OVERRIDE a given trait if it already exists
				CurrentSubject.SetTraitDeferred(FinishedMove);
			}
			
		}, /*Maximum number of threads=*/4, /*Minimum number of Subjects=*/2);
}

// Get new destination for subjects who have finished their move
void AAINPC::GetNewDestination()
{
	// 1. Make a filter of subjects who have finished moving
	FFilter Filter = FFilter::Make<FFinishedMoving>();

	// 3. Operate!
	Enchain(Filter)->Operate([&](FSubjectHandle CurrentSubject)
	{
		// Firstly, lets create a new random destination for the subject
		FDestination NewDestination;
		NewDestination.Value = FVector(FMath::RandRange(-RandomWidth,RandomWidth), FMath::RandRange(-RandomWidth,RandomWidth), 0);;
		// Now lets add it to the subject
		CurrentSubject.SetTraitDeferred(NewDestination);

		// Lastly, lets remove the finished moving trait, so that this code doesnt call again.
		// Next tick, this subject will continue moving
		CurrentSubject.RemoveTraitDeferred<FFinishedMoving>();
	});
}

// Draw destinations of subjects
void AAINPC::DrawDestinations() const
{
	// 1. Make a filter of only finished moving subjects
	const FFilter Filter = FFilter::Make<FDestination>();

	// 2. Make our chain
	//FChain* Chain = Enchain(Filter);

	// 3. Operate!
	Enchain(Filter)->Operate([&](FSubjectHandle CurrentSubject, FDestination CurrentDestination)
	{
		// Unreal command to draw our positions.
		//DrawDebugPoint(GetWorld(), CurrentDestination.Value, 5.0f, FColor::Red, false, 0.01f);
		
	});
}

// Draw positions of subjects
void AAINPC::DrawPositions() const
{
	// 1. Make a filter of only finished moving subjects
	const FFilter Filter = FFilter::Make<FSubjectPosition>();

	// 2. Make our chain
	//FChain* Chain = Enchain(Filter);

	// 3. Operate!
	Enchain(Filter)->Operate([&](FSubjectHandle CurrentSubject, FSubjectPosition CurrentPos)
	{
		// Unreal command to draw our positions.
		DrawDebugPoint(GetWorld(), CurrentPos.Value, 10.0f, CurrentPos.Color, false, 0.1f);

		const FString PosLocation = FString::Printf(TEXT("%s: X:%f, Y:%f, Z:%f"), *CurrentPos.DisplayName, CurrentPos.Value.X,  CurrentPos.Value.Y, CurrentPos.Value.Z);
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, PosLocation);
		
	});
}

void AAINPC::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	const FVector PlayerLocation = PC->GetPawnOrSpectator()->GetActorLocation();
	//DrawDebugPoint(GetWorld(), PlayerLocation, 20.0f, FColor::Black, false, 1.0f);
	MoveSubjects(DeltaTime);
	GetNewDestination();
	//DrawDestinations();
	//DrawPositions();


	const FString VectorString = FString::Printf(TEXT("X:%f, Y:%f, Z:%f | Total"), PlayerLocation.X, PlayerLocation.Y, PlayerLocation.Z);
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, VectorString);
}

