// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/DamageInterface.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
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
		FStatus() : HP(1) {}

		int HP;	//ゴーストの体力
	};

	//☆列挙型
	//敵の状態
	enum class EState
	{
		Wait,	//待機
		Move,	//移動
		Attack,	//攻撃
		Die,	//死亡
		Appear,	//出現
	};

	enum class EEnemyColor : uint8
	{
		White = 0,
		Green = 1,
		Red = 2,
		Blue = 3,
	};

	//☆変数
	//敵の行動制御用のカウント
	int MoveCount;

	//ゲームのfps数値を設定
	float Gamefps;

	//敵が状態遷移したときに最初に行う処理を行ったらtrue
	bool bShouldBeenProcessWhenFirstStateTransition;

	//☆関数
	//Tickでの処理
	virtual void TickProcess() PURE_VIRTUAL(AEnemys::TickProcess, );

	//エネミーの状態判断
	virtual void Think() PURE_VIRTUAL(AEnemys::Think, );

	//状態の更新
	void UpdateState(EState NowState);

	//状態に基づいた動きをする
	virtual void ActProcess() PURE_VIRTUAL(AEnemys::ActProcess, );

	//現在のFPSを取得する
	float GetWorldFPS();

	//構造体変数-------------------------------------------------------------------------------------------------------------------
	FStatus Status;

	//列挙型変数-------------------------------------------------------------------------------------------------------------------
	EState State;
	EEnemyColor EnemyColor;

	//コンポーネント関係-----------------------------------------------------------------------------------------------------------
	//☆変数
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent>				DefaultSceneRoot;	// シーンコンポーネント
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent>		GhostMesh;			// スタティックメッシュコンポーネント
	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent>			GhostCollision;		// コリジョン
	TObjectPtr<UMaterialInstanceDynamic>	DynamicMaterial;	// ダイナミックマテリアル
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent>			SplineComponent;	// スプラインコンポーネント

	//サウンド関係-----------------------------------------------------------------------------------------------------------------
	//☆変数
	TObjectPtr<USoundCue> AppearSound;		// 敵出現時の音
	TObjectPtr<USoundCue> DisappearSound;	// 敵消滅時の音 

	//☆関数
	void PlayAppearSound();					// 敵出現時の音を鳴らす
	void PlayDisappearSound();				// 敵消滅時の音を鳴らす

	//移動関係---------------------------------------------------------------------------------------------------------------------
	//☆変数
	float	MoveTime;			// ゴーストの移動にかかる時間(秒)
	FVector CurrentLocation;	// 敵の現在の座標
	FVector GoalLocation;		// 敵の移動先座標(ローカル座標)
	FVector GoalLocation_World;	// 敵の移動先座標(ワールド座標)
	bool	bHasEndedMoving;	// 移動が終了したか
	FVector Direction;			// GoalLocationへ向かう単位ベクトル
	float	TotalDistance;		// 開始位置から目的地までの直線距離
	float	TraveledDistance;	// これまでに進んだ距離
	float	Amplitude;			// 振幅
	float	Frequency;			// 波の速さ
	float	Speed;				// 目的地までの移動速度

	//☆関数
	virtual void ProcessJustForFirst_Move() PURE_VIRTUAL(AEnemys::ProcessJustForFirst_Move, );	// 状態Move遷移時にのみ行う処理
	virtual bool Move() PURE_VIRTUAL(AEnemys::Move, return false;);											// 移動処理

	//攻撃関係---------------------------------------------------------------------------------------------------------------------
	//☆変数
	bool	bHasEndedAttack;											// 攻撃が終了したか
	float	AttackUpToTime;												// ゴーストの攻撃までの時間(秒)

	//☆関数
	virtual bool Attack() PURE_VIRTUAL(AEnemys::Move, return false;);	// 攻撃処理

	//死亡関係---------------------------------------------------------------------------------------------------------------------
	void EnemyDead();	// HPが0になったら消滅させる

	//出現関係---------------------------------------------------------------------------------------------------------------------
	//☆変数 
	bool	bHasEndedAppear;	// 出現が終了したか
	float	OpacityValue;		// オパシティの値
	int		TimeSpentInAppear;	// 出現するのにかかる時間

	//☆関数
	virtual void ProcessJustForFirst_Appear() PURE_VIRTUAL(AEnemys::ProcessJustForFirst_Appear, );	// 状態：Appearで最初に一度だけする処理
	virtual bool Appear() PURE_VIRTUAL(AEnemys::Appear, return false;);								// 敵出現処理

	//移動関係(Spline使用)---------------------------------------------------------------------------------------------------------
	float			NowEnemyLocation;		// 現在敵がいるスプライン上の位置
	float			SplineLength;			// スプラインの全長
	int				CountGotInMoveState;	// 移動状態になった回数のカウント(何回目の移動か)
	TArray<FVector> GoalLocations;			// 敵の移動先座標(ローカル座標)

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Setter関数-------------------------------------------------------------------------------------------------------------------
	void SetHP(const int HPValue);								// HPの設定用関数
	void SetAttackUpToTime(const float SetTime);				// 攻撃までの時間設定用関数
	void SetGoalLocations(const TArray<FVector>& SetLocations);	// 目標座標の設定用関数
	void SetMoveTime(const float SetTime);						// 移動時間の設定用
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void SetInitialData(const int HP, const float AttackUpToTimeValue, const TArray<FVector>& SetLocations, const float MoveTimeValue);	// 生成されたときの設定用関数
	virtual bool CheckPlayerLightColor(EFlashlight_Color PlayerColor) const PURE_VIRTUAL(AEnemys::GetEnemyColor, return false;);		// プレイヤーのライトの色と敵のライトの色をチェックする関数
};