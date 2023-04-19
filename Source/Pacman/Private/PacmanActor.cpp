// Fill out your copyright notice in the Description page of Project Settings.

#include "PacmanActor.h"
#include "Components/SphereComponent.h"
#include "Point.h"
#include "Phantom.h"
#include "Components/StaticMeshComponent.h"
#include <Kismet/GameplayStatics.h>
#include "DrawDebugHelpers.h"



// Sets default values
APacmanActor::APacmanActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMesh"));
	PlayerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Set as root component
	RootComponent = PlayerMesh;

	//// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(1.5f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void APacmanActor::BeginPlay()
{
	Super::BeginPlay();
	//Listen for overlaps
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &APacmanActor::OnOverlap);
}

// Called every frame
void APacmanActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bStopMovement)
	{
		bool bCanMove = Grid->CalculateNewDestination(CurrentDirection, Destination);
		//If the difference between the destination and the current position is less than 1, we are on aour destination and we can stop, 
		//If not, we can continue moving
		FVector NextPosition = GetActorLocation() + (MovementDirection * (Speed * SpeedMultiplier * DeltaTime));
		NextPosition.Z = 0.0f;
		FVector Diff = (Destination - NextPosition) * MovementDirection;
		if (!bCanMove && (FMath::Abs(Diff.Size()) < ThresholdRadius))
		{
			//Stop movement, we are where we want
			SetActorLocation(Destination, true);
			bStopMovement = true;
		}
		else
		{
			//Move component following the movement direction
			SetActorLocation(NextPosition, true);
		}
	}
}

void APacmanActor::SetMovement(EDirection NewDirection)
{
	if (NewDirection != CurrentDirection && Grid && Grid->CanPlayerMoveInDir(NewDirection))
	{
		bStopMovement = false;
		CurrentDirection = NewDirection;
		MovementDirection = Grid->GetVectorDirection(CurrentDirection);
	}
}

bool APacmanActor::IsPacmanInvincible()
{
	return PacmanIsInvincible;
}

void APacmanActor::OnSpeedBoostTimerExpired()
{
	SpeedMultiplier = 1.0f;
	PacmanIsInvincible = false;

	TArray<AActor*> Phantoms;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APhantom::StaticClass(), Phantoms);
	for (AActor* Actor : Phantoms)
	{
		APhantom* Phantom = Cast<APhantom>(Actor);
		if (Phantom)
		{
			Phantom->ChangeState(EState::Scatter);
			//same thing, changing the direction triggers something in one random ghosts and it stops behaving as it's supposed to
			//Phantom->ChangeDirection();
		}
	}
}

void APacmanActor::OnDeadTimerExpired()
{
	TArray<AActor*> Phantoms;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APhantom::StaticClass(), Phantoms);
	for (AActor* Actor : Phantoms)
	{
		APhantom* Phantom = Cast<APhantom>(Actor);
		if (Phantom->GetState() == EState::Dead)
		{
			Phantom->ChangeState(EState::Scatter);
		}
	}
}

void APacmanActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(APoint::StaticClass()))
	{
		APoint* Point = Cast<APoint>(OtherActor);
		Point->Eat();
		if (Point->GetSpecial()) {
			SpeedMultiplier = 2.0f;
			PacmanIsInvincible = true;

			TArray<AActor*> Phantoms;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APhantom::StaticClass(), Phantoms);
			for (AActor* Actor : Phantoms)
			{
				APhantom* Phantom = Cast<APhantom>(Actor);
				if (Phantom)
				{
					Phantom->ChangeState(EState::Frightened);

					//same thing, changing the direction triggers something in one random ghosts and it stops behaving as it's supposed to
					//Phantom->ChangeDirection();
				}
			}

			GetWorld()->GetTimerManager().SetTimer(SpeedBoostTimerHandle, this, &APacmanActor::OnSpeedBoostTimerExpired, SpeedBoostDuration, false);
		}
	}
	else if (OtherActor->IsA(APhantom::StaticClass()))
	{
		APhantom* Phantom = Cast<APhantom>(OtherActor);
		if (PacmanIsInvincible)
		{
			//Phantom->Destroy();
	
			Phantom->ChangeState(EState::Dead);

			GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, this, &APacmanActor::OnDeadTimerExpired, DeadDuration, false);
		}

	}
	//else
	//{
	//	bStopMovement = true;
	//	//Move Slighly back the capsule in the inverse direction to capture new overlaps
	//	SetActorLocation(GetActorLocation() + (MovementDirection * -2.0f), true);
	//}

}


