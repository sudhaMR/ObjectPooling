// Fill out your copyright notice in the Description page of Project Settings.

#include "BulletActor.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Engine/World.h"

// Sets default values
ABulletActor::ABulletActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	lifeTime = 5;
}

// Called when the game starts or when spawned
void ABulletActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABulletActor::Tick(float DeltaTime)
{
	
}

void ABulletActor::BulletHit()
{
	lifeTime--;
	if (lifeTime < 0)
	{
		this->Destroy();
		GetWorld()->ForceGarbageCollection(true);
	}
}

