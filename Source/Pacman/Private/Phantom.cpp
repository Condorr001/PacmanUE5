// Fill out your copyright notice in the Description page of Project Settings.

#include "Phantom.h"
#include "PacmanActor.h"
#include "PacmanGameModeBase.h"
#include "Components/SphereComponent.h"
#include <Kismet/GameplayStatics.h>
#include "DrawDebugHelpers.h"

// Sets default values
APhantom::APhantom()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PhantomMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PhantomMesh"));
	PhantomMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Set as root component
	RootComponent = PhantomMesh;

	//// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(1.5f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APhantom::BeginPlay()
{
	Super::BeginPlay();
	//Listen for overlaps
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &APhantom::OnOverlap);

	//start scatter-chase alternation
	GetWorld()->GetTimerManager().SetTimer(ScatterTimerHandle, this, &APhantom::OnScatterTimerExpired, ScatterDuration, false);
}

// Called every frame
void APhantom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Grid)
	{
		//If we arrive close enough to the destination, move to position and swap directions (if needed)
		
		int XTileActor, YTileActor = 0;
		Grid->GetTileFromWorld(GetActorLocation(), XTileActor, YTileActor);
		
		if (XTileActor == XTileDestination && YTileActor == YTileDestination)
		{
			//Move actor to destination
			SetActorLocation(Destination);

			//Teleport managing
			if (Grid->GetTileValue(XTileActor, YTileActor) == 'T')
			{
				if (YTileActor == 0)
					SetActorLocation(Grid->GetGridSpecialPosition(RightTeleport));
				else
					SetActorLocation(Grid->GetGridSpecialPosition(LeftTeleport));
			}

			//Get next destination and movement
			Destination = Grid->GetNextDestination( GetTargetPosition(), GetActorLocation(), MovementDir, NextMovementDir);
			Grid->GetTileFromWorld(Destination, XTileDestination, YTileDestination);

			//Swap Directions
			MovementDir = NextMovementDir;
		}

		//Move
		FVector NextPosition = GetActorLocation() + (MovementDir * (Speed * SpeedMultiplier * DeltaTime));
		//never moving in the z axis
		NextPosition.Z = 0;
		SetActorLocation(NextPosition, true);
	}

	if (GetState() == EState::Dead)
	{
		int XTileDead = 0, YTileDead = 0;
		Grid->GetTileFromWorld(GetActorLocation(), XTileDead, YTileDead);

		if (XTileDead == 11 && YTileDead == 13)
			ChangeState(EState::Chase);
	}
}

void APhantom::SetGrid(AGrid * NewGrid)
{
	Grid = NewGrid;
	Destination = Grid->GetNextDestination(GetTargetPosition(), GetActorLocation(), MovementDir, NextMovementDir);
	Grid->GetTileFromWorld(Destination, XTileDestination, YTileDestination);
}

EState APhantom::GetState()
{
	return CurrentState;
}

void APhantom::ChangeState(EState NewState)
{
	if (NewState != CurrentState)
	{
		//Edit: for unknown reasons the "MovementDir *= -1.0f" causes the Frightened state to work unpropriately when called
		//Change Direction
		//MovementDir *= -1.0f;
		CurrentState = NewState;
	}
}

void APhantom::SetSpeedMultiplier(float Value)
{
	SpeedMultiplier = Value;
}

void APhantom::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(APacmanActor::StaticClass())) {
		APacmanActor* Pacman = Cast<APacmanActor>(OtherActor);

		if (!Pacman->IsPacmanInvincible()) {
			APacmanGameModeBase* GameMode = Cast<APacmanGameModeBase>(GetWorld()->GetAuthGameMode());
			GameMode->Lives--;
		}
	}
}

//does not work as expected, one or more ghosts start "floating" around the map instead of changing their direction
void APhantom::ChangeDirection()
{
	if (Grid)
	{
		MovementDir *= -1.0f;
		Destination = Grid->GetNextDestination(GetTargetPosition(), GetActorLocation(), MovementDir, NextMovementDir);
		Grid->GetTileFromWorld(Destination, XTileDestination, YTileDestination);
	}
}

//Scatter and Chase alternation
void APhantom::OnScatterTimerExpired()
{
	ChangeState(EState::Chase);
}


//different target for different states (Pacman for Scatter state, ghosts house for Frightened state)
FVector APhantom::GetTargetPosition()
{
	switch (CurrentState)
	{
	case EState::Chase:
	{
		SetSpeedMultiplier(1.0f);
		return Grid->GetGridSpecialPosition(EGridPositions::PlayerPosition);
		break;
	}
	case EState::Scatter:
	{
		//DrawDebugSphere(GetWorld(), Grid->GetGridSpecialPosition(ScatterPosition), 20.0f, 32, FColor::Green, false, 1.0f);
		SetSpeedMultiplier(1.0f);
		return Grid->GetGridSpecialPosition(ScatterPosition);
		break;
	}
	case EState::Frightened:
	{
		if (Grid)
		{
			SetSpeedMultiplier(0.7f);
			return Grid->GetGridSpecialPosition(GhostsHouse);
		}
		break;
	}
	case EState::Dead:
	{
		if (Grid)
		{
			SetSpeedMultiplier(3.0f);
			return Grid->GetGridSpecialPosition(GhostsHouse);
		}
		break;
	}
	default:
		break;
	}
	return FVector();
}
