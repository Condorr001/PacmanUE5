// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PacmanGrid.h"
#include "Phantom.generated.h"

class AGrid;
class USphereComponent;

UENUM(BlueprintType)
enum class EState : uint8
{
	Chase,
	Scatter,
	Frightened,
	Dead
};

UCLASS()
class PACMAN_API APhantom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APhantom();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* PhantomMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionComp = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Base Speed of the player
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pacman")
	float Speed = 200.0f;

	//Multiplier over the speed from 0-1 (0% - 100%)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pacman")
	float SpeedMultiplier = 1.0f;

	//functions and variables of Scatter-Chase alternation
	UFUNCTION()
	void OnScatterTimerExpired();

	//UFUNCTION()
	//void OnChaseTimerExpired();

	UPROPERTY(EditAnywhere)
	float ScatterDuration = 7.0f;

	UPROPERTY(EditAnywhere)
	float ChaseDuration = 20.0f;

	FTimerHandle ScatterTimerHandle;
	//FTimerHandle ChaseTimerHandle;

	//Position to go when Scatter State. Normally is innaccesible so we move around the area
	UPROPERTY(EditDefaultsOnly, Category = "Pacman")
	EGridPositions ScatterPosition = EGridPositions::TopLeft;

	//Position to go when Frightened State (ghosts house) and when dead
	UPROPERTY(EditDefaultsOnly, Category = "Pacman")
	EGridPositions GhostsHouse = EGridPositions::Middle;

	UPROPERTY(EditDefaultsOnly, Category = "Pacman")
	EGridPositions LeftTeleport = EGridPositions::LeftTeleport;

	UPROPERTY(EditDefaultsOnly, Category = "Pacman")
	EGridPositions RightTeleport = EGridPositions::RightTeleport;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetGrid(AGrid* NewGrid);

	void SetStartDirection(FVector StartDirection) { MovementDir = StartDirection; }

	void ChangeState(EState NewState);

	void SetSpeedMultiplier(float Value);

	EState GetState();

	//Called when a actor overlaps something
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	void ChangeDirection();

protected:

	//Current Phantom State
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EState CurrentState = EState::Scatter;

	//Return a world position (normally a center Tile point) where to go
	virtual FVector GetTargetPosition();

	//Current Moving Dir
	FVector MovementDir = FVector(-1.0f, 0.0f, 0.0f);

	//Next Movement Dir after reaching Destination
	FVector NextMovementDir = FVector(-1.0f, 0.0f, 0.0f);

	//World position to reach
	FVector Destination = FVector::ZeroVector;

	//Grid Object
	AGrid* Grid = nullptr;

	int XTileDestination = 0;
	int YTileDestination = 0;

};
