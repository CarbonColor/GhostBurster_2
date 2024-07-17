// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemys.h"

#include "NormalEnemy.generated.h"

UCLASS()
class GHOSTBURSTER_API ANormalEnemy : public AEnemys
{
	GENERATED_BODY()

	//���֐��錾
	//�G�l�~�[�̏�Ԕ��f
	virtual void Think() override;

	//��ԂɊ�Â�������������
	virtual void Move() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	//�R���X�g���N�^
	ANormalEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//���֐��錾
	//�_���[�W���󂯂鏈���A�����ł�������U���͕��̗͂����炷
	virtual void RecieveEnemyDamage(int DamageAmount, EFlashlight_Color Color) override;
};
