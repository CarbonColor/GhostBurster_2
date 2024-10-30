// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SettingPlayer.h"

// Sets default values
ASettingPlayer::ASettingPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASettingPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASettingPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASettingPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

