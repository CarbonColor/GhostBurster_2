// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TreasureBox.h"

// Sets default values
ATreasureBox::ATreasureBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// VRRootコンポーネントを作る
	DefaultComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultComponent"));
	// Rootコンポーネントに設定
	RootComponent = DefaultComponent;

	//スタティックメッシュの読み込み
	TreasureBox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TreasureBox"));
	UStaticMesh* Box = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/_TeamFolder/Player/SM_Box"));
	TreasureBox->SetStaticMesh(Box);
	TreasureBox->SetupAttachment(DefaultComponent);
	TreasureCover = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TreasureCover"));
	UStaticMesh * Cover = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/_TeamFolder/Player/SM_Cover"));
	TreasureCover->SetStaticMesh(Cover);
	TreasureCover->SetupAttachment(DefaultComponent);

	//当たり判定の作成
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	CollisionComponent->SetupAttachment(DefaultComponent);
	
}

// Called when the game starts or when spawned
void ATreasureBox::BeginPlay()
{
	Super::BeginPlay();

	bIsOpened = false;

	//座標の保存
	InitialCoverLocation = TreasureCover->GetRelativeLocation();
	InitialCoverRotation = TreasureCover->GetRelativeRotation();

	//向きによってアイテム出現位置のプリセットを変える
	switch (TreasureAngle)
	{
	case ETreasureBoxDirection::Y:
		ItemLocationPreset = FVector(0.0f, 35.0f, 0.0f);
		break;

	case ETreasureBoxDirection::X_:
		ItemLocationPreset = FVector(-35.0f, 0.0f, 0.0f);
		break;

	case ETreasureBoxDirection::Y_:
		ItemLocationPreset = FVector(0.0f, -35.0f, 0.0f);
		break;

	case ETreasureBoxDirection::X:
		ItemLocationPreset = FVector(35.0f, 0.0f, 0.0f);
		break;
	}
	
	//タイムラインのセッティング
	if (CoverRotation && CoverLocationY && CoverLocationZ)
	{
		// カーブをタイムラインにバインド
		CoverTimeline.AddInterpFloat(CoverRotation, FOnTimelineFloatStatic::CreateUFunction(this, FName("HandleCoverProgress")));
		CoverTimeline.AddInterpFloat(CoverLocationY, FOnTimelineFloatStatic::CreateUFunction(this, FName("HandleCoverProgress")));
		CoverTimeline.AddInterpFloat(CoverLocationZ, FOnTimelineFloatStatic::CreateUFunction(this, FName("HandleCoverProgress")));

		// タイムライン完了時の処理をバインド
		CoverTimeline.SetTimelineFinishedFunc(FOnTimelineEventStatic::CreateUFunction(this, FName("OnTimelineFinished")));

		// タイムラインの長さを設定 (0.5秒)
		CoverTimeline.SetTimelineLength(0.5f);
	}

}

// Called every frame
void ATreasureBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CoverTimeline.IsPlaying())
	{
		CoverTimeline.TickTimeline(DeltaTime);
	}
}

void ATreasureBox::HandleCoverProgress(float Value)
{
	FRotator NewRotation = UKismetMathLibrary::RLerp(InitialCoverRotation, TargetCoverRotation, Value, true);
	FVector NewLocation = InitialCoverLocation + FVector(0.f, Value * TargetCoverLocation.Y, Value * TargetCoverLocation.Z);

	TreasureCover->SetRelativeLocation(NewLocation);
	TreasureCover->SetRelativeRotation(NewRotation);
}

void ATreasureBox::OpenTreasureBox()
{
	bIsOpened = true;
	if (OpenBoxSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, OpenBoxSound, GetActorLocation());
	}

	if (CoverTimeline.IsPlaying() == false)
	{
		CoverTimeline.PlayFromStart();
	}
}

void ATreasureBox::OnTimelineFinished()
{
	if (ItemActor)
	{
		GetWorld()->SpawnActor<AActor>(ItemActor, GetActorLocation() + ItemLocationPreset, FRotator::ZeroRotator);
	}
}

bool ATreasureBox::IsOpenedTreasure()
{
	return bIsOpened;
}