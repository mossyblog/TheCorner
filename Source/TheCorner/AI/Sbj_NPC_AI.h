// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SubjectiveActor.h"
#include "Sbj_NPC_AI.generated.h"

UCLASS(Category = "AI")
class THECORNER_API ASbj_NPC_AI : public ASubjectiveActor
{
	GENERATED_BODY()

	
public:
	// Sets default values for this actor's properties
	ASbj_NPC_AI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Subject Handle"), Category = "ECS")
	FSubjectHandle GetSubject();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMesh;


};
