// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/VRPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/SplineComponent.h"
#include "PlayerSplinePath.generated.h"

UCLASS()
class GHOSTBURSTER_API APlayerSplinePath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerSplinePath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
		TObjectPtr<USplineComponent> SplineComponent;

	//経路上の現在地を0～1で表す
	float CurrentSplineDistance;
	//移動速度
	const float DefaultSpeed = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Speed")
	float MovementSpeed;


	//プレイヤー
	UPROPERTY()
		TObjectPtr<AVRPlayerCharacter> PlayerCharacter;

	//カメラ
	UPROPERTY()
		TObjectPtr<UCameraComponent> Camera;

	//回転処理用変数
	bool bIsRotating;			//回転フラグ
	float RotationDuration;		//回転時間（秒）
	float CurrentRotationTime;	//現在の回転時間
	FRotator StartRotation;		//回転開始時の角度
	FRotator TargetRotation;	//目標の角度

	bool bIsRotatePoint;	//回転場所で回転したかどうか
	bool bIsStairsPoint;	//階段を下りる場所かどうか


public:
	UFUNCTION(BlueprintCallable, Category = "Spline")
		void SetMovementSpeed(float Speed);

	UFUNCTION(BlueprintCallable, Category = "Spline")
		void StopMovement();

	UFUNCTION(BlueprintCallable, Category = "Spline")
		void StartMovement();

	UFUNCTION(BlueprintCallable, Category = "Spline")
		FVector GetLocationAtCurrentDistance() const;

	UFUNCTION(BlueprintCallable, Category = "Spline")
		void MoveAlongSpline(float DeltaTime);

};
