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
	//Tick�ł̏���
	virtual void TickProcess() override;
	
	//�G�l�~�[�̏�Ԕ��f
	virtual void Think() override;

	//��ԂɊ�Â�������������
	virtual void ActProcess() override;

	//�ړ��֌W
	virtual bool ProcessJustForFirst_Move() override;	// ���Move�J�ڎ��ɂ̂ݍs������
	virtual bool Move() override;						// �ړ�����

	//�U���֌W
	virtual bool Attack() override;	// �U������

	//�o���֌W
	virtual bool Appear() override;	// �G�o������

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	//�R���X�g���N�^
	ANormalEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//���֐��錾
	//�_���[�W���󂯂鏈���A�����ł���������l���̗͂����炷
	virtual void RecieveEnemyDamage(int DamageAmount, EFlashlight_Color Color) override;

	//�A�C�e���Ń_���[�W���󂯂鏈���A�����ł���������l���̗͂����炷
	virtual void RecieveItemDamage(int DamageAmount) override;
};
