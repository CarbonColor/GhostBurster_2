// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/PlayerSplinePath.h"
#include "Components/SplineComponent.h"
#include "Player/VRPlayerCharacter.h"
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
	MovementSpeed = 300.0f; // 移動速度。あとで酔わない程度に変更する
}

// Called when the game starts or when spawned
void APlayerSplinePath::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerSplinePath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveAlongSpline(DeltaTime);
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
	MovementSpeed = 300.0f; // 初期値に設定
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
		if (AVRPlayerCharacter* PlayerCharacter = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
		{
			PlayerCharacter->SetActorLocation(NewLocation);
		}
	}
}