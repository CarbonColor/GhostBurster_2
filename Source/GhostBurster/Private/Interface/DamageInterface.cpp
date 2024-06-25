// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/DamageInterface.h"

// Add default functionality here for any IDamageInterface functions that are not pure virtual.
void IDamageInterface::RecieveEnemyDamage(int DamageAmount)
{
	GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, TEXT("EnemyDamageInterface Called"));
}

void IDamageInterface::RecievePlayerDamage()
{
	GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, TEXT("PlayerDamageInterface Called"));
}