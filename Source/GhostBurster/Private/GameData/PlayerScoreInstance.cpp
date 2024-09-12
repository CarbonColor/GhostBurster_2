// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/PlayerScoreInstance.h"
#include "GenericPlatform/GenericPlatformTime.h"

// ------------------------------------------------------------------------------------
// 各データのリセット
// ------------------------------------------------------------------------------------
void UPlayerScoreInstance::AllDataResetFunction()
{
	PlayerDafaultScore = 0;
	PlayerDamageScore = 0;
	PlayerTimeScore = 0;
	PlayerItemScore = 0;
	TotalScore = 0;

	PlayerDamageCount = 0;
	StartTime = 0;
	EndTime = 0;
	Minute = 0;
	Second = 0;
	PlayerItem = 3;
}

// ------------------------------------------------------------------------------------
// 各データのゲッター
// ------------------------------------------------------------------------------------
int32 UPlayerScoreInstance::GetPlayerDefaultScore()
{
	return PlayerDafaultScore;
}
int32 UPlayerScoreInstance::GetPlayerTimeScore()
{
	return PlayerTimeScore;
}
int32 UPlayerScoreInstance::GetPlayerDamageScore()
{
	return PlayerDamageScore;
}
int32 UPlayerScoreInstance::GetPlayerItemScore()
{
	return PlayerItemScore;
}
int32 UPlayerScoreInstance::GetTotalScore()
{
	return TotalScore;
}

int32 UPlayerScoreInstance::GetPlayerDamageCount()
{
	return PlayerDamageCount;
}

int32 UPlayerScoreInstance::GetPlayerItemCount()
{
	return PlayerItem;
}

int32 UPlayerScoreInstance::GetGameTime_Minute()
{
	return Minute;
}

int32 UPlayerScoreInstance::GetGameTime_Second()
{
	return Second;
}

int32 UPlayerScoreInstance::GetItemPerScore()
{
	return ItemScore;
}

// ------------------------------------------------------------------------------------
// 各データのセッター
// ------------------------------------------------------------------------------------
void UPlayerScoreInstance::SetDevicePortNumber(int32 Port)
{
	DevicePortNumber = Port;
}

void UPlayerScoreInstance::AddPlayerScore(int32 Value)
{
	PlayerDafaultScore += Value;
}

void UPlayerScoreInstance::AddPlayerDamageCount()
{
	PlayerDamageCount++;
}

void UPlayerScoreInstance::AddPlayerItem()
{
	PlayerItem++;
}

void UPlayerScoreInstance::UsePlayerItem()
{
	PlayerItem--;
}

void UPlayerScoreInstance::SetGameStartTime()
{
	StartTime = FPlatformTime::Seconds();
}

void UPlayerScoreInstance::SetGameEndTime()
{
	EndTime = FPlatformTime::Seconds();
}

// ------------------------------------------------------------------------------------
// スコア計算
// ------------------------------------------------------------------------------------
void UPlayerScoreInstance::ScoringFunction()
{
	//------------------------------------------------------------
	//クリアタイムスコア
	//------------------------------------------------------------
	int32 ClearTime = FMath::FloorToInt(EndTime - StartTime);
	Minute = ClearTime / 60;
	Second = ClearTime % 60;
	//最速クリアタイム分を減らす
	ClearTime -= DefaultClearTime;
	//減点倍率を決める
	int32 Multiple = ClearTime / DecreasePer;
	//スコア計算をする
	PlayerTimeScore = TimeMaxScore - (TimeDecreaseScore * Multiple);

	//------------------------------------------------------------
	//ダメージスコア
	//------------------------------------------------------------
	PlayerDamageScore = DamageScore * PlayerDamageCount;

	//------------------------------------------------------------
	//アイテムスコア
	//------------------------------------------------------------
	PlayerItemScore = 0;
	for (int i = 0; i < PlayerItem; ++i)
	{
		PlayerItemScore += ItemScore;
	}


	//------------------------------------------------------------
	//合計スコア
	//------------------------------------------------------------
	TotalScore = PlayerDafaultScore + PlayerTimeScore + PlayerDamageScore + PlayerItemScore;

}
