// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/PlayerScoreInstance.h"
#include "GenericPlatform/GenericPlatformTime.h"

// ------------------------------------------------------------------------------------
// コンストラクタ
// ------------------------------------------------------------------------------------
UPlayerScoreInstance::UPlayerScoreInstance()
{
	FingerBorder = { 0, 0, 0, 0, 0 };
	NormalFinger = { 0, 0, 0, 0, 0 };
	FoxFinger = { 5000, 0, 5000, 5000, 0 };
	GunFinger = { 0, 0, 5000, 5000, 5000 };
}

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
	PlayerItem = 4;
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
void UPlayerScoreInstance::SetDevicePortNumber(const int32 Port)
{
	DevicePortNumber = Port;
}

void UPlayerScoreInstance::SetFingerBorder()
{
	int32 Thumb = 0, Index = 0, Middle = 0, Ring = 0, Little = 0;

	/*
	* 曲げる指＝（曲げ指の最小値 + 伸び指の最大値）/ 2
	* 曲げない指＝（伸び指の最大値）+ 100〜200
	*/

	//親指のボーダー決め
	if (NormalFinger[0] > GunFinger[0])
	{
		//通常時の方が値が大きい場合
		Thumb = (FoxFinger[0] + NormalFinger[0]) / 2;
	}
	else
	{
		//銃の方が値が大きい場合
		Thumb = (FoxFinger[0] + GunFinger[0]) / 2;
	}

	//人差し指のボーダー決め
	Index = FMath::Max3(NormalFinger[1], FoxFinger[1], GunFinger[1]) + 150;

	//中指のボーダー決め
	if (FoxFinger[2] < GunFinger[2])
	{
		//狐の方が値が小さい時
		Middle = (FoxFinger[2] + NormalFinger[2]) / 2;
	}
	else
	{
		//銃の方が値が小さい時
		Middle = (GunFinger[2] + NormalFinger[2]) / 2;
	}

	//薬指のボーダー決め
	if (FoxFinger[3] < GunFinger[3])
	{
		//狐の方が値が小さい時
		Ring = (FoxFinger[3] + NormalFinger[3]) / 2;
	}
	else
	{
		//銃の方が値が小さい時
		Ring = (GunFinger[3] + NormalFinger[3]) / 2;
	}

	//小指のボーダー決め
	if (NormalFinger[4] > FoxFinger[4])
	{
		//通常時の方が値が大きい場合
		Little = (GunFinger[4] + NormalFinger[4]) / 2;
	}
	else
	{
		//銃の方が値が大きい場合
		Little = (GunFinger[4] + FoxFinger[4]) / 2;
	}

	//ボーダーのセット
	FingerBorder[0] = Thumb;
	FingerBorder[1] = Index;
	FingerBorder[2] = Middle;
	FingerBorder[3] = Ring;
	FingerBorder[4] = Little;
}

void UPlayerScoreInstance::CheckNormalFinger(const int32 Thumb, const int32 Index, const int32 Middle, const int32 Ring, const int32 Little)
{
	//親指：伸びる指の最大値を設定
	if (NormalFinger[0] < Thumb)
	{
		NormalFinger[0] = Thumb;
	}
	//人差し指：伸びる指の最大値を設定
	if (NormalFinger[1] < Index)
	{
		NormalFinger[1] = Index;
	}
	//中指：伸びる指の最大値を設定
	if (NormalFinger[2] < Middle)
	{
		NormalFinger[2] = Middle;
	}
	//薬指：伸びる指の最大値を設定
	if (NormalFinger[3] < Ring)
	{
		NormalFinger[3] = Ring;
	}
	//小指：伸びる指の最大値を設定
	if (NormalFinger[4] < Little)
	{
		NormalFinger[4] = Little;
	}
}

void UPlayerScoreInstance::CheckFoxFinger(const int32 Thumb, const int32 Index, const int32 Middle, const int32 Ring, const int32 Little)
{
	//親指：曲げる指の最小値を設定
	if (FoxFinger[0] > Thumb)
	{
		FoxFinger[0] = Thumb;
	}
	//人差し指：伸びる指の最大値を設定
	if (FoxFinger[1] < Index)
	{
		FoxFinger[1] = Index;
	}
	//中指：曲げる指の最小値を設定
	if (FoxFinger[2] > Middle)
	{
		FoxFinger[2] = Middle;
	}
	//薬指：曲げる指の最小値を設定
	if (FoxFinger[3] > Ring)
	{
		FoxFinger[3] = Ring;
	}
	//小指：伸びる指の最大値を設定
	if (FoxFinger[4] < Little)
	{
		FoxFinger[4] = Little;
	}
}

void UPlayerScoreInstance::CheckGunFinger(const int32 Thumb, const int32 Index, const int32 Middle, const int32 Ring, const int32 Little)
{
	//親指：伸びる指の最大値を設定
	if (GunFinger[0] < Thumb)
	{
		GunFinger[0] = Thumb;
	}
	//人差し指：伸びる指の最大値を設定
	if (GunFinger[1] < Index)
	{
		GunFinger[1] = Index;
	}
	//中指：曲げる指の最小値を設定
	if (GunFinger[2] > Middle)
	{
		GunFinger[2] = Middle;
	}
	//薬指：曲げる指の最小値を設定
	if (GunFinger[3] > Ring)
	{
		GunFinger[3] = Ring;
	}
	//小指：曲げる指の最小値を設定
	if (GunFinger[4] > Little)
	{
		GunFinger[4] = Little;
	}
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
	int32 Multiple = 0;
	if (ClearTime > 0)
	{
		Multiple = (ClearTime / DecreasePer) + 1;
	}
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
