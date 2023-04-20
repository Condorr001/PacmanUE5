// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pacman/Public/PacmanGrid.h"
#include "PacmanActor.generated.h"

class USphereComponent;

UCLASS()
class PACMAN_API APacmanActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APacmanActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* PlayerMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionComp = nullptr;

	// Base Speed of the player
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pacman")
	float Speed = 500.0f;

	//Multiplier over the speed from 0-1 (0% - 100%)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pacman")
	float SpeedMultiplier = 1.0f;

	//Threshold to know when we are close enough of the destination
	//Normally is related to the Speed, ifwe have a bigger speed the threshold need to be bigger
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pacman")
	float ThresholdRadius = 40.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Pacman")
	EGridPositions LeftTeleport = EGridPositions::LeftTeleport;

	UPROPERTY(EditDefaultsOnly, Category = "Pacman")
	EGridPositions RightTeleport = EGridPositions::RightTeleport;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetMovement(EDirection NewDirection);
	
	//Called when a actor overlaps something
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	void SetGrid(AGrid* NewGrid) { Grid = NewGrid; }

	//function and variables for the speed boost when Pacman eats a special food
	UFUNCTION()
	void OnSpeedBoostTimerExpired();

	UPROPERTY(EditAnywhere)
	float SpeedBoostDuration = 3.5f;

	FTimerHandle SpeedBoostTimerHandle;

	//functions and variables for when Pacman eats a ghost
	void OnDeadTimerExpired();

	UPROPERTY(EditAnywhere)
	float DeadDuration = 1.5f;

	FTimerHandle DeadTimerHandle;

	//Pacman invincibility proprerties
	bool PacmanIsInvincible = false;
	bool IsPacmanInvincible();

private:
	//Define the direction of movement of the player in X,Y. 
	// X => 1 : Right| -1 : Left
	// Y => 1 : Up | -1 : Down
	//Start Right
	FVector MovementDirection = FVector(1.0f, 0.0f, 0.0f);

	//Set to tru when we can't move
	bool bStopMovement = false;

	EDirection CurrentDirection = EDirection::Right;

	AGrid* Grid = nullptr;

	//Player Destination, normally a center of a Tile
	FVector Destination = FVector::ZeroVector;
};
