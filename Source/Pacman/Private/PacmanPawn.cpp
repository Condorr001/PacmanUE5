// Fill out your copyright notice in the Description page of Project Settings.

#include "PacmanPawn.h"
#include "PacmanActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APacmanPawn::APacmanPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void APacmanPawn::BeginPlay()
{
	SearchForPlayer();
}

// Called to bind functionality to input
void APacmanPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent)
	PlayerInputComponent->BindAction("Up", IE_Pressed, this, &APacmanPawn::UpInput);
	PlayerInputComponent->BindAction("Down", IE_Pressed, this, &APacmanPawn::DownInput);
	PlayerInputComponent->BindAction("Right", IE_Pressed, this, &APacmanPawn::RightInput);
	PlayerInputComponent->BindAction("Left", IE_Pressed, this, &APacmanPawn::LeftInput);
}

void APacmanPawn::UpInput()
{
	SearchForPlayer();
	if (PacmanActor)
	{
		PacmanActor->SetMovement(EDirection::Up);
		PacmanActor->SetActorRotation(FRotator(0, 270, 0));
	}
}

void APacmanPawn::DownInput()
{
	SearchForPlayer();
	if (PacmanActor)
	{
		PacmanActor->SetMovement(EDirection::Down);
		PacmanActor->SetActorRotation(FRotator(0, 90, 0));
	}
}

void APacmanPawn::RightInput()
{
	SearchForPlayer();
	if (PacmanActor)
	{
		PacmanActor->SetMovement(EDirection::Right);
		PacmanActor->SetActorRotation(FRotator(0, 0, 0));
	}
}

void APacmanPawn::LeftInput()
{
	SearchForPlayer();
	if (PacmanActor)
	{
		PacmanActor->SetMovement(EDirection::Left);
		PacmanActor->SetActorRotation(FRotator(0, 180, 180));
	}
}

void APacmanPawn::SearchForPlayer()
{
	if (!PacmanActor)
	{
		TArray<AActor*> Players;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APacmanActor::StaticClass(), Players);
		if (Players.Num() > 0)
		{
			PacmanActor = Cast<APacmanActor>(Players[0]);
		}
	}
}

