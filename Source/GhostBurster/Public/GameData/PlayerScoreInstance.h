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

	// プレイヤーのスコア
	UPROPERTY()
	int32 PlayerScore;

	// ダメージカウント
	UPROPERTY()
	int32 PlayerDamageCount;

public:

	// スコアを取得するメソッド
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	int32 GetPlayerScore();
	// スコアを初期化するメソッド
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	void ResetPlayerScore();
	// スコアを加算するメソッド
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	void AddPlayerScore(int32 Value);

	// ダメージカウントを取得するメソッド
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	int32 GetPlayerDamageCount();
	// ダメージカウントを初期化するメソッド
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	void ResetPlayerDamageCount();
	// ダメージカウントを加算するメソッド
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	void AddPlayerDamageCount();

};
