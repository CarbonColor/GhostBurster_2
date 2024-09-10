// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/RedEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/VRPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

ARedEnemy::ARedEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	//☆シーンコンポーネント--------------------------------------------------------------------------------------------------------
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

	//☆アニメーション-------------------------------------------------------------------------------------------------------------
	this->DefaultAnim = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/_TeamFolder/CG/CG_Model/Ghost/Anim_Idle"));			// 特定のアニメーションを使用しない状態のアニメーション
	this->AttackAnim = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/_TeamFolder/CG/CG_Model/Ghost/Test_GhostAttack"));	// 攻撃状態のアニメーション

	//☆サウンド-------------------------------------------------------------------------------------------------------------------
	AppearSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_GhostAppear_2_Cue"));	//出現時の音設定
	DisappearSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_GhostDead_2_Cue"));	//消滅時の音設定
}

void ARedEnemy::BeginPlay()
{
	Super::BeginPlay();

	//☆赤の敵の設定
	this->EnemyColor = EEnemyColor::Red;

	//☆マテリアル
	//体のマテリアルをロード
	TObjectPtr<UMaterial> BodyMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Game/_TeamFolder/Enemy/M_Red"));
	if (BodyMaterial)
	{
		//ダイナミックマテリアルインスタンスを作成
		this->DynamicMaterial_Body = UMaterialInstanceDynamic::Create(BodyMaterial, this);

		//GhostMeshにダイナミックマテリアルを設定
		this->GhostMeshComponent->SetMaterial(0, DynamicMaterial_Body);

		//初期オパシティ値を設定
		this->DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
	}
	//目のマテリアルをロード
	TObjectPtr<UMaterial> EyeMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Game/_TeamFolder/CG/CG_Model/Ghost/M_Ghost_Eye"));
	if (EyeMaterial)
	{
		//ダイナミックマテリアルインスタンスを作成
		this->DynamicMaterial_Eye = UMaterialInstanceDynamic::Create(EyeMaterial, this);

		//GhostMeshにダイナミックマテリアルを設定
		this->GhostMeshComponent->SetMaterial(1, DynamicMaterial_Eye);

		//初期オパシティ値を設定
		this->DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
	}
}

void ARedEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickProcess();
}

//☆追加関数
//Tickでの処理
void ARedEnemy::TickProcess()
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
void ARedEnemy::Think()
{
	EState NowState = this->State;
	switch (NowState)
	{
	case EState::Wait:	//待機
		if (MoveCount >= AttackUpToTime * Gamefps) { NowState = EState::Attack; }	// 攻撃へ
		if (Status.HP <= 0) { NowState = EState::Die; }				// 死亡へ
		break;

	case EState::Move:	//移動
		if (this->bHasEndedMoving) { NowState = EState::Wait; }	// 待機へ
		if (Status.HP <= 0) { NowState = EState::Die; }			// 死亡へ
		break;

	case EState::Attack:	//攻撃
		if (this->bHasEndedAttack) { NowState = EState::Wait; }	// 待機へ
		if (Status.HP <= 0) { NowState = EState::Die; }			// 死亡へ
		break;

	case EState::Appear:	//出現
		if (this->bHasEndedAppear) { NowState = EState::Move; }	// 移動へ
		break;
	}

	UpdateState(NowState);
}

//状態に基づいた動きをする
void ARedEnemy::ActProcess()
{
	switch (this->State)
	{
	case EState::Wait:	//待機		
		break;

	case EState::Move:	//移動
		//状態Move遷移時にのみ行う処理
		if (this->bShouldBeenProcessWhenFirstStateTransition == false)
		{
			ProcessJustForFirst_Move();
		}

		//移動処理(移動処理が終わったら状態遷移する)
		this->bHasEndedMoving = Move();
		break;

	case EState::Attack:	//攻撃
		//攻撃処理
		this->bHasEndedAttack = this->Attack();
		break;

	case EState::Die:
		EnemyDead();
		break;

	case EState::Appear:	//出現
		//状態Move遷移時にのみ行う処理
		if (this->bShouldBeenProcessWhenFirstStateTransition == false)
		{
			ProcessJustForFirst_Appear();
		}

		//出現処理
		this->bHasEndedAppear = this->Appear();
		break;
	}
}

//ダメージを受ける処理、引数でもらった攻撃力分体力を減らす
void ARedEnemy::RecieveEnemyDamage(int DamageAmount)
{
	if (this->State != EState::Appear && this->State != EState::Die)
	{
		Status.HP -= DamageAmount;
	}
}

//プレイヤーのライトの色と敵のライトの色をチェックする関数
bool ARedEnemy::CheckPlayerLightColor(EFlashlight_Color PlayerColor) const
{
	return (int)PlayerColor == (int)this->EnemyColor;
}

