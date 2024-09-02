// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/PlayerScoreInstance.h"
#include "GenericPlatform/GenericPlatformTime.h"

// ------------------------------------------------------------------------------------
// �e�f�[�^�̃��Z�b�g
// ------------------------------------------------------------------------------------
void UPlayerScoreInstance::AllDataResetFunction()
{
	PlayerScore = 0;
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
int32 UPlayerScoreInstance::GetPlayerScore()
{
	return PlayerScore;
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


// ------------------------------------------------------------------------------------
// �e�f�[�^�̃Z�b�^�[
// ------------------------------------------------------------------------------------
void UPlayerScoreInstance::AddPlayerScore(int32 Value)
{
	PlayerScore += Value;
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
// �X�R�A�v�Z�Ȃ�
// ------------------------------------------------------------------------------------
void UPlayerScoreInstance::ConvertItemToScore_Title()
{
	AddPlayerScore(ItemScore);
	UsePlayerItem();
}

int32 UPlayerScoreInstance::ConvertItemToScore()
{
	UsePlayerItem();
	return ItemScore;
}

TArray<int32> UPlayerScoreInstance::ScoringFunction()
{
	//------------------------------------------------------------
	// ���^�[���p
	//------------------------------------------------------------
	TArray<int32> Data;
	Data.Empty();

	//------------------------------------------------------------
	//�f�t�H���g�X�R�A
	//------------------------------------------------------------
	Data.Add(PlayerScore);

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
	int32 Tmp_TimeScore = TimeMaxScore - (TimeDecreaseScore * Multiple);
	Data.Add(Tmp_TimeScore);

	//------------------------------------------------------------
	//�_���[�W�X�R�A
	//------------------------------------------------------------
	int32 Tmp_DamageScore = DamageScore * PlayerDamageCount;
	Data.Add(Tmp_DamageScore);

	//------------------------------------------------------------
	//�A�C�e���X�R�A
	//------------------------------------------------------------
	int32 Tmp_ItemScore = 0;
	while (PlayerItem > 0)
	{
		Tmp_ItemScore += ConvertItemToScore();
	}
	Data.Add(Tmp_ItemScore);


	return Data;
}
