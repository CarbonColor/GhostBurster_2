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

	// ------------------------------------------------------------------------------------	
	//�_���[�W�̃X�R�A
	const int32 DamageScore = -100;
	//�^�C���̃X�R�A
	const int32 DefaultClearTime = 0;
	const int32 TimeMaxScore = 10000;
	const int32 TimeDecreaseScore = 80;
	const int32 DecreasePer = 45;
	//�A�C�e���̃X�R�A
	const int32 ItemScore = 2000;
	// ------------------------------------------------------------------------------------

	// �v���C���[�̃f�t�H���g�X�R�A
	UPROPERTY()
		int32 PlayerDafaultScore;

	// �v���C���[�̃N���A�^�C��
	UPROPERTY()
		double StartTime;
	UPROPERTY()
		double EndTime;
	UPROPERTY()
		int32 Minute;
	UPROPERTY()
		int32 Second;
	// �N���A�^�C���X�R�A
	UPROPERTY()
		int32 PlayerTimeScore;

	// �_���[�W�J�E���g
	UPROPERTY()
		int32 PlayerDamageCount;
	// �_���[�W�X�R�A
	UPROPERTY()
		int32 PlayerDamageScore;

	// �Q�[���I����̃A�C�e����
	UPROPERTY()
		int32 PlayerItem;
	// �A�C�e���X�R�A
	UPROPERTY()
		int32 PlayerItemScore;

	//���v�X�R�A
	UPROPERTY()
		int32 TotalScore;

protected:

public:

	//�S�ď��������郁�\�b�h
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		void AllDataResetFunction();

	//�O���[�u�f�o�C�X�p��COM�|�[�g�ԍ�
	UPROPERTY(BlueprintReadOnly, Category = "DeviceData")
		int32 DevicePortNumber;
	//�w�̋Ȃ���̃{�[�_�[
	UPROPERTY(BlueprintReadOnly, Category = "DeviceData")
		TArray<int32> FingerBorder;

	UFUNCTION(BlueprintCallable, Category = "DeviceData")
		void SetDevicePortNumber(const int32 Port);
	UFUNCTION(BlueprintCallable, Category = "DeviceData")
		void SetFingerBorder(const int32 Thumb, const int32 Index, const int32 Middle, const int32 Ring, const int32 Little);


	// �X�R�A���擾���郁�\�b�h
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetPlayerDefaultScore();
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetPlayerTimeScore();
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetPlayerDamageScore();
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetPlayerItemScore();
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetTotalScore();

	// �_���[�W�J�E���g���擾���郁�\�b�h
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetPlayerDamageCount();
	// �A�C�e�������擾���郁�\�b�h
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetPlayerItemCount();
	// �N���A�^�C���i���j���擾���郁�\�b�h
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetGameTime_Minute();
	// �N���A�^�C���i�b�j���擾���郁�\�b�h
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetGameTime_Second();
	// �A�C�e���X�R�A���擾���郁�\�b�h
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		int32 GetItemPerScore();

	// �X�R�A�����Z���郁�\�b�h
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		void AddPlayerScore(int32 Value);
	// �_���[�W�J�E���g�����Z���郁�\�b�h
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		void AddPlayerDamageCount();
	// �A�C�e���������Z���郁�\�b�h
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		void AddPlayerItem();
	// �A�C�e���������Z���郁�\�b�h
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		void UsePlayerItem();


	// �Q�[���X�^�[�g�̎��Ԃ��Z�b�g����
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		void SetGameStartTime();
	// �Q�[���G���h�̎��Ԃ��Z�b�g����
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		void SetGameEndTime();

	// �X�R�A���v�Z���āA���̊e�X�R�A�𑗂郁�\�b�h
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
		void ScoringFunction();

};
