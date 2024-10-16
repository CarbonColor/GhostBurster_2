// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/VRPlayerCharacter.h"
#include "Enemy/NormalEnemy.h"
#include "Enemy/GreenEnemy.h"
#include "Enemy/RedEnemy.h"
#include "Enemy/BlueEnemy.h"

ABossEnemy::ABossEnemy()
	:
	BossState(EBossState::Wait),
	PlayerLocation_BossRoom(FVector(0, 0, 0)),
	PlayerRotation_Z_BossRoom(0.f),
	FinishCount(0.f),
	//待機関係
	ChangingBossColor(EEnemyColor::White), bHasEndedWait(false), bHasFinishedTransparentize(false), bHasFinishedChangeDecidedColor(false), ColorValue(FLinearColor(0, 0, 0)), bHasFinishedShow(false),
	bIsBattleStarted(true),
	//チャージ関係
	ChargeTime(0.f), bIsTransitionAttack(false), bIsTransitionStan(false), CountUpToAttackStateTransition(0), TimeUpToAttackStateTransition(1), ChargeCount(0), CountUpToAttack(5), StanValue(0), 
	MaxStanValue(5 * AssumptionFPS),
	//スタン関係
	bHasEndedStan(false),
	//通常敵呼び関係
	bHasEndedEnemyCall(false), CallingEnemyNumber(0),
	//通常敵の討伐待機関係
	NowExistsEnemyNumber(0),
	//通常敵の討伐後関係
	bHasEndedAfterEnemyExpedition(false),
	//移動関係
	BossGoalLocation(FVector(0.f, 0.f, 0.f)), bHasEndedTeleportation(false)
{
	//Tickを有効にする
	PrimaryActorTick.bCanEverTick = true;

	//マテリアルのオパシティの値を最大値にしておく
	this->OpacityValue_Body = 1.f;
	this->OpacityValue_Eye = 1.f;

	//☆シーンコンポーネント
	//シーンコンポーネントの作成
	this->DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	if (this->DefaultSceneRoot)
	{
		//シーンコンポーネントをルートコンポーネントに設定
		RootComponent = this->DefaultSceneRoot;

		//☆スケルタルメッシュコンポーネント------------------------------------------------------------------------------------------
		//スケルタルメッシュコンポーネントの作成
		this->GhostMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Ghost"));
		//スケルタルメッシュをロード
		TObjectPtr<USkeletalMesh> GhostMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/_TeamFolder/CG/CG_Model/Ghost/SKM_TestGhost"));
		if (this->GhostMeshComponent)
		{
			if (GhostMesh)
			{
				//スケルタルメッシュコンポーネントにスケルタルメッシュを設定する
				GhostMeshComponent->SetSkeletalMesh(GhostMesh);
			}
			//スケルタルメッシュコンポーネントをルートコンポーネントにアタッチする
			this->GhostMeshComponent->SetupAttachment(RootComponent);
			//スケルタルメッシュのコリジョンを無くす
			this->GhostMeshComponent->SetCollisionProfileName("NoCollision");

			//☆マテリアル--------------------------------------------------------------------------------------------------------
			//体のマテリアルをロード
			TObjectPtr<UMaterial> BodyMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Game/_TeamFolder/Enemy/M_Boss"));
			if (BodyMaterial) // nullチェック
			{
				//ダイナミックマテリアルインスタンスを作成
				this->DynamicMaterial_Body = UMaterialInstanceDynamic::Create(BodyMaterial, this);

				//GhostMeshにダイナミックマテリアルを設定
				this->GhostMeshComponent->SetMaterial(0, DynamicMaterial_Body);

				//初期オパシティ値を設定
				this->DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
			}
			//目のマテリアルをロード
			TObjectPtr<UMaterial> EyeMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Game/_TeamFolder/CG/CG_Model/Ghost/M_Ghost_Eye"));
			if (EyeMaterial) // nullチェック
			{
				//ダイナミックマテリアルインスタンスを作成
				this->DynamicMaterial_Eye = UMaterialInstanceDynamic::Create(EyeMaterial, this);

				//GhostMeshにダイナミックマテリアルを設定
				this->GhostMeshComponent->SetMaterial(1, DynamicMaterial_Eye);

				//初期オパシティ値を設定
				this->DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);
			}
		}

		//☆コリジョン-----------------------------------------------------------------------------------------------------------------
		//スフィアコリジョンの作成
		GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("GhostCollision"));
		if (GhostCollision)
		{
			//GhostCollisionをルートコンポーネントにアタッチする
			GhostCollision->SetupAttachment(RootComponent);
			//GhostCollisionのコリジョンプリセットをOverlapAllDynamicにする
			GhostCollision->SetCollisionProfileName("OverlapAllDynamic");
		}
	}

	//☆ボス敵の設定-----------------------------------------------------------------------------------------------
	this->Status.MaxHP = 300;
	this->Status.HP = Status.MaxHP;
	this->EnemyColor = EEnemyColor::BossColor;
}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();

}

void ABossEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickProcess();
}

//----------------------------------------------------------------------------------------
//★追加関数の処理★
//----------------------------------------------------------------------------------------
//Tickでの処理
void ABossEnemy::TickProcess()
{
	//現在のFPSを取得
	Gamefps = GetWorldFPS();

	MoveCount++;

	//エネミーの状態判断
	Think();
	//状態に基づいた動き
	ActProcess();
}

//エネミーの状態判断
void ABossEnemy::Think()
{
	EBossState NowState = this->BossState;
	switch (NowState)
	{
	//待機-----------------------------------------------------------------------------------
	case EBossState::Wait:
		if (bHasEndedWait)
		{
			int StateDecideNumber = FMath::RandRange((int)EBossState::Wait + 1, (int)EBossState::MaxStateCountAtTransitionFromWait - 1); //待機状態以外の生存時になる状態をランダムで決める
			//状態遷移
			switch (StateDecideNumber)
			{
			case 1:	// チャージ状態へ
				NowState = EBossState::Charge;
				UKismetSystemLibrary::PrintString(this, TEXT("Change Charge"), true, true, FColor::Red, 2.f);
				break;

			case 2:	// 雑魚敵を呼ぶ状態へ
				NowState = EBossState::EnemyCall;
				UKismetSystemLibrary::PrintString(this, TEXT("Change EnemyCall"), true, true, FColor::Green, 2.f);
				break;

			case 3:	// 瞬間移動状態へ
				NowState = EBossState::Teleportation;
				UKismetSystemLibrary::PrintString(this, TEXT("Change Teleportation"), true, true, FColor::Blue, 2.f);
				break;
			}
		}
		if (Status.HP <= 0) { NowState = EBossState::Die; }	// 死亡状態へ
		break;

	//チャージ-------------------------------------------------------------------------------
	case EBossState::Charge:
		if (bIsTransitionAttack) { NowState = EBossState::Attack; }	// 攻撃状態へ
		if (bIsTransitionStan) { NowState = EBossState::Stan; }		// スタン状態へ
		if (Status.HP <= 0) { NowState = EBossState::Die; }			// 死亡状態へ
		break;

	//攻撃-----------------------------------------------------------------------------------
	case EBossState::Attack:
		if (this->bHasEndedAttack) { NowState = EBossState::Wait; }	//待機状態へ
		if (Status.HP <= 0) { NowState = EBossState::Die; }			//死亡状態へ
		break;

	//スタン---------------------------------------------------------------------------------
	case EBossState::Stan:
		if (bHasEndedStan) { NowState = EBossState::Wait; }			//待機状態へ
		if (Status.HP <= 0) { NowState = EBossState::Die; }			//死亡状態へ
		break;

	//通常敵を呼ぶ---------------------------------------------------------------------------
	case EBossState::EnemyCall:
		if (bHasEndedEnemyCall) { NowState = EBossState::EnemyExpeditionWait; }	//通常敵の討伐待機状態へ
		if (Status.HP <= 0) { NowState = EBossState::Die; }						//死亡状態へ
		break;

	//通常敵の討伐待機-----------------------------------------------------------------------
	case EBossState::EnemyExpeditionWait:
		if (NowExistsEnemyNumber == 0) { NowState = EBossState::AfterEnemyExpedition; }	//通常敵の討伐後状態
		break;

	//通常敵の討伐後-------------------------------------------------------------------------
	case EBossState::AfterEnemyExpedition:
		if (bHasEndedAfterEnemyExpedition) { NowState = EBossState::Wait; }	//待機状態へ
		if (Status.HP <= 0) { NowState = EBossState::Die; }					//死亡状態へ
		break;

	//瞬間移動-------------------------------------------------------------------------------
	case EBossState::Teleportation:
		if (MoveCount >= 1 * Gamefps) { NowState = EBossState::Wait; }	//待機状態へ
		if (Status.HP <= 0) { NowState = EBossState::Die; }				//死亡状態へ
		break;
	}

	UpdateState(NowState);
}

