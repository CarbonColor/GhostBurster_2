// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TitleEnemy.h"
#include "TitleEnemy_White.generated.h"

UCLASS()
class GHOSTBURSTER_API ATitleEnemy_White : public ATitleEnemy
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATitleEnemy_White();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void RecieveEnemyDamage(int DamageAmount) override;

	UFUNCTION(BlueprintCallable)
	void SetHP(int32 Value);

};
