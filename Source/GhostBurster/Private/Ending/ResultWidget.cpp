// Fill out your copyright notice in the Description page of Project Settings.


#include "Ending/ResultWidget.h"

// Sets default values
AResultWidget::AResultWidget()
{
	// ------------------------------------------------------------------------------------
	// �e�����N�{�[�_�[�̃Z�b�g
	// �X�e�[�W��̓G�@�F60��	���@7000�_
	// �z��N���A�^�C���F05:00	���@1500�_
	// �z���_���[�W���F10��	���@-1000�_
	// �z��A�C�e�����@�F2��	���@600�_
	// 
	// �Œፇ�v�X�R�A�F8100�_
	// ------------------------------------------------------------------------------------
	ScoreBorder.Add(10000, "S+");
	ScoreBorder.Add(8500, "S");
	ScoreBorder.Add(7500, "A");

	// ------------------------------------------------------------------------------------
	// �ȉ��A�ύX�s��
	// ------------------------------------------------------------------------------------

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//�V�[���R���|�[�l���g�̍쐬
	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	RootComponent = DefaultSceneComponent;

	//�E�B�W�F�b�g�R���|�[�l���g�̍쐬
	ResultWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ResultWidgetComponent"));
	ResultWidgetComponent->SetupAttachment(DefaultSceneComponent);
	//�E�B�W�F�b�g�̐ݒ�
	UClass* WidgetClass = LoadObject<UClass>(nullptr, TEXT("/Game/_TeamFolder/UI/UI_Result.UI_Result_C"));
	ResultWidgetComponent->SetWidgetClass(WidgetClass);
	ResultWidgetComponent->SetWidgetSpace(EWidgetSpace::World);

}

// Called when the game starts or when spawned
void AResultWidget::BeginPlay()
{
	Super::BeginPlay();

	//�X�R�A�̃C���X�^���X���擾����
	ScoreInstance = Cast<UPlayerScoreInstance>(GetGameInstance());
	//�X�R�A�v�Z��������
	ScoreInstance->ScoringFunction();

	//�E�B�W�F�b�g����e�e�L�X�g���擾
	ResultWidgetComponent->InitWidget();
	Widget = ResultWidgetComponent->GetUserWidgetObject();

	DefaultScoreText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("Score")));

	ClearTimeText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("Time")));
	ClearTimeScoreText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("TimeScore")));
	
	DamageText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("Damage")));
	DamageScoreText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("DamageScore")));
	
	ItemText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("Item")));
	ItemScoreText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("ItemScore")));
	
	Static_TotalScoreText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("Static_Total")));
	TotalScoreText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("TotalScore")));
	RankText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("Rank")));


	//�e�e�L�X�g�������Ȃ��悤�ɂ���
	DefaultScoreText->SetVisibility(ESlateVisibility::Hidden);
	
	ClearTimeText->SetVisibility(ESlateVisibility::Hidden);
	ClearTimeScoreText->SetVisibility(ESlateVisibility::Hidden);
	
	DamageText->SetVisibility(ESlateVisibility::Hidden);
	DamageScoreText->SetVisibility(ESlateVisibility::Hidden);
	
	ItemText->SetVisibility(ESlateVisibility::Hidden);
	ItemScoreText->SetVisibility(ESlateVisibility::Hidden);
	
	Static_TotalScoreText->SetVisibility(ESlateVisibility::Hidden);
	TotalScoreText->SetVisibility(ESlateVisibility::Hidden);
	RankText->SetVisibility(ESlateVisibility::Hidden);

	
	//�e�L�X�g��ύX����
	DefaultScoreText->SetText(FText::AsNumber(ScoreInstance->GetPlayerDefaultScore()));
	
	FString Time = FString::Printf(TEXT("%02d:%02d"), ScoreInstance->GetGameTime_Minute(), ScoreInstance->GetGameTime_Second());
	ClearTimeText->SetText(FText::FromString(Time));
	ClearTimeScoreText->SetText(FText::AsNumber(ScoreInstance->GetPlayerTimeScore()));

	DamageText->SetText(FText::AsNumber(ScoreInstance->GetPlayerDamageCount()));
	DamageScoreText->SetText(FText::AsNumber(ScoreInstance->GetPlayerDamageScore()));

	ItemText->SetText(FText::AsNumber(ScoreInstance->GetPlayerItemCount()));
	ItemScoreText->SetText(FText::AsNumber(ScoreInstance->GetPlayerItemScore()));

	int32 TotalScore = ScoreInstance->GetTotalScore();
	TotalScoreText->SetText(FText::AsNumber(TotalScore));
	
	FString Rank = "B";
	for (const auto& Border : ScoreBorder)
	{
		if (TotalScore >= Border.Key)
		{
			Rank = Border.Value;
			break;
		}
	}
	RankText->SetText(FText::FromString(Rank));

	//�^�C�}�[���Z�b�g����
	float DelayTime = 2.5f;

	GetWorld()->GetTimerManager().SetTimer(FirstView,  this, &AResultWidget::FirstViewFunction,  DelayTime, false);
	GetWorld()->GetTimerManager().SetTimer(SecondView, this, &AResultWidget::SecondViewFunction, DelayTime + 0.5f, false);
	GetWorld()->GetTimerManager().SetTimer(ThirdView,  this, &AResultWidget::ThirdViewFunction,  DelayTime + 1.0f, false);
	GetWorld()->GetTimerManager().SetTimer(FourthView, this, &AResultWidget::FourthViewFunction, DelayTime + 1.5f, false);
	GetWorld()->GetTimerManager().SetTimer(FifthView,  this, &AResultWidget::FifthViewFunction,  DelayTime + 2.0f, false);
	GetWorld()->GetTimerManager().SetTimer(SixthView,  this, &AResultWidget::SixthViewFunction,  DelayTime + 3.0f, false);

}
void AResultWidget::FirstViewFunction()
{
	DefaultScoreText->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().ClearTimer(FirstView);
}
void AResultWidget::SecondViewFunction()
{
	ClearTimeText->SetVisibility(ESlateVisibility::Visible);
	ClearTimeScoreText->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().ClearTimer(SecondView);
}
void AResultWidget::ThirdViewFunction()
{
	DamageText->SetVisibility(ESlateVisibility::Visible);
	DamageScoreText->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().ClearTimer(ThirdView);
}
void AResultWidget::FourthViewFunction()
{
	ItemText->SetVisibility(ESlateVisibility::Visible);
	ItemScoreText->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().ClearTimer(FourthView);
}
void AResultWidget::FifthViewFunction()
{
	Static_TotalScoreText->SetVisibility(ESlateVisibility::Visible);
	TotalScoreText->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().ClearTimer(FifthView);
}
void AResultWidget::SixthViewFunction()
{
	RankText->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().ClearTimer(SixthView);
}


// Called every frame
void AResultWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

