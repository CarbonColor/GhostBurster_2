// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/PlayerScoreInstance.h"
#include "GenericPlatform/GenericPlatformTime.h"

// ------------------------------------------------------------------------------------
// 各データのリセット
// ------------------------------------------------------------------------------------
void UPlayerScoreInstance::AllDataResetFunction()
{
	PlayerScore = 0;
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
int32 UPlayerScoreInstance::GetPlayerScore()
{
	return PlayerScore;
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


// ------------------------------------------------------------------------------------
// 各データのセッター
// ------------------------------------------------------------------------------------
void UPlayerScoreInstance::AddPlayerScore(int32 Value)
{
	PlayerScore += Value;
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
// スコア計算など
// ------------------------------------------------------------------------------------
void UPlayerScoreInstance::ConvertItemToScore_Title()
{
	AddPlayerScore(ItemScore);
	UsePlayerItem();
}

int32 UPlayerScoreInstance::ConvertItemToScore()
{
	UsePlayerItem();
	return ItemScore;
}

TArray<int32> UPlayerScoreInstance::ScoringFunction()
{
	//------------------------------------------------------------
	// リターン用
	//------------------------------------------------------------
	TArray<int32> Data;
	Data.Empty();

	//------------------------------------------------------------
	//デフォルトスコア
	//------------------------------------------------------------
	Data.Add(PlayerScore);

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
	int32 Tmp_TimeScore = TimeMaxScore - (TimeDecreaseScore * Multiple);
	Data.Add(Tmp_TimeScore);

	//------------------------------------------------------------
	//ダメージスコア
	//------------------------------------------------------------
	int32 Tmp_DamageScore = DamageScore * PlayerDamageCount;
	Data.Add(Tmp_DamageScore);

	//------------------------------------------------------------
	//アイテムスコア
	//------------------------------------------------------------
	int32 Tmp_ItemScore = 0;
	while (PlayerItem > 0)
	{
		Tmp_ItemScore += ConvertItemToScore();
	}
	Data.Add(Tmp_ItemScore);


	return Data;
}
