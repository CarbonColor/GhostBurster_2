// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/PlayerScoreInstance.h"

int32 UPlayerScoreInstance::GetPlayerScore()
{
	return PlayerScore;
}

void UPlayerScoreInstance::ResetPlayerScore()
{
	PlayerScore = 0;
}

void UPlayerScoreInstance::AddPlayerScore(int32 Value)
{
	PlayerScore += Value;
}

int32 UPlayerScoreInstance::GetPlayerDamageCount()
{
	return PlayerDamageCount;
}

void UPlayerScoreInstance::ResetPlayerDamageCount()
{
	PlayerDamageCount = 0;
}

void UPlayerScoreInstance::AddPlayerDamageCount()
{
	PlayerDamageCount++;
}



