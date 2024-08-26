// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/PlayerSplinePath.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerSplinePath::APlayerSplinePath()
{
	PrimaryActorTick.bCanEverTick = true;

	// スプラインコンポーネントの作成
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponent;

	// スプライン経路の現在地の初期化
	CurrentSplineDistance = 0.0f; // スプラインが始まってからの距離。0が始点位置になる
	MovementSpeed = DefaultSpeed; // 移動速度。あとで酔わない程度に変更する

	// スプラインのポイントの設定
	SplineComponent->AddSplinePoint(FVector(0, 100.0f, 200.0f), ESplineCoordinateSpace::World);				//スタート地点
	SplineComponent->AddSplinePoint(FVector(6800.0f, 100.0f, 200.0f), ESplineCoordinateSpace::World);		//左に曲がる地点
	SplineComponent->AddSplinePoint(FVector(6800.0f, -2850.0f, 200.0f), ESplineCoordinateSpace::World);		//階段を下りる直前
	SplineComponent->AddSplinePoint(FVector(6800.0f, -3650.0f, -600.0f), ESplineCoordinateSpace::World);	//階段を下りた直後
	SplineComponent->AddSplinePoint(FVector(6800.0f, -8200.0f, -600.0f), ESplineCoordinateSpace::World);	//ゴール地点

	// スプラインのタイプの設定
	for (int i = 0; i < SplineComponent->GetNumberOfSplinePoints(); ++i)
	{
		SplineComponent->SetSplinePointType(i, ESplinePointType::Linear);	//移動経路が直線になるように設定
	}

	//回転変数の初期化
	bIsRotating = false;
	RotationDuration = 3.0f;
	CurrentRotationTime = 0.0f;
	bIsRotatePoint = false;
	bIsStairsPoint = false;
}

// Called when the game starts or when spawned
void APlayerSplinePath::BeginPlay()
{
	Super::BeginPlay();

	//プレイヤーポーンを取得
	PlayerCharacter = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	//カメラを取得
	Camera = PlayerCharacter->FindComponentByClass<UCameraComponent>();
}

// Called every frame
void APlayerSplinePath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//回転処理
	if (bIsRotating)
	{
		CurrentRotationTime += DeltaTime;
		float Alpha = FMath::Clamp(CurrentRotationTime / RotationDuration, 0.0f, 1.0f);

		FRotator NewRotation = FMath::Lerp(StartRotation, TargetRotation, Alpha);
		PlayerCharacter->SetActorRotation(NewRotation);
		Camera->SetWorldRotation(NewRotation);
		if (Alpha >= 1.0f)
		{
			//回転が完了したら、移動を再開
			bIsRotating = false;
			StartMovement();
		}
	}
	else
	{
		MoveAlongSpline(DeltaTime);
	}
}

void APlayerSplinePath::SetMovementSpeed(float Speed)
{
	MovementSpeed = Speed;
}

void APlayerSplinePath::StopMovement()
{
	MovementSpeed = 0.0f;
}

void APlayerSplinePath::StartMovement()
{
	MovementSpeed = DefaultSpeed; // 初期値に設定
}

FVector APlayerSplinePath::GetLocationAtCurrentDistance() const
{
	return SplineComponent->GetLocationAtDistanceAlongSpline(CurrentSplineDistance, ESplineCoordinateSpace::World);
}

void APlayerSplinePath::MoveAlongSpline(float DeltaTime)
{
	if (MovementSpeed > 0.0f)
	{
		CurrentSplineDistance += MovementSpeed * DeltaTime;
		FVector NewLocation = SplineComponent->GetLocationAtDistanceAlongSpline(CurrentSplineDistance, ESplineCoordinateSpace::World);

		// VRPlayerCharacter を新しい位置に移動
		if (PlayerCharacter)
		{
			PlayerCharacter->SetActorLocation(NewLocation);

			//左に曲がる地点に到達したかチェック
			if (bIsRotatePoint == false)
			{
				if (CurrentSplineDistance >= SplineComponent->GetDistanceAlongSplineAtSplinePoint(1))
				{
					//回転処理の準備
					bIsRotating = true;
					bIsRotatePoint = true;
					StartRotation = PlayerCharacter->GetActorRotation();
					TargetRotation = StartRotation + FRotator(0.0f, -90.0f, 0.0f);
					CurrentRotationTime = 0.0f;

					//移動を止める
					StopMovement();
				}
			}
			//階段を下りる地点に到達したかチェック
			if (bIsRotatePoint == false)
			{
				if (CurrentSplineDistance >= SplineComponent->GetDistanceAlongSplineAtSplinePoint(2))
				{
					//速度を半分に落とす
					bIsStairsPoint = true;
					SetMovementSpeed(DefaultSpeed / 2.0f);
				}
			}
			//階段を下りきった地点に到達したかチェック
			else
			{
				if (CurrentSplineDistance >= SplineComponent->GetDistanceAlongSplineAtSplinePoint(3))
				{
					//速度をもとに戻す
					SetMovementSpeed(DefaultSpeed);
				}
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, TEXT("Player is none"));
		}
	}
}