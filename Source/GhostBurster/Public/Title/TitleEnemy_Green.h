// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TitleEnemy.h"
#include "TitleEnemy_Green.generated.h"

UCLASS()
class GHOSTBURSTER_API ATitleEnemy_Green : public ATitleEnemy
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATitleEnemy_Green();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void RecieveEnemyDamage(int DamageAmount) override;

	//�v���C���[�̃��C�g�̐F�ƓG�̃��C�g�̐F���`�F�b�N���鏈��
	virtual bool CheckPlayerLightColor(EFlashlight_Color PlayerColor) const override;

};
