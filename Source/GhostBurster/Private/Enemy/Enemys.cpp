// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemys.h"
#include "Engine/World.h"
#include "Player/VRPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemys::AEnemys()
	:
	MoveCount(0),
	bOnceDoProcessBeenIs(false),
	//FPS関係
	Gamefps(60.f), AssumptionFPS(60),
	//構造体
	Status(),
	//列挙型
	State(EState::Appear), EnemyColor(EEnemyColor::White),
	//コンポーネント関係
	DefaultSceneRoot(nullptr), GhostMeshComponent(nullptr), GhostCollision(nullptr), DynamicMaterial_Body(nullptr), DynamicMaterial_Eye(nullptr), EnemyScale(FVector(0.6f, 0.6f, 0.6f)),
	//アニメーション関係
	DefaultAnim(nullptr), AttackAnim(nullptr), AttackTiming(55),
	//サウンド関係
	AppearSound(nullptr), DisappearSound(nullptr),
	//移動関係
	MoveTime(1.f), TraveledDistance(0.f), CurrentLocation(FVector(0.f, 0.f, 0.f)), GoalLocations(), MovingTimesCount(0), GoalLocation_World(FVector(0.f, 0.f, 0.f)), bHasEndedMoving(false),
	Direction(FVector(0.f, 0.f, 0.f)), TotalDistance(0.f), Amplitude(40.f), Frequency(1.f), 
	//攻撃関係
	bHasEndedAttack(false), AttackUpToTime(1.f),
	//死亡関係
	bIsDestroy(false),
	//出現関係
	bHasEndedAppear(false), OpacityValue(0.f), TimeSpentInAppear(1), MaxOpacity(0.8f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemys::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEnemys::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//状態の更新
void AEnemys::UpdateState(EState NowState)
{
	if (NowState != this->State)
	{
		//アニメーション変更
		ChangeAnimation(State, NowState);

		State = NowState;
		MoveCount = 0;
		bOnceDoProcessBeenIs = false;
	}
}

//FPS関係----------------------------------------------------------------------------------------------------------------------
//現在のFPSを取得する
float AEnemys::GetWorldFPS()
{
	//DeltaTime取得
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	//現在のFPSを計算して取得
	float FPS = 1.f / DeltaTime;

	return FPS;
}

//死亡関係---------------------------------------------------------------------------------------------------------------------
//HPが0になったら消滅させる
void AEnemys::EnemyDead()
{
	//EnemyDeadで一度だけ行う処理
	if (bOnceDoProcessBeenIs == false)
	{
		ProcessDoOnce_EnemyDead();
	}

	//徐々に透明にする
	bIsDestroy = Transparentize_Dead();

	//敵を消滅させる
	if (bIsDestroy)
	{
		this->Destroy();
	}
}

//EnemyDeadで一度だけ行う処理
void AEnemys::ProcessDoOnce_EnemyDead()
{
	//イベントに死亡通知を送る
	// プレイヤーを取得
	TObjectPtr<AVRPlayerCharacter> Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (Player)
	{
		//ステージ名を取得
		int Stage = Player->GetStageNumber();
		FString SpawnBPName = FString::Printf(TEXT("EnemysSpawn_BP_C_%d"), Stage + 2);
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Silver, SpawnBPName);

		//該当のEnemySpawnを取得
		TArray<AActor*> Spawners;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Spawner"), Spawners);

		for (AActor* Spawner : Spawners)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, Spawner->GetName());

			if (Spawner->GetName() == SpawnBPName)
			{
				if (UFunction* Func = Spawner->FindFunction(FName("EnemyDeadFunction")))
				{
					Spawner->ProcessEvent(Func, nullptr);
					Player->AddScore(100);
				}
			}
		}
	}

	//当たり判定を消す
	if (GhostCollision) // nullチェック
	{
		GhostCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	//敵消滅時の音を鳴らす
	PlayDisappearSound();

	//複数回処理が行われないようにする
	bOnceDoProcessBeenIs = true;
}

//徐々に透明にする
bool AEnemys::Transparentize_Dead()
{
	//DeltaTimeの取得
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	if (DynamicMaterial_Body && DynamicMaterial_Eye)
	{
		//オパシティの値を変更
		this->OpacityValue -= DeltaTime;

		//出現が終わったら処理を終了する
		if (this->OpacityValue <= 0.f)
		{
			//オパシティの値が0を下回らないようにする
			this->OpacityValue = 0.f;

			//オパシティを設定
			this->DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
			this->DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);

			//この関数が呼ばれないようにする
			return true;
		}

		//オパシティを設定
		this->DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
		this->DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
	}

	//もう一度この関数を呼ぶ
	return false;
}

