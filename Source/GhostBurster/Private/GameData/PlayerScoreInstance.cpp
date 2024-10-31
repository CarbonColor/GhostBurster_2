// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/PlayerScoreInstance.h"
#include "GenericPlatform/GenericPlatformTime.h"

// ------------------------------------------------------------------------------------
// �R���X�g���N�^
// ------------------------------------------------------------------------------------
UPlayerScoreInstance::UPlayerScoreInstance()
{
	FingerBorder = { 0, 0, 0, 0, 0 };
}

// ------------------------------------------------------------------------------------
// �e�f�[�^�̃��Z�b�g
// ------------------------------------------------------------------------------------
void UPlayerScoreInstance::AllDataResetFunction()
{
	PlayerDafaultScore = 0;
	PlayerDamageScore = 0;
	PlayerTimeScore = 0;
	PlayerItemScore = 0;
	TotalScore = 0;

	PlayerDamageCount = 0;
	StartTime = 0;
	EndTime = 0;
	Minute = 0;
	Second = 0;
	PlayerItem = 4;
}

// ------------------------------------------------------------------------------------
// �e�f�[�^�̃Q�b�^�[
// ------------------------------------------------------------------------------------
int32 UPlayerScoreInstance::GetPlayerDefaultScore()
{
	return PlayerDafaultScore;
}
int32 UPlayerScoreInstance::GetPlayerTimeScore()
{
	return PlayerTimeScore;
}
int32 UPlayerScoreInstance::GetPlayerDamageScore()
{
	return PlayerDamageScore;
}
int32 UPlayerScoreInstance::GetPlayerItemScore()
{
	return PlayerItemScore;
}
int32 UPlayerScoreInstance::GetTotalScore()
{
	return TotalScore;
}

int32 UPlayerScoreInstance::GetPlayerDamageCount()
{
	return PlayerDamageCount;
}

int32 UPlayerScoreInstance::GetPlayerItemCount()
{
	return PlayerItem;
}

int32 UPlayerScoreInstance::GetGameTime_Minute()
{
	return Minute;
}

int32 UPlayerScoreInstance::GetGameTime_Second()
{
	return Second;
}

int32 UPlayerScoreInstance::GetItemPerScore()
{
	return ItemScore;
}

// ------------------------------------------------------------------------------------
// �e�f�[�^�̃Z�b�^�[
// ------------------------------------------------------------------------------------
void UPlayerScoreInstance::SetDevicePortNumber(const int32 Port)
{
	DevicePortNumber = Port;
}

void UPlayerScoreInstance::SetFingerBorder(const TArray<int32> NormalFinger, const TArray<int32> FoxFinger, const TArray<int32> GunFinger)
{
	int32 Thumb = 0, Index = 0, Middle = 0, Ring = 0, Little = 0;

	/*
	* �Ȃ���w���i�Ȃ��w�̍ŏ��l + �L�юw�̍ő�l�j/ 2
	* �Ȃ��Ȃ��w���i�L�юw�̍ő�l�j+ 100�`200
	*/

	//�e�w�̃{�[�_�[����
	if (NormalFinger[0] > GunFinger[0])
	{
		//�ʏ펞�̕����l���傫���ꍇ
		Thumb = (FoxFinger[0] + NormalFinger[0]) / 2;
	}
	else
	{
		//�e�̕����l���傫���ꍇ
		Thumb = (FoxFinger[0] + GunFinger[0]) / 2;
	}

	//�l�����w�̃{�[�_�[����
	Index = FMath::Max3(NormalFinger[1], FoxFinger[1], GunFinger[1]) + 150;

	//���w�̃{�[�_�[����
	if (FoxFinger[2] < GunFinger[2])
	{
		//�ς̕����l����������
		Middle = (FoxFinger[2] + NormalFinger[2]) / 2;
	}
	else
	{
		//�e�̕����l����������
		Middle = (GunFinger[2] + NormalFinger[2]) / 2;
	}

	//��w�̃{�[�_�[����
	if (FoxFinger[3] < GunFinger[3])
	{
		//�ς̕����l����������
		Ring = (FoxFinger[3] + NormalFinger[3]) / 2;
	}
	else
	{
		//�e�̕����l����������
		Ring = (GunFinger[3] + NormalFinger[3]) / 2;
	}

	//���w�̃{�[�_�[����
	if (NormalFinger[4] > FoxFinger[4])
	{
		//�ʏ펞�̕����l���傫���ꍇ
		Little = (GunFinger[4] + NormalFinger[4]) / 2;
	}
	else
	{
		//�e�̕����l���傫���ꍇ
		Little = (GunFinger[4] + FoxFinger[4]) / 2;
	}

	//�{�[�_�[�̃Z�b�g
	FingerBorder[0] = Thumb;
	FingerBorder[1] = Index;
	FingerBorder[2] = Middle;
	FingerBorder[3] = Ring;
	FingerBorder[4] = Little;

	FString String = FString::FromInt(Thumb) + ", " + FString::FromInt(Index) + ", " + FString::FromInt(Middle) + ", " + FString::FromInt(Ring) + ", " + FString::FromInt(Little);

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, String);

}


void UPlayerScoreInstance::AddPlayerScore(int32 Value)
{
	PlayerDafaultScore += Value;
}

void UPlayerScoreInstance::AddPlayerDamageCount()
{
	PlayerDamageCount++;
}

void UPlayerScoreInstance::AddPlayerItem()
{
	PlayerItem++;
}

void UPlayerScoreInstance::UsePlayerItem()
{
	PlayerItem--;
}

void UPlayerScoreInstance::SetGameStartTime()
{
	StartTime = FPlatformTime::Seconds();
}

void UPlayerScoreInstance::SetGameEndTime()
{
	EndTime = FPlatformTime::Seconds();
}

// ------------------------------------------------------------------------------------
// �X�R�A�v�Z
// ------------------------------------------------------------------------------------
void UPlayerScoreInstance::ScoringFunction()
{
	//------------------------------------------------------------
	//�N���A�^�C���X�R�A
	//------------------------------------------------------------
	int32 ClearTime = FMath::FloorToInt(EndTime - StartTime);
	Minute = ClearTime / 60;
	Second = ClearTime % 60;
	//�ő��N���A�^�C���������炷
	ClearTime -= DefaultClearTime;
	//���_�{�������߂�
	int32 Multiple = 0;
	if (ClearTime > 0)
	{
		Multiple = (ClearTime / DecreasePer) + 1;
	}
	//�X�R�A�v�Z������
	PlayerTimeScore = TimeMaxScore - (TimeDecreaseScore * Multiple);

	//------------------------------------------------------------
	//�_���[�W�X�R�A
	//------------------------------------------------------------
	PlayerDamageScore = DamageScore * PlayerDamageCount;

	//------------------------------------------------------------
	//�A�C�e���X�R�A
	//------------------------------------------------------------
	PlayerItemScore = 0;
	for (int i = 0; i < PlayerItem; ++i)
	{
		PlayerItemScore += ItemScore;
	}


	//------------------------------------------------------------
	//���v�X�R�A
	//------------------------------------------------------------
	TotalScore = PlayerDafaultScore + PlayerTimeScore + PlayerDamageScore + PlayerItemScore;

}