//状態の更新
void ABossEnemy::UpdateState(EBossState nowState)
{
	if (nowState != this->BossState)
	{
		this->BossState = nowState;
		this->MoveCount = 0;
		this->bShouldBeenProcessWhenFirstStateTransition = false;
	}
}

//状態に基づいた動きをする
void ABossEnemy::ActProcess()
{
	switch (this->BossState)
	{
	//待機-----------------------------------------------------------------------------------
	case EBossState::Wait:
		//戦闘が始まっていたら色の変更を行う
		if (bIsBattleStarted)
		{
			//状態：Wait遷移時にのみ行う処理
			if (this->bShouldBeenProcessWhenFirstStateTransition == false)
			{
				ProcessJustForFirst_Wait();
			}

			//色の変更処理
			bHasEndedWait = ChangeColor(ChangingBossColor);
		}
		break;

	//チャージ-------------------------------------------------------------------------------
	case EBossState::Charge:
		//状態：Charge遷移時にのみ行う処理
		if (this->bShouldBeenProcessWhenFirstStateTransition == false)
		{
			ProcessJustForFirst_Charge();
		}

		//攻撃チャージの処理
		ChargeAttack();
		break;

	//攻撃-----------------------------------------------------------------------------------
	case EBossState::Attack:
		//状態：Attack遷移時にのみ行う処理
		if (this->bShouldBeenProcessWhenFirstStateTransition == false)
		{
			ProcessJustForFirst_Attack();
		}

		//攻撃する
		this->bHasEndedAttack = Attack();
		break;

	//スタン---------------------------------------------------------------------------------
	case EBossState::Stan:
		//スタン中の処理
		bHasEndedStan = Stan();
		break;

	//通常敵呼び出し-------------------------------------------------------------------------
	case EBossState::EnemyCall:
		//状態：EnemyCall遷移時にのみ行う処理
		if (this->bShouldBeenProcessWhenFirstStateTransition == false)
		{
			ProcessJustForFirst_EnemyCall();
		}

		//敵呼び出しの処理
		bHasEndedEnemyCall = EnemyCall();
		break;

	//通常敵の討伐待機-----------------------------------------------------------------------
	case EBossState::EnemyExpeditionWait:
		break;

	//通常敵の討伐後-------------------------------------------------------------------------
	case EBossState::AfterEnemyExpedition:
		//通常敵の討伐後状態で行う処理
		if (MoveCount >= 1 * FMath::RoundToInt(Gamefps)) // Gamefpsにかけている数は敵を生成するまでの秒数
		{
			AfterEnemyExpeditionProcess();
		}
		break;

	//瞬間移動-------------------------------------------------------------------------------
	case EBossState::Teleportation:
		/*メモ*/
		/*ファイル読み込み(ここで移動パターンを決定させる)*/
		/*ファイルから読み込んだ値をアクターの座標にする*/
		//状態:Teleportation遷移時にのみ行う処理
		if (this->bShouldBeenProcessWhenFirstStateTransition == false)
		{
			ProcessJustForFirst_Move();
		}

		//瞬間移動
		bHasEndedTeleportation = Move();
		break;

	//死亡-----------------------------------------------------------------------------------
	case EBossState::Die:
		EnemyDead();
		break;
	}
}