//出現関係---------------------------------------------------------------------------------------------------------------------
//状態：Appearで最初に一度だけする処理
void AEnemys::ProcessJustForFirst_Appear()
{
	//敵出現時の音を鳴らす
	PlayAppearSound();

	//複数回処理が行われないようにする
	this->bOnceDoProcessBeenIs = true;
}

//敵出現処理
bool AEnemys::Appear()
{
	//DeltaTimeの取得
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	if (DynamicMaterial_Body && DynamicMaterial_Eye)
	{
		//オパシティの値を変更
		OpacityValue += MaxOpacity / (float)TimeSpentInAppear * DeltaTime;

		//出現が終わったら処理を終了する
		if (OpacityValue >= MaxOpacity)
		{
			//オパシティの値が1を超えないようにする
			OpacityValue = MaxOpacity;

			//オパシティを設定
			DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
			DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);

			//当たり判定を付ける
			GhostCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			//状態遷移可能にする
			return true;
		}

		//オパシティを設定
		DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
		DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
	}

	return false;
}

//回転関係---------------------------------------------------------------------------------------------------------------------
//プレイヤーの方向を向く
void AEnemys::FacePlayerHowTo()
{
	//プレイヤーを取得する
	TObjectPtr<AVRPlayerCharacter> Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (Player) //nullチェック
	{
		//方向を計算する
		FVector DirectionUpToTarget = Player->GetActorLocation() - GetActorLocation();

		//ベクトルを正規化する
		DirectionUpToTarget.Normalize();

		//X軸方向を基にした回転を取得
		FRotator NewRotation = FRotationMatrix::MakeFromX(DirectionUpToTarget).Rotator();

		//回転をアクターに適用させる
		SetActorRotation(NewRotation);
	}
}

//アニメーション関係-----------------------------------------------------------------------------------------------------------
void AEnemys::ChangeAnimation(const EState PreState, const EState NewState)
{
	//状態に合わせたアニメーション変更
	switch (NewState)
	{
	case EState::Attack: // 攻撃状態
		if (AttackAnim)	// nullチェック
		{
			GhostMeshComponent->PlayAnimation(AttackAnim, false);
		}
		break;

	default: // 特定のアニメーションがない状態
		if (DefaultAnim) // nullチェック
		{
			if (PreState == EState::Attack || PreState == EState::Appear) // 変更前の状態が特定のアニメーションを持っているまたは、アニメーションを使用しない状態だったら
			{
				GhostMeshComponent->PlayAnimation(DefaultAnim, true);
			}
		}
		break;

	//アニメーションを使用しない状態-------------------------------------------------------------------------
	case EState::Appear: // この状態は敵の最初の状態なのでアニメーションの強制終了は必要ない
		break;

	case EState::Die: // この状態の後にアニメーションが変更されることはないのでdefaultのif文の変更前の状態は死亡状態かどうかの確認はしなくてよい
		if (GhostMeshComponent)
		{
			GhostMeshComponent->Stop();
		}
		break;
	}
}

//サウンド関数-----------------------------------------------------------------------------------------------------------------
//敵出現時の音を鳴らす
void AEnemys::PlayAppearSound()
{
	if (this->AppearSound) // 設定されていなかったら鳴らないようにする
	{
		UGameplayStatics::PlaySoundAtLocation(this, AppearSound, GetActorLocation());
	}
}

//敵消滅時の音を鳴らす
void AEnemys::PlayDisappearSound()
{
	if (DisappearSound) // 設定されていなかったら鳴らないようにする
	{
		UGameplayStatics::PlaySoundAtLocation(this, DisappearSound, GetActorLocation());
	}
}

//Setter関数-------------------------------------------------------------------------------------------------------------------
//HPの設定用関数
void AEnemys::SetHP(const int HPValue)
{
	this->Status.HP = HPValue;
}

//攻撃までの時間設定用関数
void AEnemys::SetAttackUpToTime(const float SetTime)
{
	this->AttackUpToTime = SetTime;
}

//目標座標の設定用関数
void AEnemys::SetGoalLocations(const TArray<FVector>& SetLocations)
{
	this->GoalLocations = SetLocations;
}

//移動時間の設定用
void AEnemys::SetMoveTime(const float SetTime)
{
	this->MoveTime = SetTime;
}

//生成されたときの設定用関数
void AEnemys::SetInitialData(const int HP, const float AttackUpToTimeValue, const TArray<FVector>& SetLocations, const float MoveTimeValue)
{
	this->SetHP(HP);
	this->SetAttackUpToTime(AttackUpToTimeValue);
	this->SetGoalLocations(SetLocations);
	this->SetMoveTime(MoveTimeValue);
}