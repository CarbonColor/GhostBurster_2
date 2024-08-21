// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/MyGameInstance.h"

void UMyGameInstance::ResetGameData()
{
    EscapeTime = 0.0f;
    GhostsDefeated = 0;
    TotalScore = TEXT("N/A");
}
UMyGameInstance::UMyGameInstance()
{
    DefeatedGhostCount = 0;
}

void UMyGameInstance::IncrementDefeatedGhostCount()
{
    DefeatedGhostCount++;
}