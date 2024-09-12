// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/VRPlayerCharacter.h"
#include "Interface/DamageInterface.h"
#include "Components/SphereComponent.h"
#include "TitleEnemy.generated.h"

UCLASS()
class GHOSTBURSTER_API ATitleEnemy : public AActor, public IDamageInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATitleEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	int32 HP;
	float RunningTime;
	FVector InitialLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMeshComponent> GhostMeshComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> GhostCollision;

	FRotator	RotationCorrectionValue;	// ��]�̕␳�l
	FVector		EnemyScale;					// �G�̃X�P�[��

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void EnemyDead();

	//�v���C���[�̃��C�g�̐F�ƓG�̃��C�g�̐F���`�F�b�N����֐�
	virtual bool CheckPlayerLightColor(EFlashlight_Color PlayerColor) const PURE_VIRTUAL(ATitleEnemy::CheckPlayerLightColor, return false;);

private:

	//�X�R�A�̃C���X�^���X
	UPROPERTY()
		TObjectPtr<AVRPlayerCharacter> Player;

};
