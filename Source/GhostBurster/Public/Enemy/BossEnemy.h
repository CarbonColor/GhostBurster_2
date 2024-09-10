// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemys.h"
#include "Components/SphereComponent.h"
#include "BossEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GHOSTBURSTER_API ABossEnemy : public AEnemys
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
	virtual void ProcessJustForFirst_Move() override {} // ���Move�J�ڎ��ɂ̂ݍs������
	virtual bool Move() override { return false; }		// �ړ�

	//�o���֌W---------------------------------------------------------------------------------------------------------------------
	virtual void ProcessJustForFirst_Appear() override {}	// ��ԁFAppear�ōŏ��Ɉ�x�����s������
	virtual bool Appear() { return false; }					// �G�o������

protected:
	virtual void BeginPlay() override;

public:
	//�R���X�g���N�^
	ABossEnemy();

	//Tick
	virtual void Tick(float DeltaTime) override;

	//���֐��錾
	//�_���[�W���󂯂鏈���A�����ł�������U���͕��̗͂����炷
	virtual void RecieveEnemyDamage(int DamageAmount) override;

	//�v���C���[�̃��C�g�̐F�ƓG�̃��C�g�̐F���`�F�b�N���鏈��
	virtual bool CheckPlayerLightColor(EFlashlight_Color PlayerColor) const override;
};
