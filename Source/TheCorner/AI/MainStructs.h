// A subjects position
// We will use this value as the real world "truth" of where our subject is.
#pragma once
#include "MainStructs.generated.h"


USTRUCT(BlueprintType)
struct FSubjectPosition
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector Value;

	UPROPERTY(BlueprintReadWrite)
	int32 Total;

	UPROPERTY(BlueprintReadWrite)
	FString DisplayName;

	UPROPERTY(BlueprintReadWrite)
	FColor Color;
};

// Here we have a struct that will house our move target
USTRUCT(BlueprintType)
struct FDestination
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector Value; // This doesn't need to be called "Value", it can be named anything.
};

// Any subject that has finished pathing will have this trait
// That will let us find any Subjects that require a new move target.
USTRUCT(BlueprintType)
struct FFinishedMoving
{
	GENERATED_BODY()
	// This struct doesnt need to contain any data.
};
