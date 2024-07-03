// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemys.h"
#include "EnemySpawn1.generated.h"

UCLASS()
class GHOSTBURSTER_API AEnemySpawn1 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawn1();

	// イベントディスパッチャーの宣言
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpawnEvent);
	UPROPERTY(BlueprintAssignable)
	FSpawnEvent OnSpawnEnemy1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// スポーン関数の宣言
	UFUNCTION()
	void HandleSpawnEvent();
	void SpawnEnemies(FVector SpawnLocation, FRotator SpawnRotation, int32 NumEnemies);
};