//プレイヤーがボス部屋で止まった時、位置と回転を取得する
void ABossEnemy::GetPlayerLocationAndRotation(const FVector PlayerLocation, const float PlayerRotation_Z)
{
	PlayerLocation_BossRoom = PlayerLocation;
	PlayerRotation_Z_BossRoom = PlayerRotation_Z;
}

//1秒カウントしたらtrueを返す
bool ABossEnemy::bOneSecondsPassedIs()
{
	FinishCount++;

	if (FinishCount >= FMath::RoundToInt(Gamefps) * 1)
	{
		FinishCount = 0;
		return true;
	}

	return false;
}

//☆状態：Waitの処理---------------------------------------------------------------------------------------------
//状態：Wait遷移時にのみ行う処理
void ABossEnemy::ProcessJustForFirst_Wait()
{
	//変更する色を決める
	do
	{
		ChangingBossColor = (EEnemyColor)FMath::RandRange(1, static_cast<int>(EEnemyColor::ColorTypeCount) - 1);
	} while (ChangingBossColor == this->EnemyColor);

	//呼ばれる関数を制御する変数をfalseにする
	bHasFinishedTransparentize = false;
	bHasFinishedChangeDecidedColor = false;
	bHasFinishedShow = false;

	//この関数が複数回呼ばれないようにする
	this->bShouldBeenProcessWhenFirstStateTransition = true;
}

//色の変更処理
bool ABossEnemy::ChangeColor(const EEnemyColor ChangingColor)
{
	//DeltaTimeの取得
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	//透明にする
	if (bHasFinishedTransparentize == false)
	{
		bHasFinishedTransparentize = Transparentize(DeltaTime);
	}

	//決めた色に変更する
	if (bHasFinishedTransparentize == true && bHasFinishedChangeDecidedColor == false)
	{
		bHasFinishedChangeDecidedColor = ChangeDecidedColor(ChangingColor);
	}

	//表示させる
	if (bHasFinishedChangeDecidedColor == true && bHasFinishedShow == false)
	{
		bHasFinishedShow = Show(DeltaTime);
	}

	//色の変更処理終了
	if (bHasFinishedShow == true)
	{
		//1秒待ったら状態を変更できるようにする
		if (bOneSecondsPassedIs())
		{
			return true;
		}
	}

	//色の変更処理続行
	return false;
}

//透明にする処理
bool ABossEnemy::Transparentize(const float DeltaTime)
{
	if (DynamicMaterial_Body && DynamicMaterial_Eye) // nullチェック
	{
		//オパシティの値を計算
		OpacityValue_Body -= MaxOpacity_Body / TimeUpToTransparency * DeltaTime;	// 体のオパシティの計算
		OpacityValue_Eye -= MaxOpacity_Eye / TimeUpToTransparency * DeltaTime;		// 目のオパシティの計算

		//出現が終わったら処理を終了する
		if (OpacityValue_Body <= 0.f && OpacityValue_Eye <= 0.f)
		{
			//オパシティの値が0を下回らないようにする
			OpacityValue_Body = 0.f;
			OpacityValue_Eye = 0.f;

			//オパシティを設定
			DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
			DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);

			//この関数が呼ばれないようにする
			return true;
		}

		//オパシティを設定
		DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
		DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);
	}

	//もう一度この関数を呼ぶ
	return false;
}

//決めた色に変更する
bool ABossEnemy::ChangeDecidedColor(const EEnemyColor ChangingColor)
{
	//敵の現在の色情報を変更
	this->EnemyColor = ChangingBossColor;

	//どの色に変更するか判別
	switch (ChangingColor)
	{
	case EEnemyColor::Green:	//緑
		ColorValue = FLinearColor(0.f, 1.f, 0.f);		// 緑の敵と同じ色
		break;

	case EEnemyColor::Red:		//赤
		ColorValue = FLinearColor(1.f, 0.f, 0.f);		// 赤の敵と同じ色
		break;

	case EEnemyColor::Blue:		//青
		ColorValue = FLinearColor(0.f, 0.f, 1.f);		// 青の敵と同じ色
		break;
	}

	//色を変更
	DynamicMaterial_Body->SetVectorParameterValue(FName("BaseColor"), ColorValue);

	//この関数を終えるまでのカウント
	FinishCount++;

	if (FinishCount >= 1 * Gamefps) // Gamefpsにかけている数は透明になっている時間
	{
		//FinishCountをゼロクリアする
		FinishCount = 0;

		//この関数が呼ばれないようにする
		return true;
	}

	//もう一度この関数を呼ぶ
	return false;
}

