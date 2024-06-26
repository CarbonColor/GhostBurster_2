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
	
	//���֐��錾
	//�G�l�~�[�̏�Ԕ��f
	virtual void Think() override;

	//��ԂɊ�Â�������������
	virtual void Move() override;

protected:
	virtual void BeginPlay() override;

public:
	//�R���X�g���N�^
	AGreenEnemy();

	//Tick
	virtual void Tick(float DeltaTime) override;

	//���ϐ��錾
	//SceneComponent�̕ϐ��錾
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	//StaticMeshComponent�̕ϐ��錾
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> GhostMesh;

	//�R���W�����̕ϐ��錾
	UPROPERTY(EditAnywhere)
	USphereComponent* GhostCollision;

	//���֐��錾
	//�_���[�W���󂯂鏈���A�����ł�������U���͕��̗͂����炷
	virtual void RecieveEnemyDamage(int DamageAmount) override;
};
