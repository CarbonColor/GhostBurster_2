// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemys.h"

#include "RedEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GHOSTBURSTER_API ARedEnemy : public AEnemys
{
	GENERATED_BODY()
	
	//☆関数宣言
//エネミーの状態判断
	virtual void Think() override;

	//状態に基づいた動きをする
	virtual void Move() override;

protected:
	virtual void BeginPlay() override;

public:
	//コンストラクタ
	ARedEnemy();

	//Tick
	virtual void Tick(float DeltaTime) override;

	//☆関数宣言
	//ダメージを受ける処理、引数でもらった攻撃力分体力を減らす
	virtual void RecieveEnemyDamage(int DamageAmount, EFlashlight_Color Color) override;
};