//表示させる
bool ABossEnemy::Show(const float DeltaTime)
{
	if (DynamicMaterial_Body && DynamicMaterial_Eye)
	{
		//オパシティの値を計算
		OpacityValue_Body += MaxOpacity_Body / TimeSpentInAppear * DeltaTime;	// 体のオパシティの計算
		OpacityValue_Eye += MaxOpacity_Eye / TimeSpentInAppear * DeltaTime;		// 目のオパシティの計算

		//出現が終わったら処理を終了する
		if (OpacityValue_Body >= MaxOpacity_Body && OpacityValue_Eye >= MaxOpacity_Eye)
		{
			//オパシティの値が1を超えないようにする
			OpacityValue_Body = MaxOpacity_Body;
			OpacityValue_Eye = MaxOpacity_Eye;

			//オパシティを設定
			DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
			DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);

			//当たり判定を付ける
			GhostCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			//状態遷移可能にする
			return true;
		}

		//オパシティを設定
		DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
		DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);
	}

	//もう一度この関数を呼ぶ
	return false;
}

//ボスの戦闘行動を開始させる
void ABossEnemy::BattleStart()
{
	bIsBattleStarted = true;
}

//☆状態：Chargeの処理------------------------------------------------------------------------------------------- 
//状態：Charge遷移時にのみ行う処理 
void ABossEnemy::ProcessJustForFirst_Charge()
{
	//遷移先を決める変数をfalseにする
	bIsTransitionAttack = false;	// 攻撃状態に遷移するか
	bIsTransitionStan = false;		// スタン状態に遷移するか

	//ボスの出現座標をクリアする
	BossGoalLocations.Empty();

	//現在位置を取得
	this->CurrentLocation = GetActorLocation();

	//ボスの瞬間移動する座標を設定する
	for (int i = 0; i < CountUpToAttack; ++i)
	{
		//ボスの移動位置を決めるラジアンの値を設定
		float GoalDegrees = PlayerRotation_Z_BossRoom + FMath::FRandRange(-90.f, 90.f);	// 何度にするか計算
		float GoalRadians = FMath::DegreesToRadians(GoalDegrees);						// ラジアンに変換

		//ボスの移動位置を決める
		float GoalX = (PlayerLocation_BossRoom.X + 500) * FMath::Cos(GoalRadians);
		float GoalY = (PlayerLocation_BossRoom.Y + 500) * FMath::Sin(GoalRadians);
		float GoalZ = CurrentLocation.Z;

		//ボスの出現位置を設定
		BossGoalLocations.Add(FVector(GoalX, GoalY, GoalZ));
	}

	//この関数が複数回呼ばれないようにする
	this->bShouldBeenProcessWhenFirstStateTransition = true;
}

//攻撃チャージの処理
void ABossEnemy::ChargeAttack()
{
	//チャージが終わっていなかったら
	if (ChargeCount < CountUpToAttack)
	{
		//決まった秒数たったら
		if (MoveCount % (FMath::RoundToInt(Gamefps) * 1) == 0) // FMath::RoundToInt(Gamefps)に掛けているマジックナンバーは秒数を表す
		{
			//瞬間移動し、チャージ回数のカウントを増やす
			Teleportation_Charge();
		}
	}

	//チャージが完了したら
	if (ChargeCount == CountUpToAttack)
	{
		//状態：Attackに遷移するまでの時間をカウントする
		CountUpToAttackStateTransition++;

		//攻撃状態に遷移出来るようにする
		if (CountUpToAttackStateTransition == TimeUpToAttackStateTransition * FMath::RoundToInt(Gamefps))
		{
			//攻撃までのカウント、チャージ回数のリセット
			CountUpToAttackStateTransition = 0;	// カウントのリセット
			ChargeCount = 0;					// チャージ回数のリセット

			//攻撃状態に遷移出来るようにする
			bIsTransitionAttack = true;
		}
	}

	//スタン値が最大になったら
	if (StanValue >= MaxStanValue)
	{
		//スタン値が最大値を超えないようにする
		StanValue = MaxStanValue;

		//スタン状態に遷移する
		bIsTransitionStan = true;
	}
}

