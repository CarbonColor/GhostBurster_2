// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PlayerScoreInstance.generated.h"

//------------------------------------------------------------
// プレイヤーのスコアを格納するインスタンス
//------------------------------------------------------------

UCLASS()
class GHOSTBURSTER_API UPlayerScoreInstance : public UGameInstance
{
	GENERATED_BODY()

private:

	UPlayerScoreInstance();

	// ------------------------------------------------------------------------------------	
	//ダメージのスコア
	const int32 DamageScore = -100;
	//タイムのスコア
	const int32 DefaultClearTime = 180;
	const int32 TimeMaxScore = 2000;
	const int32 TimeDecreaseScore = 100;
	const int32 DecreasePer = 30;
	//アイテムのスコア
	const int32 ItemScore = 300;
	// ------------------------------------------------------------------------------------

	// プレイヤーのデフォルトスコア
	UPROPERTY()
		int32 PlayerDafaultScore;

	// プレイヤーのクリアタイム
	UPROPERTY()
		double StartTime;
	UPROPERTY()
		double EndTime;
	UPROPERTY()
		int32 Minute;
	UPROPERTY()
		int32 Second;
	// クリアタイムスコア
	UPROPERTY()
		int32 PlayerTimeScore;

	// ダメージカウント
	UPROPERTY()
		int32 PlayerDamageCount;
	// ダメージスコア
	UPROPERTY()
		int32 PlayerDamageScore;

	// ゲーム終了後のアイテム数
	UPROPERTY()
		int32 PlayerItem;
	// アイテムスコア
	UPROPERTY()
		int32 PlayerItemScore;

	//合計スコア
	UPROPERTY()
		int32 TotalScore;

protected:

public:

	//全て初期化するメソッド
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		void AllDataResetFunction();

	//グローブデバイス用のCOMポート番号
	UPROPERTY(BlueprintReadOnly, Category = "DeviceData")
		int32 DevicePortNumber = 0;
	//指の曲げ具合のボーダー
	UPROPERTY(BlueprintReadOnly, Category = "DeviceData")
		TArray<int32> FingerBorder;


	//デバイス関係のセッター
	UFUNCTION(BlueprintCallable, Category = "DeviceData")
		void SetDevicePortNumber(const int32 Port);
	UFUNCTION(BlueprintCallable, Category = "DeviceData")
		void SetFingerBorder(const TArray<int32> NormalFinger, const TArray<int32> FoxFinger, const TArray<int32> GunFinger);


	// スコアを取得するメソッド
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetPlayerDefaultScore();
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetPlayerTimeScore();
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetPlayerDamageScore();
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetPlayerItemScore();
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetTotalScore();

	// ダメージカウントを取得するメソッド
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetPlayerDamageCount();
	// アイテム数を取得するメソッド
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetPlayerItemCount();
	// クリアタイム（分）を取得するメソッド
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetGameTime_Minute();
	// クリアタイム（秒）を取得するメソッド
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetGameTime_Second();
	// アイテムスコアを取得するメソッド
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetItemPerScore();

	// スコアを加算するメソッド
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		void AddPlayerScore(int32 Value);
	// ダメージカウントを加算するメソッド
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		void AddPlayerDamageCount();
	// アイテム数を加算するメソッド
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		void AddPlayerItem();
	// アイテム数を減算するメソッド
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		void UsePlayerItem();


	// ゲームスタートの時間をセットする
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		void SetGameStartTime();
	// ゲームエンドの時間をセットする
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		void SetGameEndTime();

	// スコアを計算して、その各スコアを送るメソッド
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		void ScoringFunction();

};
