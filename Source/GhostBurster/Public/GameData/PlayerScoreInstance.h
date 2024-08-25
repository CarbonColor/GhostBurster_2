// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PlayerScoreInstance.generated.h"

//------------------------------------------------------------
// �v���C���[�̃X�R�A���i�[����C���X�^���X
//------------------------------------------------------------

UCLASS()
class GHOSTBURSTER_API UPlayerScoreInstance : public UGameInstance
{
	GENERATED_BODY()

private:

	// �v���C���[�̃X�R�A
	UPROPERTY()
	int32 PlayerScore;

	// �_���[�W�J�E���g
	UPROPERTY()
	int32 PlayerDamageCount;

public:

	// �X�R�A���擾���郁�\�b�h
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	int32 GetPlayerScore();
	// �X�R�A�����������郁�\�b�h
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	void ResetPlayerScore();
	// �X�R�A�����Z���郁�\�b�h
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	void AddPlayerScore(int32 Value);

	// �_���[�W�J�E���g���擾���郁�\�b�h
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	int32 GetPlayerDamageCount();
	// �_���[�W�J�E���g�����������郁�\�b�h
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	void ResetPlayerDamageCount();
	// �_���[�W�J�E���g�����Z���郁�\�b�h
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	void AddPlayerDamageCount();

};
