// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/NormalEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/VRPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ANormalEnemy::ANormalEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//シーンコンポーネント------------------------------------------------------------------------------------------------------
	//シーンコンポーネントの作成
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	//シーンコンポーネントをルートコンポーネントに設定
	RootComponent = DefaultSceneRoot;

	//スタティックメッシュコンポーネント----------------------------------------------------------------------------------------
	//スタティックメッシュコンポーネントの作成
	GhostMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ghost"));
	//スタティックメッシュをロードしてスタティックメッシュコンポーネントのスタティックメッシュに設定する
	UStaticMesh* GMesh = LoadObject<UStaticMesh>(NULL, TEXT("/Engine/BasicShapes/Sphere"), NULL, LOAD_None, NULL);
	GhostMesh->SetStaticMesh(GMesh);
	//スタティックメッシュコンポーネントをルートコンポーネントにアタッチする
	GhostMesh->SetupAttachment(RootComponent);
	//スタティックメッシュのコリジョンを無くす
	GhostMesh->SetCollisionProfileName("NoCollision");

	//コリジョン-----------------------------------------------------------------------------------------------------------------
	//スフィアコリジョンの作成
	GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("GhostCollision"));
	//GhostCollisionをルートコンポーネントにアタッチする
	GhostCollision->SetupAttachment(RootComponent);
	//GhostCollisionのコリジョンプリセットをOverlapAllDynamicにする
	GhostCollision->SetCollisionProfileName("OverlapAllDynamic");

	//スプラインコンポーネント---------------------------------------------------------------------------------------------------
	//スプラインコンポーネントの作成
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("EnemySplineComponent"));
	//スプラインコンポーネントをルートコンポーネントにアタッチする
	SplineComponent->SetupAttachment(RootComponent);

	//サウンド-------------------------------------------------------------------------------------------------------------------
	AppearSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_GhostAppear_2_Cue"));	//出現時の音設定
	DisappearSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_GhostDead_2_Cue"));	//消滅時の音設定

}

// Called when the game starts or when spawned
void ANormalEnemy::BeginPlay()
{
	Super::BeginPlay();

	//☆白い敵の設定
	this->EnemyColor = EEnemyColor::White;

	//☆マテリアル
	//マテリアルをロード
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Game/_TeamFolder/Enemy/White"), NULL, LOAD_None, NULL);
	if (Material)
	{
		//ダイナミックマテリアルインスタンスを作成
		this->DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);

		//GhostMeshにダイナミックマテリアルを設定
		GhostMesh->SetMaterial(0, DynamicMaterial);

		//初期オパシティ値を設定
		this->DynamicMaterial->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
	}

	GoalLocations.Add(FVector(0.f, 50.f, 0.f));
}

// Called every frame
void ANormalEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickProcess();
}

