// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/DamageInterface.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

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
		int HP = 1;	//ゴーストの体力
	};
	FStatus Status;

	//☆列挙型
	//敵の状態
	enum class State
	{
		Wait,	//待機
		Move,	//移動
		Attack,	//攻撃
		Die,	//死亡
		Appear,	//出現
	};
	State state = State::Appear;

	enum class EnemyColor : uint8
	{
		White	= 0,
		Green	= 1,
		Red		= 2,
		Blue	= 3,
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

	//ダイナミックマテリアルの変数宣言
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

	//敵の行動制御用のカウント
	int MoveCount = 0;

	//ゲームのfps数値を設定
	float Gamefps = 60.f;

	//敵が状態遷移したときに最初に行う処理を行ったらtrue
	bool bShouldBeenProcessWhenFirstStateTransition = false;

	//移動関係
	float	MoveTime = 1.f;							// ゴーストの移動にかかる時間(秒)
	FVector CurrentLocation = FVector(0, 0, 0);		// 敵の現在の座標
	FVector GoalLocation = FVector(0, 0, 0);		// 敵の移動先座標(ローカル座標)
	FVector GoalLocation_World = FVector(0, 0, 0);	// 敵の移動先座標(ワールド座標)
	bool	bHasEndedMoving = false;				// 移動が終了したか
	FVector Direction = FVector(0, 0, 0);			// GoalLocationへ向かう単位ベクトル
	float	TotalDistance = 0.f;					// 開始位置から目的地までの直線距離
	float	TraveledDistance = 0.f;					// これまでに進んだ距離
	float	Amplitude = 40.0f;						// 振幅
	float	Frequency = 1.0f;						// 波の速さ
	float	Speed = 80.0f;							// 目的地までの移動速度

	//攻撃関係
	bool	bHasEndedAttack = false;					// 攻撃が終了したか
	float	AttackUpToTime = 0.f;							// ゴーストの攻撃までの時間(秒)

	//☆関数宣言
	//Tickでの処理
	virtual void TickProcess() PURE_VIRTUAL(AEnemys::TickProcess, );

	//エネミーの状態判断
	virtual void Think() PURE_VIRTUAL(AEnemys::Think, );

	//状態の更新
	void UpdateState(State nowState);

	//状態に基づいた動きをする
	virtual void ActProcess() PURE_VIRTUAL(AEnemys::ActProcess, );

	//HPが0になったら消滅させる
	void EnemyDead();

	//現在のFPSを取得する
	float GetWorldFPS();

	//状態：Moveで使う関数
	virtual bool ProcessJustForFirst_Move() PURE_VIRTUAL(AEnemys::ProcessJustForFirst_Move, return false;);	// 状態Move遷移時にのみ行う処理
	virtual bool Move() PURE_VIRTUAL(AEnemys::Move, return false;);											// 移動処理

	//攻撃関係
	//状態：Attackで使う関数
	virtual bool Attack() PURE_VIRTUAL(AEnemys::Move, return false;);										// 攻撃処理

	//出現関係---------------------------------------------------------------------------------------------------------------------
	//☆変数 
	bool	bHasEndedAppear = false;	// 出現が終了したか
	float	OpacityValue = 0.f;			// オパシティの値
	int		TimeSpentInAppear = 1;		// 出現するのにかかる時間

	//☆関数
	virtual void ProcessJustForFirst_Appear() PURE_VIRTUAL(AEnemys::ProcessJustForFirst_Appear, );	// 状態：Appearで最初に一度だけする処理
	virtual bool Appear() PURE_VIRTUAL(AEnemys::Appear, return false;);								// 敵出現処理

	//サウンド関係-----------------------------------------------------------------------------------------------------------------
	//☆変数
	TObjectPtr<USoundCue> AppearSound;		// 敵出現時の音
	TObjectPtr<USoundCue> DisappearSound;	// 敵消滅時の音 
	
	//☆関数
	void PlayAppearSound();					// 敵出現時の音を鳴らす
	void PlayDisappearSound();				// 敵消滅時の音を鳴らす

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Setter関数
	void SetHP(int HPValue);					//HPの設定用関数
	void SetAttackUpToTime(float SetTime);		//攻撃までの時間設定用関数
	void SetGoalLocation(FVector SetLocation);	//目標座標の設定用関数
	void SetMoveTime(float SetTime);			//移動時間の設定用

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void SetInitialData(int HP, float AttackUpToTimeValue, FVector SetLocation, float MoveTimeValue); //生成されたときの設定用関数

	//プレイヤーのライトの色と敵のライトの色をチェックする関数
	virtual bool CheckPlayerLightColor(EFlashlight_Color PlayerColor) const PURE_VIRTUAL(AEnemys::GetEnemyColor, return false;);
};