// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawn.generated.h"

UCLASS()
class GHOSTBURSTER_API ASpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawn();
    // イベントディスパッチャーの宣言
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpawnEvent);
    UPROPERTY(BlueprintAssignable)
    FSpawnEvent OnSpawn;

    // スポーン関数の宣言
    UFUNCTION(BlueprintCallable)
    void SpawnEnemies(FVector SpawnLocation, FRotator SpawnRotation, int32 NumEnemies);

protected:
    virtual void BeginPlay() override;
};
