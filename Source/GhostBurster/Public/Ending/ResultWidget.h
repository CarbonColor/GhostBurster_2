// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "GameData/PlayerScoreInstance.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "ResultWidget.generated.h"

UCLASS()
class GHOSTBURSTER_API AResultWidget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AResultWidget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	//// Called every frame
	//virtual void Tick(float DeltaTime) override;

	//�E�B�W�F�b�g�R���|�[�l���g
	UPROPERTY(EditAnywhere, Category = "UI")
		TObjectPtr<UWidgetComponent> ResultWidgetComponent;

private:
	//�V�[���R���|�[�l���g
	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USceneComponent> DefaultSceneComponent;

	//���U���g��UI
	UPROPERTY()
		TObjectPtr<UUserWidget> ResultWidget;

	//�X�R�A�̃C���X�^���X
	UPROPERTY()
		TObjectPtr<UPlayerScoreInstance> ScoreInstance;

	//�e�L�X�gUI
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> DefaultScoreText;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> ClearTimeText;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> ClearTimeScoreText;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> DamageText;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> DamageScoreText;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> ItemText;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> ItemScoreText;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> Static_TotalScoreText;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> TotalScoreText;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> RankText;

	//�\�����^�C�}�[�n���h��
	FTimerHandle FirstView;		//�f�t�H���g�X�R�A
	FTimerHandle SecondView;	//�N���A�^�C��
	FTimerHandle ThirdView;		//�_���[�W�X�R�A
	FTimerHandle FourthView;	//�A�C�e���X�R�A
	FTimerHandle FifthView;		//�g�[�^���X�R�A
	FTimerHandle SixthView;		//�����N

	//�^�C�}�[�p���\�b�h
	void FirstViewFunction();
	void SecondViewFunction();
	void ThirdViewFunction();
	void FourthViewFunction();
	void FifthViewFunction();
	void SixthViewFunction();

	TMap<int32, FString> ScoreBorder;

};
