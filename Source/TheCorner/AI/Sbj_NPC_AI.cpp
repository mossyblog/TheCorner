// Fill out your copyright notice in the Description page of Project Settings.


#include "Sbj_NPC_AI.h"
#include "SubjectiveActorComponent.h"
#include "AAINPC.h"
#include "MainStructs.h"
#include "Engine.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASbj_NPC_AI::ASbj_NPC_AI()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	/*
	if (!StaticMesh)
	{
		StaticMesh = NewObject<UStaticMeshComponent>(this, TEXT("StaticMesh0"));
		StaticMesh->SetupAttachment(RootComponent);
	}
	*/

	
	
	//SetTrait(NewPosition);
	//SetTrait(NewDestination);
    
	// Load the default cube static mesh
	/*static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMeshAsset.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("Mesh Created"));
		StaticMesh->SetStaticMesh(CubeMeshAsset.Object);
	}*/
	//RootComponent = StaticMesh;
}

// Called when the game starts or when spawned
void ASbj_NPC_AI::BeginPlay()
{
	Super::BeginPlay();
	SetActorEnableCollision(false);

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	const FVector PlayerLocation = PC->GetPawn()->GetActorLocation();

	const int32 RandomWidth = 1000;
	
	FDestination NewDestination;
	FSubjectPosition NewPosition;
	NewPosition.Value = FVector(FMath::RandRange(-RandomWidth,RandomWidth), FMath::RandRange(-RandomWidth,RandomWidth), 0);
	NewPosition.DisplayName = "Manual";
	NewPosition.Color = FColor::Red;
	
	SetTrait(NewPosition);
	SetTrait(NewDestination);
}

// Called every frame
void ASbj_NPC_AI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GetActor()->SetActorLocation(GetTraitPtr<FSubjectPosition, EParadigm::Auto>()->Value);
}

UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Subject Handle"), Category = "ECS", BlueprintCallable)
FSubjectHandle ASbj_NPC_AI::GetSubject()
{
	return GetHandle();
}

