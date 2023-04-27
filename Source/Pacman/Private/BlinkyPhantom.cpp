// Fill out your copyright notice in the Description page of Project Settings.

#include "BlinkyPhantom.h"

void ABlinkyPhantom::BeginPlay()
{
	Super::BeginPlay();
	ChangeScatterPosition(EGridPositions::TopRight);
}
