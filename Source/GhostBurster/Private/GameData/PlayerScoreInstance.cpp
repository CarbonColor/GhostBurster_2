// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/PlayerScoreInstance.h"
#include "GenericPlatform/GenericPlatformTime.h"

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
	PlayerItem = 3;
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
void UPlayerScoreInstance::SetDevicePortNumber(int32 Port)
{
	DevicePortNumber = Port;
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
	int32 Multiple = ClearTime / DecreasePer;
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
