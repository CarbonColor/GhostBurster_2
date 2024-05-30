// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FlashLight.h"

// Sets default values
AFlashLight::AFlashLight()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFlashLight::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFlashLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFlashLight::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

