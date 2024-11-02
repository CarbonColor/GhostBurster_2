// Fill out your copyright notice in the Description page of Project Settings.


#include "Sound/GameBGM.h"

// Sets default values
AGameBGM::AGameBGM()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// VRRoot�R���|�[�l���g�����
	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	// Root�R���|�[�l���g�ɐݒ�
	RootComponent = DefaultSceneComponent;

	//�I�[�f�B�I�R���|�[�l���g�̍쐬
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;     //�����Đ��𖳌��ɂ���
	AudioComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AGameBGM::BeginPlay()
{
	Super::BeginPlay();

	BGMIndex = 0;
	FadeTime = 3.5f;

	if (BGM_Cue.Num() <= BGMIndex || BGM_Cue.Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Cue is Null or Over"));
	}
	else
	{
		AudioComponent->SetSound(BGM_Cue[BGMIndex]);
	}
	AudioComponent->Play();
}

void AGameBGM::ChangeBGM()
{
	BGMIndex++;

	if (BGM_Cue.Num() > BGMIndex)
	{
		//BGM�̃t�F�[�h�A�E�g
		AudioComponent->FadeOut(FadeTime, 0.0f);
	
		//Boss���O�Ȃ璷���ԉ����~�߂�
		if (BGMIndex == 5)
		{
			//-----------------------------------------------------------------
			//5�X�e�[�W�ڏI���`�{�X�o�g���J�n�܂ł̎��Ԃ��v��
			//-----------------------------------------------------------------
			GetWorld()->GetTimerManager().SetTimer(FadeEndHandle, this, &AGameBGM::NextBGM, 22.0f, false);
		}
		else
		{
			//2�b���BGM�̐؂�ւ�
			GetWorld()->GetTimerManager().SetTimer(FadeEndHandle, this, &AGameBGM::NextBGM, FadeTime, false);
		}
	
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Silver, TEXT("TimerHandle Called"));
	}
	else
	{
		FadeTime = 8.0f;
		//BGM�̃t�F�[�h�A�E�g
		AudioComponent->FadeOut(FadeTime, 0.0f);

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Silver, TEXT("Final BGM"));
	}
}

void AGameBGM::NextBGM()
{
	GetWorld()->GetTimerManager().ClearTimer(FadeEndHandle);
	//BGM�̕ύX
	AudioComponent->SetSound(BGM_Cue[BGMIndex]);
	//�t�F�[�h�C�����čĐ�
	AudioComponent->FadeIn(FadeTime, 1.0f);

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Silver, TEXT("BGM Changed"));
}

//// Called every frame
//void AGameBGM::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}
//
