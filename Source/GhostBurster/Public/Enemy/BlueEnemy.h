// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemys.h"

#include "BlueEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GHOSTBURSTER_API ABlueEnemy : public AEnemys
{
	GENERATED_BODY()
	
	//☆関数宣言
	//Tickでの処理
	virtual void TickProcess() override;
	
	//エネミーの状態判断
	virtual void Think() override;

	//状態に基づいた動きをする
	virtual void ActProcess() override;

	//移動関係
	virtual bool ProcessJustForFirst_Move() override;	// 状態Move遷移時にのみ行う処理
	virtual bool Move() override;						// 移動処理

	virtual bool Attack() override;	// 攻撃処理

	//出現関係
	virtual bool Appear() override;	// 敵出現処理

protected:
	virtual void BeginPlay() override;

public:
	//コンストラクタ
	ABlueEnemy();

	//Tick
	virtual void Tick(float DeltaTime) override;

	//☆関数宣言
	//ダメージを受ける処理、引数でもらった攻撃力分体力を減らす
	virtual void RecieveEnemyDamage(int DamageAmount, EFlashlight_Color Color) override;

	//アイテムでダメージを受ける処理、引数でもらった数値分体力を減らす
	virtual void RecieveItemDamage(int DamageAmount) override;
};
