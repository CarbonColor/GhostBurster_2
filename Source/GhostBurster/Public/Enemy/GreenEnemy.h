// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemys.h"
#include "Components/SphereComponent.h"
#include "GreenEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GHOSTBURSTER_API AGreenEnemy : public AEnemys
{
	GENERATED_BODY()
	
	//☆関数宣言
	//エネミーの状態判断
	virtual void Think() override;

	//状態に基づいた動きをする
	virtual void Move() override;

protected:
	virtual void BeginPlay() override;

public:
	//コンストラクタ
	AGreenEnemy();

	//Tick
	virtual void Tick(float DeltaTime) override;

	//☆変数宣言
	//SceneComponentの変数宣言
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	//StaticMeshComponentの変数宣言
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> GhostMesh;

	//コリジョンの変数宣言
	UPROPERTY(EditAnywhere)
	USphereComponent* GhostCollision;

	//☆関数宣言
	//ダメージを受ける処理、引数でもらった攻撃力分体力を減らす
	virtual void RecieveEnemyDamage(int DamageAmount) override;
};
