// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemys.generated.h"

UCLASS()
class GHOSTBURSTER_API AEnemys : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemys();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//☆構造体
	//ステータス
	struct Status
	{
		float HP = 1;	//ゴーストの体力
	};
	Status status;

	//☆列挙型
	//敵の状態
	enum class State
	{
		Stand,
		Move,
		Attack,
		Die,
	};
	State state = State::Stand;

	//敵の色情報
	enum class EnemyColor : uint8
	{
		White = 0,
		Green = 1,
		Red	  = 2,
		Blue  = 3,
	};
	EnemyColor EColor = EnemyColor::White;

	//☆変数宣言
	int MoveCount = 0;	//ゴーストの行動制御用のカウント
	int Gamefps = 60;	//ゲームのfps数値を設定

	//☆関数宣言
	//エネミーの状態判断
	virtual void Think() PURE_VIRTUAL(AEnemys::Think, );

	//状態の更新
	void UpdateState(State nowState);

	//状態に基づいた動きをする
	virtual void Move() PURE_VIRTUAL(AEnemys::Move, );

	//HPが0になったら消滅させる
	void EnemyDead();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//☆関数宣言
	//ダメージを受ける処理、引数でもらった攻撃力分体力を減らす
	virtual void Damage(float damage) PURE_VIRTUAL(AEnemys::Damage, );
};
