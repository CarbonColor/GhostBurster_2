// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NormalEnemy.generated.h"

UCLASS()
class GHOSTBURSTER_API ANormalEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANormalEnemy();

	//SceneComponentの変数宣言
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	//StaticMeshComponentの変数宣言
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> GhostMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//追加変数・関数
private:
	//☆構造体
	//ステータス
	struct Status
	{
		float HP = 100;	//ゴーストの体力
	};
	Status status;

	//☆変数宣言
	int MoveCount = 0;		//ゴーストの行動制御用のカウント
	int Gamefps = 60;	//ゲームのfps数値を設定

	//☆関数宣言
	//HPが0になったら消滅させる
	void EnemyDead();

public:
	//☆関数宣言
	//出現してから5秒たったら攻撃させる
	void Attack();

	//ダメージを受ける処理、引数でもらった攻撃力分体力を減らす
	void Damage(float damage);

};
