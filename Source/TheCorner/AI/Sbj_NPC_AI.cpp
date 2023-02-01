// Fill out your copyright notice in the Description page of Project Settings.


#include "Sbj_NPC_AI.h"
#include "SubjectiveActorComponent.h"
#include "AAINPC.h"
#include "MainStructs.h"


// Sets default values
ASbj_NPC_AI::ASbj_NPC_AI()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

}

// Called when the game starts or when spawned
void ASbj_NPC_AI::BeginPlay()
{
	Super::BeginPlay();

	FSubjectPosition NewPosition;
	FDestination NewDestination;
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	
	const FVector PlayerLocation = PC->GetPawn()->GetActorLocation();
	NewPosition.Value = PlayerLocation;
	SetTrait(NewPosition);
	SetTrait(NewDestination);
	
}

// Called every frame
void ASbj_NPC_AI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

