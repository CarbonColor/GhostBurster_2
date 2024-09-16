// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "TreasureBox.generated.h"

UENUM(BlueprintType)
enum class ETreasureBoxDirection : uint8
{
	//列挙子を記述する
	Y = 0	UMETA(DisplayName = "Y (0)"),
	X_ = 1	UMETA(DisplayName = "-X (90)"),
	Y_ = 2	UMETA(DisplayName = "-Y (180)"),
	X = 3	UMETA(DisplayName = "X (270)")
};


UCLASS()
class GHOSTBURSTER_API ATreasureBox : public AActor
{
	GENERATED_BODY()

	//宝箱の体力
	UPROPERTY(EditAnywhere, Category = "HP")
	int32 HP;
	
public:	
	// Sets default values for this actor's properties
	ATreasureBox();

	UPROPERTY(EditAnywhere, Category = "Angle")
	ETreasureBoxDirection TreasureAngle;

	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USceneComponent> DefaultComponent;

	UPROPERTY(VisibleAnywhere)
		TObjectPtr<UStaticMeshComponent> TreasureBox;

	UPROPERTY(VisibleAnywhere)
		TObjectPtr<UStaticMeshComponent> TreasureCover;

	UPROPERTY(VisibleAnywhere)
		TObjectPtr<UBoxComponent> CollisionComponent;


	UPROPERTY(EditAnywhere, Category = "SE")
		TObjectPtr<USoundCue> OpenBoxSound;

	//蓋の目標座標
	UPROPERTY(EditAnywhere)
		FVector TargetCoverLocation;
	//蓋の目標角度
	UPROPERTY(EditAnywhere)
		FRotator TargetCoverRotation;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//蓋に攻撃を当てる関数
	UFUNCTION(BlueprintCallable, Category = "Treasure")
		void OpenChargeBox();

	//蓋を開ける関数
	UFUNCTION(BlueprintCallable, Category = "Treasure")
		void OpenTreasureBox();


private:

	//アイテムのブループリントを入れる変数
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> ItemActor;

	//蓋の初期座標
	FVector InitialCoverLocation;
	//蓋の初期角度
	FRotator InitialCoverRotation;

	//アイテムの座標プリセット
	FVector ItemLocationPreset;

	//すでに開けたかどうか
	bool bIsOpened;

	//タイムライン
	FTimeline CoverTimeline;

	//タイムラインのカーブ
	UPROPERTY(EditAnywhere, Category = "Timeline")
		TObjectPtr<UCurveFloat> CoverRotation;

	UPROPERTY(EditAnywhere, Category = "Timeline")
		TObjectPtr<UCurveFloat> CoverLocationY;

	UPROPERTY(EditAnywhere, Category = "Timeline")
		TObjectPtr<UCurveFloat> CoverLocationZ;

	//タイムラインの更新
	UFUNCTION()
		void HandleCoverProgress(float Value);

	//タイムラインの完了処理
	UFUNCTION()
		void OnTimelineFinished();

};
