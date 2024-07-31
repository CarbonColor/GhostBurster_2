// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/DamageInterface.h"
#include "Components/SphereComponent.h"

#include "Enemys.generated.h"

UCLASS(Abstract)
class GHOSTBURSTER_API AEnemys : public AActor, public IDamageInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemys();

	//仮想デストラクタ
	virtual ~AEnemys() {}
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//☆構造体
	//ステータス
	struct FStatus
	{
		float HP = 1;	//ゴーストの体力
	};
	FStatus Status;

	//座標
	struct FPosition
	{
		float GoalX = 0;
		float GoalY = 0;
		float GoalZ = 0;
	};
	FPosition Position;
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

	enum class EnemyColor : uint8
	{
		White = 0,
		Green = 1,
		Red = 2,
		Blue = 3,
	};
	EnemyColor enemyColor = EnemyColor::White;

	//☆変数宣言
	//SceneComponentの変数宣言
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	//StaticMeshComponentの変数宣言
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> GhostMesh;

	//コリジョンの変数宣言
	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> GhostCollision;

	int MoveCount = 0;			//ゴーストの行動制御用のカウント
	int Gamefps = 60;			//ゲームのfps数値を設定
	int AttackUpToTime = 15;	//ゴーストの攻撃までの時間
	int MoveTime = 0;			//ゴーストの移動にかかる時間

	//☆関数宣言
	//Tickでの処理
	virtual void TickProcess() PURE_VIRTUAL(AEnemys::TickProcess, );

	//エネミーの状態判断
	virtual void Think() PURE_VIRTUAL(AEnemys::Think, );

	//状態の更新
	void UpdateState(State nowState);

	//状態に基づいた動きをする
	virtual void Move() PURE_VIRTUAL(AEnemys::Move, );

	//HPが0になったら消滅させる
	void EnemyDead();

	//現在のFPSを取得する
	float GetWorldFPS();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Setter関数
	void SetHP(float HPValue);									//HPの設定用関数
	void SetAttackUpToTime(int SetTime);						//攻撃までの時間設定用関数
	void SetGoalPosition(float SetX, float SetY, float SetZ);	//目標座標の設定用関数
	void SetMoveTime(int SetTime);								//移動時間の設定用

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void SetInitialData(float HP, int AttackUpToTimeValue, float GoalX, float GoalY, float GoalZ, int MoveTimeValue); //生成されたときの設定用関数
};
