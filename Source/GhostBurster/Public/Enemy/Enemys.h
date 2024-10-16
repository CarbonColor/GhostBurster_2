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
		FStatus() : HP(60), MaxHP(60), Speed(0.f) {}

		int		HP;		// ゴーストの体力
		int		MaxHP;	// ゴーストの最大HP
		float	Speed;	// 目的地までの移動速度
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

	//敵の色
	enum class EEnemyColor : uint8
	{
		White = 0,						//白
		Green = 1,						//緑
		Red = 2,						//赤
		Blue = 3,						//青
		ColorTypeCount UMETA(Hidden),	//色の種類の数
		BossColor,						//ボスの色
	};

	//☆関数
	//Tickでの処理
	virtual void TickProcess() PURE_VIRTUAL(AEnemys::TickProcess, );

	//FPS関係----------------------------------------------------------------------------------------------------------------------
	//☆変数
	float	Gamefps;		// ゲームのfps数値を設定
	int		AssumptionFPS;	// 想定FPS

	//☆関数
	float GetWorldFPS();	// 現在のFPSを取得する

	//構造体変数-------------------------------------------------------------------------------------------------------------------
	FStatus Status;

	//列挙型変数-------------------------------------------------------------------------------------------------------------------
	EState State;
	EEnemyColor EnemyColor;

	//コンポーネント関係-----------------------------------------------------------------------------------------------------------
	//☆変数
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent>				DefaultSceneRoot;		// シーンコンポーネント
	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMeshComponent>		GhostMeshComponent;		// スケルタルメッシュコンポーネント
	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent>			GhostCollision;			// コリジョン
	TObjectPtr<UMaterialInstanceDynamic>	DynamicMaterial_Body;	// 体のダイナミックマテリアル
	TObjectPtr<UMaterialInstanceDynamic>	DynamicMaterial_Eye;	// 目のダイナミックマテリアル
	FVector									EnemyScale;				// 敵のスケール

	//マテリアル関係---------------------------------------------------------------------------------------------------------------
	//☆変数
	float	TimeUpToTransparency;		// 透明になるまでの時間(秒)
	float	EmissiveValueAtDamage;		// ダメージを受けた時の発光の値
	float	EmissiveValueAtDead;		// 死亡時の発光の値

	//☆関数
	bool	Transparentize_Dead();		// 死亡時の徐々に透明にする処理
	void	ChangeEmissiveValue();		// HPの減少に伴い発光させる
	void	ChangeEmissiveValue_Dead();	// 死亡時に発光させる

	//アニメーション関係-----------------------------------------------------------------------------------------------------------
	//☆変数
	TObjectPtr<UAnimSequence>	DefaultAnim;	// 特定のアニメーションを使用しない状態のアニメーション
	TObjectPtr<UAnimSequence>	AttackAnim;		// 攻撃状態のアニメーション
	int							AttackTiming;	// 攻撃のタイミング(フレーム)、アニメーションに合わせて数値を決める

	void ChangeAnimation(const EState PreState, const EState NewState);	// アニメーションの変更

	//サウンド関係-----------------------------------------------------------------------------------------------------------------
	//☆変数
	TObjectPtr<USoundCue> AppearSound;		// 敵出現時の音
	TObjectPtr<USoundCue> DisappearSound;	// 敵消滅時の音 

	//☆関数
	void PlayAppearSound();					// 敵出現時の音を鳴らす
	void PlayDisappearSound();				// 敵消滅時の音を鳴らす

	//状態遷移関係-----------------------------------------------------------------------------------------------------------------
	//☆変数
	int				MoveCount;											// 敵の行動制御用のカウント
	bool			bOnceDoProcessBeenIs;								// 一度だけ行う処理をしたか
	float			TimeFromWaitToStateTransition;						// 待機から状態遷移するまでの時間
	bool			bShouldBeenProcessWhenFirstStateTransition;			// 状態遷移時にのみ行う処理を行ったか判定

	//☆関数
	virtual void	Think() PURE_VIRTUAL(AEnemys::Think, );				// エネミーの状態判断
	void			UpdateState(EState NowState);						// 状態の更新
	virtual void	ActProcess() PURE_VIRTUAL(AEnemys::ActProcess, );	// 状態に基づいた動きをする

	//移動関係---------------------------------------------------------------------------------------------------------------------
	//☆変数
	float			MoveTime;			// ゴーストの移動にかかる時間(秒)
	float			TraveledDistance;	// これまでに進んだ距離
	FVector			CurrentLocation;	// 敵の現在の座標
	TArray<FVector> GoalLocations;		// 敵の移動先座標(ローカル座標、向きは考慮しない)
	int				MovingTimesCount;	// 何回目の移動か、１回目を0とする(移動状態に遷移した回数でカウント)
	FVector			GoalLocation_World;	// 敵の移動先座標(ワールド座標)
	FVector			Direction;			// GoalLocationへ向かう単位ベクトル
	float			TotalDistance;		// 開始位置から目的地までの直線距離
	float			Amplitude;			// 振幅
	float			Frequency;			// 波の速さ
	bool			bHasEndedMoving;	// 移動が終了したか

	//☆関数
	virtual void ProcessJustForFirst_Move() PURE_VIRTUAL(AEnemys::ProcessJustForFirst_Move, );	// 状態Move遷移時にのみ行う処理
	virtual bool Move() PURE_VIRTUAL(AEnemys::Move, return false;);								// 移動処理

	//攻撃関係---------------------------------------------------------------------------------------------------------------------
	//☆変数
	bool	bHasEndedAttack;											// 攻撃が終了したか

	//☆関数
	virtual bool Attack() PURE_VIRTUAL(AEnemys::Move, return false;);	// 攻撃処理

	//死亡関係---------------------------------------------------------------------------------------------------------------------
	//☆変数
	bool	bIsDestroy;					// 徐々に透明にする処理を終了するか
	bool	bIsEscaped;					// 逃走したか

	//☆関数
	void	EnemyDead();				// HPが0になったら消滅させる
	void	ProcessDoOnce_EnemyDead();	// EnemyDeadで一度だけ行う処理

	//出現関係---------------------------------------------------------------------------------------------------------------------
	//☆変数 
	bool	bHasEndedAppear;	// 出現が終了したか
	float	OpacityValue_Body;	// 体のオパシティの値
	float	OpacityValue_Eye;	// 目のオパシティの値
	float	TimeSpentInAppear;	// 出現するのにかかる時間(秒)
	float	MaxOpacity_Body;	// 体のオパシティの最大値(0～1の範囲)
	float	MaxOpacity_Eye;		// 目のオパシティの最大値(0～1の範囲)

	//☆関数
	void ProcessJustForFirst_Appear();	// 状態：Appearで最初に一度だけする処理
	bool Appear();						// 敵出現処理

	//回転関係---------------------------------------------------------------------------------------------------------------------
	//☆変数
	FRotator RotationCorrectionValue;	// 回転の補正値
	
	//☆関数
	void FacePlayerHowTo();				// プレイヤーの方向を向く

	//デバッグ用-------------------------------------------------------------------------------------------------------------------
	int Count = 0;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Setter関数-------------------------------------------------------------------------------------------------------------------
	void SetHP(const int HPValue);								// HPの設定用関数
	void SetTimeFromWaitToStateTransition(const float SetTime);	// 攻撃状態になるまでの時間設定用関数
	void SetGoalLocations(const TArray<FVector>& SetLocations);	// 目標座標の設定用関数
	void SetMoveTime(const float SetTime);						// 移動時間の設定用
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void SetInitialData(const int HP, const float SetTime, const TArray<FVector>& SetLocations, const float MoveTimeValue);			// 生成されたときの設定用関数
	virtual bool CheckPlayerLightColor(EFlashlight_Color PlayerColor) const PURE_VIRTUAL(AEnemys::GetEnemyColor, return false;);	// プレイヤーのライトの色と敵のライトの色をチェックする関数

	//Getter関数-------------------------------------------------------------------------------------------------------------------
	int GetEnemyColor();	// 敵の色情報を取得する
};