//瞬間移動しながらチャージを行う処理
void ABossEnemy::Teleportation_Charge()
{
	//☆瞬間移動する
	//移動先座標を設定(ワールド座標)
	this->GoalLocation_World = BossGoalLocations[ChargeCount];

	//移動させる
	SetActorLocation(this->GoalLocation_World);

	//チャージする
	ChargeCount++;
}

//☆状態：Attackの処理-------------------------------------------------------------------------------------------
//状態：Attack遷移時にのみ行う処理
void ABossEnemy::ProcessJustForFirst_Attack()
{
	//スタン値を0にする
	StanValue = 0;

	//この関数が複数回呼ばれないようにする
	this->bShouldBeenProcessWhenFirstStateTransition = true;
}

//攻撃処理
bool ABossEnemy::Attack()
{
	//攻撃判定
	if (MoveCount == 1 * FMath::RoundToInt(Gamefps))
	{
		UKismetSystemLibrary::PrintString(this, TEXT("BossEnemy Attack!"), true, true, FColor::Yellow, 2.f, TEXT("None"));

		//プレイヤーへダメージを与える
		//プレイヤーへダメージを与える
		//プレイヤーの情報取得
		TObjectPtr<AVRPlayerCharacter> Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (Player)
		{
			Player->RecievePlayerDamage();
		}

		//攻撃終了(条件式で制御し、アニメーションが終わったらにするかも)
		return true;
	}

	//もう一度この関数を呼ぶ
	return false;
}

//☆状態：Stanの処理---------------------------------------------------------------------------------------------
//スタン処理
bool ABossEnemy::Stan()
{
	//スタン値の減少
	StanValue -= FMath::RoundToInt(1 * Gamefps / AssumptionFPS);

	//スタン値が0になったら待機状態に戻る
	if (StanValue <= 0)
	{
		//スタン値を0未満にならないようにする
		StanValue = 0;

		//スタン状態終了
		return true;
	}

	//もう一度この関数を呼ぶ
	return false;
}

//☆状態：EnemyCallの処理----------------------------------------------------------------------------------------
//状態：EnemyCall遷移時にのみ行う処理
void ABossEnemy::ProcessJustForFirst_EnemyCall()
{
	//敵生成に必要な情報を格納している配列を空にする
	NormalEnemyPopLocations.Empty();
	CallingEnemyColors.Empty();

	//通常敵の生成数を決める(2以上4以下の範囲)
	DecideCallingEnemyNumber();

	//通常敵の出現位置を決める
	DecideEnemyPopLocations(CallingEnemyNumber);

	//何色の敵を生成するか決める
	DecideCreateEnemyColor(CallingEnemyNumber);

	//透明化する関数を制御する変数をfalseにする
	bHasFinishedTransparentize = false;

	//この関数が呼ばれる状態の後にだけしか遷移しない状態を制御する変数をfalseにする
	bHasEndedAfterEnemyExpedition = false;

	//この関数が複数回呼ばれないようにする
	this->bShouldBeenProcessWhenFirstStateTransition = true;
}

//通常敵の生成数を決める(2以上4以下の範囲)
void ABossEnemy::DecideCallingEnemyNumber()
{
	CallingEnemyNumber = 2; // 通常敵の数：2

	//残りHPが2/3以下になったら
	if (Status.HP <= Status.HP * 2 / 3)
	{
		CallingEnemyNumber = 3; // 通常敵の数：3
	}

	//残りHPが1/3以下になったら
	if (Status.HP <= Status.MaxHP / 3)
	{
		CallingEnemyNumber = 4; // 通常敵の数：4
	}
}

