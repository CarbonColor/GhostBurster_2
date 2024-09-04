// Fill out your copyright notice in the Description page of Project Settings.


#include "Title/TitleEnemy.h"
#include "Title/TitleEventManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATitleEnemy::ATitleEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATitleEnemy::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	HP = 60 * 1.5f;
	RunningTime = 0;
	InitialLocation = GetActorLocation();

}

// Called every frame
void ATitleEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATitleEnemy::EnemyDead()
{
	ATitleEventManager* EventManager = Cast<ATitleEventManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATitleEventManager::StaticClass()));
	if (EventManager)
	{
		EventManager->EnemyDeadFunction();
		Player->AddScore(100);
	}

	this->Destroy();

	return;
}