//☆追加関数
// Tickでの処理
void ANormalEnemy::TickProcess()
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
void ANormalEnemy::Think()
{
	EState NowState = this->State;

	switch (NowState)
	{
	case EState::Wait:	//待機
		if (MoveCount >= 1 * Gamefps) { NowState = EState::Attack; }	// 攻撃へ
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
void ANormalEnemy::ActProcess()
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
		//攻撃処理(攻撃が終わった後状態遷移する)
		this->bHasEndedAttack = this->Attack();
		break;

	case EState::Die:	//死亡
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

//ダメージを受ける処理、引数でもらった数値分体力を減らす
void ANormalEnemy::RecieveEnemyDamage(int DamageAmount)
{
	if (this->State != EState::Appear && this->State != EState::Die)
	{
		Status.HP -= DamageAmount;
	}
}

//プレイヤーのライトの色と敵のライトの色をチェックする関数
bool ANormalEnemy::CheckPlayerLightColor(EFlashlight_Color PlayerColor) const
{
	return (int)PlayerColor == (int)this->EnemyColor;
}

//状態Move遷移時にのみ行う処理
void ANormalEnemy::ProcessJustForFirst_Move()
{
	//スプラインの設定
	if (this->SplineComponent)	// スプラインが格納されていたら
	{
		//ゼロクリアする
		this->TraveledDistance = 0.f;

		//既存のスプラインのポイントをクリアする
		this->SplineComponent->ClearSplinePoints();

		//スプラインの開始、終了地点設定
		FVector StartLocation = GetActorLocation();											// スプラインの開始地点設定(敵の現在の座標)
		FVector GoalLocation = StartLocation + this->GoalLocations[CountGotInMoveState];	// スプラインの終了地点設定(敵の移動先座標)

		//開始、終了地点のポイント追加
		this->SplineComponent->AddSplinePoint(StartLocation, ESplineCoordinateSpace::World);	//開始地点のポイント設定
		this->SplineComponent->AddSplinePoint(GoalLocation, ESplineCoordinateSpace::World);		//終了地点のポイント設定

		//スプラインの更新
		this->SplineComponent->UpdateSpline();

		//スプラインの全長を取得
		this->SplineLength = this->SplineComponent->GetSplineLength();

		//移動速度を計算
		this->Status.Speed = this->SplineLength / this->MoveTime;

		if (CountGotInMoveState == GoalLocations.Num() - 1) // 移動した回数を、格納している目標座標の数と同じか
		{
			//移動した回数を格納している回数をリセットする
			CountGotInMoveState = 0;
		}
		else // 同じでないなら
		{
			CountGotInMoveState++;
		}
	}

	////初期位置の設定
	//this->CurrentLocation = GetActorLocation();

	////ワールド座標への変換
	//this->GoalLocation_World = this->CurrentLocation + this->GoalLocation;

	////方向ベクトルの計算
	//this->Direction = (this->GoalLocation_World - this->CurrentLocation).GetSafeNormal();

	////総移動距離の計算
	//this->TotalDistance = FVector::Dist(this->CurrentLocation, this->GoalLocation_World);

	////目的地に着くまでの時間に合うように速度を計算
	//this->Speed = this->TotalDistance / this->MoveTime;

	//複数回処理が行われないようにする
	this->bShouldBeenProcessWhenFirstStateTransition = true;
}

//移動処理
bool ANormalEnemy::Move()
{
	//DeltaTimeの取得
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	if (this->SplineComponent)
	{
		//移動した距離を計算する
		this->TraveledDistance += this->Status.Speed * DeltaTime;

		//移動した距離がスプラインの全長を超えないようにする
		if (this->TraveledDistance > this->SplineLength)
		{
			this->TraveledDistance = this->SplineLength;
		}

		//移動後の座標を取得する
		FVector NewLocation = this->SplineComponent->GetLocationAtDistanceAlongSpline(this->TraveledDistance, ESplineCoordinateSpace::World);

		UKismetSystemLibrary::PrintString(this, FString::SanitizeFloat(NewLocation.Y), true, true, FColor::White, 2.f, TEXT("None"));

		//移動させる
		SetActorLocation(NewLocation);

		//移動が終了したかをチェック
		if (this->TraveledDistance == this->SplineLength)
		{
			//次の状態に遷移する
			return true;
		}
	}

	////目的地までの残り距離を計算
	//float RemainingDistance = TotalDistance - TraveledDistance;

	////現在の速度での移動距離を計算
	//float DeltaDistance = Speed * DeltaTime;

	////目的地に近づきすぎたら、残りの距離だけ進むように調整
	//if (DeltaDistance >= RemainingDistance)
	//{
	//	DeltaDistance = RemainingDistance;
	//	TraveledDistance = TotalDistance;
	//}
	//else
	//{
	//	TraveledDistance += DeltaDistance;
	//}

	////正弦波に基づいてオフセットを計算(目的地に瞬間移動して着かないように調整する計算)
	//float Offset_Z = Amplitude * FMath::Sin(2.0f * PI * (TraveledDistance / TotalDistance));

	//// 新しい位置を計算
	//FVector NewLocation = CurrentLocation + (Direction * TraveledDistance);
	//NewLocation.Z += Offset_Z;

	//// 新しい位置に移動
	//SetActorLocation(NewLocation);

	//// 目的地に到達したら処理を終了
	//if (TraveledDistance >= TotalDistance)
	//{
	//	SetActorLocation(this->GoalLocation_World);

	//	//状態遷移できるようにする
	//	return true;
	//}

	//もう一度この関数を呼ぶ
	return false;
}

//攻撃処理
bool ANormalEnemy::Attack()
{
	//攻撃判定
	if (MoveCount >= AttackUpToTime * Gamefps)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("WhiteEnemy Attack!"), true, true, FColor::White, 2.f, TEXT("None"));

		//プレイヤーへダメージを与える
		//プレイヤーの情報取得
		AVRPlayerCharacter* Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (Player)
		{
			Player->RecievePlayerDamage();
		}

		//攻撃終了(条件式で制御し、アニメーションが終わったらにするかも)
		return true;
	}

	return false;
}

//出現関係---------------------------------------------------------------------------------------------------------------------
//状態：Appearで最初に一度だけする処理
void ANormalEnemy::ProcessJustForFirst_Appear()
{
	//敵出現時の音を鳴らす
	PlayAppearSound();

	//複数回処理が行われないようにする
	this->bShouldBeenProcessWhenFirstStateTransition = true;
}

//敵出現処理
bool ANormalEnemy::Appear()
{
	//DeltaTimeの取得
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	if (DynamicMaterial)
	{
		//オパシティの値を変更
		this->OpacityValue += 1.f / (float)TimeSpentInAppear * DeltaTime;

		//出現が終わったら処理を終了する
		if (this->OpacityValue >= 1.f)
		{
			//オパシティの値が1を超えないようにする
			this->OpacityValue = 1.f;

			//オパシティを設定
			this->DynamicMaterial->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);

			//状態遷移可能にする
			return true;
		}

		//オパシティを設定
		this->DynamicMaterial->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
	}

	return false;
}

//メモ
/*
Move後の敵の行動を外部ファイルで制御したい
・外部ファイルの内容をセットする関数の追加
・Move後の状態遷移処理を外部ファイルの内容で決まるようにする

switch(hoge)
{
	case 0:
		State StateHoge1 = State::Wait;
}
みたいな感じ
*/