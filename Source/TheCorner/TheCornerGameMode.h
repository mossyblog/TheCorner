// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TheCornerGameMode.generated.h"

USTRUCT(BlueprintType)
struct FMovement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyStruct")

	FVector Value;
};

UCLASS(minimalapi)
class ATheCornerGameMode : public AGameModeBase
{
	GENERATED_BODY()
	


public:
	ATheCornerGameMode();
};