//通常敵の出現位置を決める(ボス敵からの相対位置)
void ABossEnemy::DecideEnemyPopLocations(const int CallingEnemyNum)
{
	switch (CallingEnemyNum)
	{
	case 2:		//2体
		NormalEnemyPopLocations.Add(FVector(0.f, 300.f, 0.f));		// ボスから右に300ユニットの位置
		NormalEnemyPopLocations.Add(FVector(0.f, -300.f, 0.f));		// ボスから左に300ユニットの位置
		break;

	case 3:		//3体
		NormalEnemyPopLocations.Add(FVector(0.f, 300.f, -50.f));	// ボスから右に300ユニット、下に50ユニットの位置
		NormalEnemyPopLocations.Add(FVector(0.f, -300.f, -50.f));	// ボスから左に300ユニット、下に50ユニットの位置
		NormalEnemyPopLocations.Add(FVector(0.f, 0.f, 100.f));		// ボスから上に100ユニットの位置
		break;

	case 4:		//4体
		NormalEnemyPopLocations.Add(FVector(0.f, 300.f, -100.f));	// ボスから右に300ユニット、下に100ユニットの位置
		NormalEnemyPopLocations.Add(FVector(0.f, -300.f, -100.f));	// ボスから左に300ユニット、下に100ユニットの位置
		NormalEnemyPopLocations.Add(FVector(0.f, 300.f, 100.f));	// ボスから右に300ユニット、上に100ユニットの位置
		NormalEnemyPopLocations.Add(FVector(0.f, -300.f, 100.f));	// ボスから左に300ユニット、上に100ユニットの位置
		break;

	default:	//不正な数
		for (int i = 0; i < CallingEnemyNum; ++i) // 敵生成時にメモリーリークが起こらないようにする
		{
			NormalEnemyPopLocations.AddZeroed();
		}
		break;
	}
}

//何色の敵が生成されるか決める
void ABossEnemy::DecideCreateEnemyColor(const int CallingEnemyNum)
{
	for (int i = 0; i < CallingEnemyNum; ++i) // 生成する敵の数だけループする
	{
		//何色の敵を生成するかランダムで決める
		CallingEnemyColors.Add(static_cast<EEnemyColor>(FMath::RandRange(1, static_cast<int>(EEnemyColor::ColorTypeCount) - 1)));
	}
}

//通常敵呼び処理
bool ABossEnemy::EnemyCall()
{
	//DeltaTimeの取得
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	//敵が出現するワープホールのようなエフェクトの生成

	//透明化する(Waitでの透明化処理を流用)
	if (bHasFinishedTransparentize == false)
	{
		bHasFinishedTransparentize = Transparentize(DeltaTime);
	}

	//透明化後の処理
	if (bHasFinishedTransparentize == true)
	{
		//この関数を終了するまでのカウント
		FinishCount++;

		if (FinishCount >= 1 * Gamefps) // Gamefpsにかけている数は敵を生成するまでの秒数
		{
			//FinishCountをゼロクリアする
			FinishCount = 0;

			//通常敵の生成
			CreateEnemies(CallingEnemyNumber, NormalEnemyPopLocations, CallingEnemyColors);

			//ボス敵の状態を敵の討伐待機状態に遷移できるようにする
			return true;
		}
	}

	//もう一度この関数を呼ぶ
	return false;
}

