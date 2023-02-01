// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ApparatusRuntime.h"
#include "Mechanism.h"
#include "MechanicalActor.h"
#include "AAINPC.generated.h"
/**
 * 
 */
UCLASS()
class THECORNER_API AAINPC : public AMechanicalActor
{
	GENERATED_BODY()
	
	virtual void Tick(const float DeltaTime) override;
	virtual void BeginPlay() override;
public:
	AAINPC();
	void SpawnSubjectiveActorWithTrait() ;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoveRandomly")
	float MinDistanceFroMDestination = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoveRandomly")
	float MoveSpeed  = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoveRandomly")
	float RandomWidth  = 250.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoveRandomly")
	int NumSubjects  = 10;
};
