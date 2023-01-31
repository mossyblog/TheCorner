// Fill out your copyright notice in the Description page of Project Settings.


#include "AAINPC.h"

#include "MainStructs.h"
#include "TheCorner/TheCornerGameMode.h"

AAINPC::AAINPC()
{
}

void AAINPC::BeginPlay()
{
	Super::BeginPlay();

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
		NewPosition.Value = FVector(FMath::RandRange(-RandomWidth, RandomWidth),
		                            FMath::RandRange(-RandomWidth, RandomWidth), 0);

		// In addition, lets give our subject a destination so that it can randomly walk around
		// Much like our Position struct, this is another custom struct ...
		FDestination NewDestination;
		NewDestination.Value = FVector(FMath::RandRange(-RandomWidth, RandomWidth),
		                               FMath::RandRange(-RandomWidth, RandomWidth), 0);

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
	// Filter for all actors that have a movement component
	FFilter Filter = FFilter::Make<FDestination>();
	int Counter = 0;
	TSharedRef<TChain<TChunkIt<FSolidSubjectHandle>, TBeltIt<FSolidSubjectHandle>>> Chain = EnchainSolid(Filter);
	Chain->OperateConcurrently([&Counter](FSolidSubjectHandle Subject,  FSubjectPosition CurrentPos, FDestination Destination)
	{
		Counter += CurrentPos.Total+1;
		UE_LOG(LogTemp, Warning, TEXT("ConcRunning: %i"), Counter)
	}, /*Maximum number of threads=*/4, /*Minimum number of Subjects=*/8);
}