// 通常敵の生成処理
void ABossEnemy::CreateEnemies(const int CallingEnemyNum, const TArray<FVector>& PopLocations, const TArray<EEnemyColor>& EnemyColors)
{
	//ボス敵の参照を取得
	TObjectPtr<ABossEnemy> BossEnemy = Cast<ABossEnemy>(UGameplayStatics::GetActorOfClass(GetWorld(), ABossEnemy::StaticClass()));

	if (BossEnemy) // ボス敵の参照がされているか
	{
		//通常敵の生成
		for (int i = 0; i < CallingEnemyNum; ++i)
		{
			//生成する敵のインスタンス
			TObjectPtr<AEnemys> CallingEnemy = nullptr;

			////決めた色の敵を生成する
			switch (EnemyColors[i])
			{
			case EEnemyColor::White:	//白
				CallingEnemy = GetWorld()->SpawnActor<AEnemys>(ANormalEnemy::StaticClass(), FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f));
				break;

			case EEnemyColor::Green:	//緑
				CallingEnemy = GetWorld()->SpawnActor<AEnemys>(AGreenEnemy::StaticClass(), FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f));
				break;

			case EEnemyColor::Red:		//赤
				CallingEnemy = GetWorld()->SpawnActor<AEnemys>(ARedEnemy::StaticClass(), FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f));
				break;

			case EEnemyColor::Blue:		//青
				CallingEnemy = GetWorld()->SpawnActor<AEnemys>(ABlueEnemy::StaticClass(), FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f));
				break;
			}

			//ボス敵を親とし、ボス敵からの相対位置で出現位置を設定する
			if (CallingEnemy) // 敵インスタンスが生成されていたら
			{
				//ボスアクターを親とする
				CallingEnemy->AttachToActor(BossEnemy, FAttachmentTransformRules::KeepRelativeTransform);

				//相対位置を設定
				CallingEnemy->SetActorRelativeLocation(PopLocations[i]);
			}

			//呼んだ(生成した)敵の数をカウントする
			NowExistsEnemyNumber++;
		}
	}
}

//☆状態：EnemyExpeditionWaitの処理------------------------------------------------------------------------------
//呼んだ敵が死んだとき、その死んだという情報を受け取る(NowExistsEnemyNumber--する)
void ABossEnemy::ReceiveInfoDeadCalledEnemy()
{
	//現在の存在している敵の数を減らす
	NowExistsEnemyNumber--;
}

//☆状態：AfterEnemyExpeditionの処理-----------------------------------------------------------------------------
//通常敵の討伐後状態で行う処理
void ABossEnemy::AfterEnemyExpeditionProcess()
{
	//DeltaTimeの取得
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	//ボス敵を表示させる(待機状態の表示処理を流用)
	bHasEndedAfterEnemyExpedition = Show(DeltaTime); // 表示し終わったら次の状態に遷移する
}

//☆状態：Teleportationの処理------------------------------------------------------------------------------------
//状態:Teleportation遷移時にのみ行う処理
void ABossEnemy::ProcessJustForFirst_Move()
{
	//現在位置を取得
	this->CurrentLocation = GetActorLocation();

	//ボスの瞬間移動する座標を設定する
	//ボスの移動位置を決めるラジアンの値を設定
	float GoalDegrees = PlayerRotation_Z_BossRoom + FMath::FRandRange(-90.f, 90.f);	// 何度にするか計算
	float GoalRadians = FMath::DegreesToRadians(GoalDegrees);						// ラジアンに変換

	//ボスの移動位置を決める
	float GoalX = (PlayerLocation_BossRoom.X + 500) * FMath::Cos(GoalRadians);
	float GoalY = (PlayerLocation_BossRoom.Y + 500) * FMath::Sin(GoalRadians);
	float GoalZ = CurrentLocation.Z;

	//ボスの出現位置を設定
	BossGoalLocation = FVector(GoalX, GoalY, GoalZ);

	//この関数が複数回呼ばれないようにする
	this->bShouldBeenProcessWhenFirstStateTransition = true;
}

//瞬間移動
bool ABossEnemy::Move()
{
	//瞬間移動する
	SetActorLocation(this->BossGoalLocation);

	return true;
}

//☆状態：Appearの処理-------------------------------------------------------------------------------------------

//ダメージを受ける処理、引数でもらった攻撃力分体力を減らす
void ABossEnemy::RecieveEnemyDamage(int DamageAmount)
{
	//ライトの色と敵の色が一致したときだけダメージを受ける
	Status.HP -= DamageAmount;
}

//プレイヤーのライトの色と敵のライトの色をチェックする関数
bool ABossEnemy::CheckPlayerLightColor(EFlashlight_Color PlayerColor) const
{
	return (int)PlayerColor == (int)this->EnemyColor;
}

/*メモ*/
/*余裕があったら、各状態に遷移する為のbool型変数をひとまとめにする*/