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
	// Spawn a SubjectiveActor with a trait
}

// Called when the game starts or when spawned
void AAINPC::SpawnSubjectiveActorWithTrait()
{
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();

	// Spawn the SubjectiveActor
	GetWorld()->SpawnActor<ASbj_NPC_AI>(SpawnLocation, SpawnRotation);
	//actor->AttachToActor(GetOwner(),FAttachmentTransformRules::KeepRelativeTransform);
}


void AAINPC::BeginPlay()
{
	Super::BeginPlay();

	SpawnSubjectiveActorWithTrait();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	const FVector PlayerLocation = PC->GetPawn()->GetActorLocation();
	
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

		// Set its X/Y randomly
		NewPosition.Value = PlayerLocation;

		// In addition, lets give our subject a destination so that it can randomly walk around
		// Much like our Position struct, this is another custom struct ...
		FDestination NewDestination;
		NewDestination.Value = PlayerLocation;

		// Add the position trait to the subject
		// Calling .SetTrait also adds the trait if it doesn't already exist.
		NewSubject.SetTrait(NewPosition);

		// Add the destination trait to the subject
		NewSubject.SetTrait(NewDestination);

		// Thats it! We created a new subject, and gave it a random position / destination

		UE_LOG(LogTemp, Warning, TEXT("Spawning...: %i"), i);
	}
}


void AAINPC::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Get the player location
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	const FVector PlayerLocation = PC->GetPawn()->GetActorLocation();

	// Print the player location to the screen
	const UWorld* World = GetWorld();
	FThreadSafeCounter Counter;
	Counter.Reset();
	
	{
		// Filter for all actors that have a movement component
		FFilter Filter = FFilter::Make<FSubjectPosition, FDestination>();
		TSharedRef<TChain<TChunkIt<FSolidSubjectHandle>, TBeltIt<FSolidSubjectHandle>>> Chain = EnchainSolid(Filter);
		
		
		Chain->OperateConcurrently(
			[&](FSolidSubjectHandle CurrentSubject, FSubjectPosition CurrentPos, FDestination Destination)
			{
				Counter.Increment();
				uint32 ThreadId = FPlatformTLS::GetCurrentThreadId();
				//UE_LOG(LogTemp, Warning, TEXT("ConcRunning: Thread %i, Counter %i"), ThreadId, Counter.GetValue());
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
					CurrentSubject.SetTraitDeferred(UpdatedPos); // Set the new position trait on our subject
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
	
	UE_LOG(LogTemp, Warning, TEXT("ConcRunning:  Counter %i"), Counter.GetValue());
	const FString VectorString = FString::Printf(TEXT("X:%f, Y:%f, Z:%f | T:%d"), PlayerLocation.X, PlayerLocation.Y, PlayerLocation.Z, Counter.GetValue());
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, VectorString);
	
	// Lets get a new destination for anyone who has finished their move.
	// MECHANIC - Get all Subjects who have finished moving, update their desired location, then remove their "Finished Moving" trait.
	{
		// 1. Make a filter of subjects who have finished moving
		FFilter Filter = FFilter::Make<FFinishedMoving>();

		// 2. Make our chain
		//FChain* Chain = Enchain(Filter);

		// 3. Operate!
		EnchainSolid(Filter)->OperateConcurrently([&](FSolidSubjectHandle CurrentSubject)
		{
			// Firstly, lets create a new random destination for the subject
			FDestination NewDestination;
			NewDestination.Value = PlayerLocation;

			// Now lets add it to the subject
			CurrentSubject.SetTraitDeferred(NewDestination);

			// Lastly, lets remove the finished moving trait, so that this code doesnt call again.
			// Next tick, this subject will continue moving
			CurrentSubject.RemoveTraitDeferred<FFinishedMoving>();
		}, /*Maximum number of threads=*/4, /*Minimum number of Subjects=*/2);
	}

	// Lets also create a utility mechanic that draws our destinations
	// MECHANIC - Draws the DESTINATION of all subjects with destinations.
	{
		// 1. Make a filter of only finished moving subjects
		const FFilter Filter = FFilter::Make<FDestination>();

		// 2. Make our chain
		//FChain* Chain = Enchain(Filter);

		// 3. Operate!
		EnchainSolid(Filter)->OperateConcurrently([&](FSolidSubjectHandle CurrentSubject, FDestination CurrentDestination)
		{
			// Unreal command to draw our positions.
			DrawDebugPoint(World, CurrentDestination.Value, 5.0f, FColor::Red, false, 0.25f);
		}, /*Maximum number of threads=*/4, /*Minimum number of Subjects=*/2);
	}


	// Lastly, lets write a simple system to debug draw our subject positions so we can see what is going on.
	// MECHANIC - Draws the POSITION of all subjects with positions.
	{
		// 1. Make a filter of only finished moving subjects
		const FFilter Filter = FFilter::Make<FSubjectPosition>();

		// 2. Make our chain
		//FChain* Chain = Enchain(Filter);

		// 3. Operate!
		EnchainSolid(Filter)->OperateConcurrently([&](FSolidSubjectHandle CurrentSubject, FSubjectPosition CurrentPos)
		{
			// Unreal command to draw our positions.
			DrawDebugPoint(World, CurrentPos.Value, 20.0f, FColor::Green, false, 0.01f);
			
		}, /*Maximum number of threads=*/4, /*Minimum number of Subjects=*/2);
	}
}
