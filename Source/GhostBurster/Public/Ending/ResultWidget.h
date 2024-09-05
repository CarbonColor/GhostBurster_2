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

	//ウィジェットコンポーネント
	UPROPERTY(EditAnywhere, Category = "UI")
		TObjectPtr<UWidgetComponent> ResultWidgetComponent;

private:
	//シーンコンポーネント
	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USceneComponent> DefaultSceneComponent;

	//リザルトのUI
	UPROPERTY()
		TObjectPtr<UUserWidget> ResultWidget;

	//スコアのインスタンス
	UPROPERTY()
		TObjectPtr<UPlayerScoreInstance> ScoreInstance;

	//テキストUI
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

	//表示順タイマーハンドル
	FTimerHandle FirstView;		//デフォルトスコア
	FTimerHandle SecondView;	//クリアタイム
	FTimerHandle ThirdView;		//ダメージスコア
	FTimerHandle FourthView;	//アイテムスコア
	FTimerHandle FifthView;		//トータルスコア
	FTimerHandle SixthView;		//ランク

	//タイマー用メソッド
	void FirstViewFunction();
	void SecondViewFunction();
	void ThirdViewFunction();
	void FourthViewFunction();
	void FifthViewFunction();
	void SixthViewFunction();

	TMap<int32, FString> ScoreBorder;

};
