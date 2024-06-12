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

	//SceneComponent�̕ϐ��錾
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	//StaticMeshComponent�̕ϐ��錾
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> GhostMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//�ǉ��ϐ��E�֐�
private:
	//���\����
	//�X�e�[�^�X
	struct Status
	{
		float HP = 100;	//�S�[�X�g�̗̑�
	};
	Status status;

	//���ϐ��錾
	int MoveCount = 0;		//�S�[�X�g�̍s������p�̃J�E���g
	int Gamefps = 60;	//�Q�[����fps���l��ݒ�

	//���֐��錾
	//HP��0�ɂȂ�������ł�����
	void EnemyDead();

public:
	//���֐��錾
	//�o�����Ă���5�b��������U��������
	void Attack();

	//�_���[�W���󂯂鏈���A�����ł�������U���͕��̗͂����炷
	void Damage(float damage);

};
