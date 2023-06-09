// Fill out your copyright notice in the Description page of Project Settings.

#include "Point.h"
#include "PacmanGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"

// Sets default values
APoint::APoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PointMesh"));
	
}

void APoint::Eat()
{
	//Notify the game mode the number of points gained and if this point is enabled
	if (APacmanGameModeBase* GameMode = Cast<APacmanGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		UGameplayStatics::PlaySound2D(this, pacman_food);
		GameMode->PointEat(bIsSpecial);
		Destroy();
	}
}

void APoint::SetSpecial(bool bSpecial)
{
	bIsSpecial = bSpecial;
	if (bIsSpecial)
	{
		SetActorRelativeScale3D(FVector(SpecialScale));
	}
}

bool APoint::GetSpecial()
{
	return bIsSpecial;
}

// Called when the game starts or when spawned
void APoint::BeginPlay()
{
	Super::BeginPlay();
	SetSpecial(bIsSpecial);
}
