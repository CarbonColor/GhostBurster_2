// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemys.h"
#include "Components/SphereComponent.h"
#include "BossEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GHOSTBURSTER_API ABossEnemy : public AEnemys
{
	GENERATED_BODY()
	
	//☆列挙型
	//敵の状態
	enum class EBossState
	{
		//待機から遷移する状態--------------------------------------------------------------------------------
		Wait = 0,				//待機(これより上の行に要素を増やさない)
		Charge = 1,				//チャージ
		EnemyCall = 2,				//雑魚敵を呼ぶ
		Teleportation = 3,				//瞬間移動
		MaxStateCountAtTransitionFromWait = 4 UMETA(Hidden),	//待機から遷移する状態の種類の数(数取得用)
		//待機から遷移しない----------------------------------------------------------------------------------
		Attack,					//攻撃
		Stan,					//スタン
		EnemyExpeditionWait,	//通常敵の討伐待機
		AfterEnemyExpedition,	//通常敵の討伐後
		Appear,					//出現
		Die,					//死亡
	};
	
	//☆変数
	//敵の状態
	EBossState BossState;
	
	//ボス部屋でのプレイヤーの位置と回転
	FVector		PlayerLocation_BossRoom;	// ボス部屋でのプレイヤーの位置
	float		PlayerRotation_Z_BossRoom;	// ボス部屋でのプレイヤーの回転

	//何かを終了させるまでのカウント
	int	FinishCount;	// 終了までのカウント、使ったら必ずその関数内でゼロクリアする

	//☆関数
	//Tickでの処理
	virtual void TickProcess() override;
	
	//エネミーの状態判断
	virtual void Think() override;

	//状態の更新
	void UpdateState(EBossState nowState);

	//状態に基づいた動きをする
	virtual void ActProcess() override;

	//1秒カウントしたらtrueを返す
	bool bOneSecondsPassedIs();

	//マテリアル関係------------------------------------------------------------------------------------------
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial_String; // 紐のダイナミックマテリアル
	float								 OpacityValue_String;	 // 紐のオパシティの値
	float								 MaxOpacity_String;		 // 紐のオパシティの最大値(0～1の範囲)

	//アニメーション関係--------------------------------------------------------------------------------------
	//☆変数
	TObjectPtr<UAnimSequence> DeadAnim;				// 死亡状態のアニメーション
	TObjectPtr<UAnimSequence> StanAnim;				// スタン状態のアニメーション
	TObjectPtr<UAnimSequence> SummonAnim;			// 敵呼び時のアニメーション
	TObjectPtr<UAnimSequence> WarpAnim;				// 瞬間移動、敵が消える時のアニメ－ション

	//☆関数
	void ChangeBossAnimation(const EBossState PreState, const EBossState NewState);	// 状態遷移時のアニメーションの変更

	//待機関係------------------------------------------------------------------------------------------------
	//☆変数
	EEnemyColor		ChangingBossColor;										// 次に変化する色
	bool			bHasEndedWait;											// 待機状態を終了するか
	bool			bHasFinishedTransparentize;								// 透明になったか
	bool			bHasFinishedChangeDecidedColor;							// 色が変化したか
	FLinearColor	ColorValue;												// 色の値
	bool			bHasFinishedShow;										// 表示が終わったか
	bool			bIsBattleStarted;										// 戦闘が開始したか

	//☆関数
	void			ProcessJustForFirst_Wait();								// 状態：Wait遷移時にのみ行う処理
	bool			ChangeColor(const EEnemyColor ChangingColor);			// 色の変更処理
	bool			Transparentize(const float DeltaTime);					// 透明にする処理
	bool			ChangeDecidedColor(const EEnemyColor ChangingColor);	// 決めた色に変更する
	bool			Show(const float DeltaTime);							// 表示する処理
	void			BattleStart();											// ボスの戦闘行動を開始させる

	//チャージ関係-------------------------------------------------------------------------------------------
	//☆変数
	float	ChargeTime;						// チャージ時間
	bool	bIsTransitionAttack;			// 攻撃状態に遷移するか
	bool	bIsTransitionStan;				// スタン状態に遷移するか
	int		CountUpToAttackStateTransition;	// 状態：Attackに遷移するまでのカウント
	int		TimeUpToAttackStateTransition;	// 状態：Attackに遷移するまでの時間
	int		ChargeCount;					// 現在のチャージ回数
	int		CountUpToAttack;				// 攻撃までのチャージ回数
	int		StanValue;						// 現在のボスのスタン値(スタン状態の処理でも使用)
	int		MaxStanValue;					// スタン値の最大値(スタン状態の処理でも使用)

	//☆関数
	void	ProcessJustForFirst_Charge();	// 状態：Charge遷移時にのみ行う処理
	void	ChargeAttack();					// 攻撃チャージの処理
	void	Teleportation_Charge();			// 瞬間移動しながらチャージを行う処理

	//攻撃関係----------------------------------------------------------------------------------------------
	//☆関数
	void		 ProcessJustForFirst_Attack();	// 状態：Attack遷移時にのみ行う処理
	virtual bool Attack() override;				// 攻撃処理

	//スタン関係---------------------------------------------------------------------------------------------
	//☆変数
	bool bHasEndedStan;	// スタン状態を終了するか

	//☆関数
	bool Stan();		// スタン処理

	//通常敵呼び関係-----------------------------------------------------------------------------------------
	//☆変数
	TArray<FVector>		NormalEnemyPopLocations;	// 敵の出現場所
	bool				bHasEndedEnemyCall;			// 通常敵呼び状態を終了するか
	int					CallingEnemyNumber;			// 通常敵の生成数
	TArray<EEnemyColor>	CallingEnemyColors;			// 生成する敵の色

	//☆関数
	void				ProcessJustForFirst_EnemyCall();																						// 状態：EnemyCall遷移時にのみ行う処理
	void				DecideCallingEnemyNumber();																								// 通常敵の生成数を決める(2以上4以下の範囲)
	void				DecideEnemyPopLocations(const int CallingEnemyNum);																		// 通常敵の出現位置を決める(ボス敵からの相対位置)
	void				DecideCreateEnemyColor(const int CallingEnemyNum);																		// 何色の敵が生成されるか決める
	bool				EnemyCall();																											// 通常敵呼び処理
	void				CreateEnemies(const int CallingEnemyNum, const TArray<FVector>& PopLocations, const TArray<EEnemyColor>& EnemyColors);	// 通常敵の生成処理

	//通常敵の討伐待機関係-----------------------------------------------------------------------------------
	//☆変数
	int		NowExistsEnemyNumber;												// 現在存在している呼んだ敵の数、一体倒されたら1減る

	//☆関数
public:
	void	ReceiveInfoDeadCalledEnemy();										// 呼んだ敵が消滅したとき、その消滅したという情報を受け取る(NowExistsEnemyNumber--する)

private:
	//通常敵の討伐後関係-------------------------------------------------------------------------------------
	//☆変数
	bool bHasEndedAfterEnemyExpedition;	// 通常敵の討伐後状態の処理を終了するか

	//☆関数
	void AfterEnemyExpeditionProcess();	// 通常敵の討伐後状態で行う処理

	//移動関係-----------------------------------------------------------------------------------------------
	//☆変数 
	TArray<FVector> BossGoalLocations;						// ボスの複数箇所の目的座標
	FVector			BossGoalLocation;						// ボスの目的座標
	bool			bHasEndedTeleportation;					// 瞬間移動が終わったらtrue 

	//☆関数
	virtual void	ProcessJustForFirst_Move() override;	// 状態Move遷移時にのみ行う処理
	virtual bool	Move() override;						// 移動

protected:
	virtual void BeginPlay() override;

public:
	//コンストラクタ
	ABossEnemy();

	//Tick
	virtual void Tick(float DeltaTime) override;

	//☆関数宣言
	//ダメージを受ける処理、引数でもらった攻撃力分体力を減らす
	virtual void RecieveEnemyDamage(int DamageAmount) override;

	//プレイヤーのライトの色と敵のライトの色をチェックする処理
	virtual bool CheckPlayerLightColor(EFlashlight_Color PlayerColor) const override;

	//プレイヤーがボス部屋で止まった時、位置と回転を取得する
	void GetPlayerLocationAndRotation(const FVector PlayerLocation, const float PlayerRotation_Z);
};