//状態Move遷移時にのみ行う処理
void ARedEnemy::ProcessJustForFirst_Move()
{
	if (GoalLocations.Num() != 0) // 目標地点があるか確認
	{
		//ゼロクリアする
		this->TraveledDistance = 0.f;

		//初期位置の設定
		this->CurrentLocation = GetActorLocation();

		//ワールド座標への変換
		this->GoalLocation_World = this->CurrentLocation + this->GoalLocations[MovingTimesCount];

		//方向ベクトルの計算
		this->Direction = (this->GoalLocation_World - this->CurrentLocation).GetSafeNormal();

		//総移動距離の計算
		this->TotalDistance = FVector::Dist(this->CurrentLocation, this->GoalLocation_World);

		//目的地に着くまでの時間に合うように速度を計算
		this->Status.Speed = this->TotalDistance / this->MoveTime;

		//移動回数の確認
		if (this->MovingTimesCount == this->GoalLocations.Num() - 1) // 移動回数を格納している変数の値が目標座標の数と同じ数なら
		{
			//移動回数のリセット
			this->MovingTimesCount = 0;
		}
		else
		{
			//移動回数を増やす
			this->MovingTimesCount++;
		}
	}

	//複数回処理が行われないようにする
	this->bShouldBeenProcessWhenFirstStateTransition = true;
}

//移動処理
bool ARedEnemy::Move()
{
	if (GoalLocations.Num() != 0) // 目標地点があるか確認
	{
		//DeltaTimeの取得
		float DeltaTime = GetWorld()->GetDeltaSeconds();

		//目的地までの残り距離を計算
		float RemainingDistance = TotalDistance - TraveledDistance;

		//現在の速度での移動距離を計算
		float DeltaDistance = Status.Speed * DeltaTime;

		//目的地に近づきすぎたら、残りの距離だけ進むように調整
		if (DeltaDistance >= RemainingDistance)
		{
			DeltaDistance = RemainingDistance;
			TraveledDistance = TotalDistance;
		}
		else
		{
			TraveledDistance += DeltaDistance;
		}

		// 新しい位置を計算
		FVector NewLocation = CurrentLocation + (Direction * TraveledDistance);

		// 新しい位置に移動
		SetActorLocation(NewLocation);

		// 目的地に到達したら処理を終了
		if (TraveledDistance >= TotalDistance)
		{
			SetActorLocation(this->GoalLocation_World);

			//状態遷移できるようにする
			return true;
		}

		//もう一度この関数を呼ぶ
		return false;
	}

	// 目標地点が無かったら状態遷移できるようにする
	return true;
}

//攻撃処理
bool ARedEnemy::Attack()
{
	if (GhostMeshComponent && AttackAnim) // nullチェック
	{
		//攻撃判定
		if (MoveCount == FMath::RoundToInt(AttackTiming * Gamefps / AssumptionFPS)) // 敵にダメージを与える
		{
			/*UKismetSystemLibrary::PrintString(this, TEXT("RedEnemy Attack!"), true, true, FColor::Red, 2.f, TEXT("None"));*/

			//プレイヤーへダメージを与える
			//プレイヤーの情報取得
			AVRPlayerCharacter* Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
			if (Player)
			{
				Player->RecievePlayerDamage();
			}
		}
		if (!GhostMeshComponent->IsPlaying()) // アニメーションが終わったら
		{
			//攻撃終了(条件式で制御し、アニメーションが終わったらにするかも)
			return true;
		}
	}
	else
	{
		//攻撃終了(条件式で制御し、アニメーションが終わったらにするかも)
		return true;
	}

	//もう一度この関数を呼ぶ
	return false;
}

//出現関係---------------------------------------------------------------------------------------------------------------------
//状態：Appearで最初に一度だけする処理
void ARedEnemy::ProcessJustForFirst_Appear()
{
	//敵出現時の音を鳴らす
	PlayAppearSound();

	//複数回処理が行われないようにする
	this->bShouldBeenProcessWhenFirstStateTransition = true;
}

//敵出現処理
bool ARedEnemy::Appear()
{
	//DeltaTimeの取得
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	if (DynamicMaterial_Body && DynamicMaterial_Eye)
	{
		//オパシティの値を変更
		this->OpacityValue += 1.f / (float)TimeSpentInAppear * DeltaTime;

		//出現が終わったら処理を終了する
		if (this->OpacityValue >= 1.f)
		{
			//オパシティの値が1を超えないようにする
			this->OpacityValue = 1.f;

			//オパシティを設定
			this->DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
			this->DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);

			//状態遷移可能にする
			return true;
		}

		//オパシティを設定
		this->DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
		this->DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
	}

	return false;
}