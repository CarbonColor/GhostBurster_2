// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TitleEnemy_White.h"
#include "TitleEnemy_Green.h"
#include "TitleEnemy_Red.h"
#include "TitleEnemy_Blue.h"
#include "TitleEventManager.generated.h"

// デリゲート用のマクロ
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFirstEventDispatcher);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSecondEventDispatcher);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FThirdEventDispatcher);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFourthEventDispatcher);

UCLASS()
class GHOSTBURSTER_API ATitleEventManager : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ATitleEventManager();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // ディスパッチャー
    UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
    FFirstEventDispatcher FirstEventDispatcher;
    UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
    FSecondEventDispatcher SecondEventDispatcher;
    UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
    FThirdEventDispatcher ThirdEventDispatcher;
    UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
    FFourthEventDispatcher FourthEventDispatcher;


    //敵が死亡した際に呼ばれるメソッド
    UFUNCTION(BlueprintCallable, Category = "Function")
    void EnemyDeadFunction();

    //レベルブループリントで呼ぶイベントメソッド
    UFUNCTION(BlueprintCallable, Category = "Event")
    void FirstEvent();  //全種類の敵を生成（全ての敵を倒す）
    UFUNCTION(BlueprintCallable, Category = "Event")
    void SecondEvent();  //たくさんの敵を生成（アイテムの使用を確認したら次）
    UFUNCTION(BlueprintCallable, Category = "Event")
    void ThirdEvent();  //少しだけ敵を生成（アイテムの使用を確認し、敵を倒したら次）
    UFUNCTION(BlueprintCallable, Category = "Event")
    void FourthEvent();  //何も出さない（アイテムの使用を確認したら次）

    UFUNCTION(BlueprintCallable, Category = "Event")
    void IsUseAttackItem();
    UFUNCTION(BlueprintCallable, Category = "Event")
    void IsUseBuffItem();
    UFUNCTION(BlueprintCallable, Category = "Event")
    void IsUseScoreItem();

    UFUNCTION(BlueprintCallable, Category = "Event")
    void NextEventNumber();

    //敵の生成位置
    UPROPERTY(EditAnywhere, Category = "FirstEvent")
    FVector First_White_Location;
    UPROPERTY(EditAnywhere, Category = "FirstEvent")
    FVector First_Green_Location;
    UPROPERTY(EditAnywhere, Category = "FirstEvent")
    FVector First_Red_Location;
    UPROPERTY(EditAnywhere, Category = "FirstEvent")
    FVector First_Blue_Location;

    UPROPERTY(EditAnywhere, Category = "SecondEvent")
    FVector Second_White_Location1;
    UPROPERTY(EditAnywhere, Category = "SecondEvent")
    FVector Second_White_Location2;
    UPROPERTY(EditAnywhere, Category = "SecondEvent")
    FVector Second_Green_Location1;
    UPROPERTY(EditAnywhere, Category = "SecondEvent")
    FVector Second_Green_Location2;
    UPROPERTY(EditAnywhere, Category = "SecondEvent")
    FVector Second_Red_Location1;
    UPROPERTY(EditAnywhere, Category = "SecondEvent")
    FVector Second_Red_Location2;
    UPROPERTY(EditAnywhere, Category = "SecondEvent")
    FVector Second_Blue_Location1;
    UPROPERTY(EditAnywhere, Category = "SecondEvent")
    FVector Second_Blue_Location2;

    UPROPERTY(EditAnywhere, Category = "ThirdEvent")
    FVector Third_White_Location;

    UPROPERTY(EditAnywhere, Category = "FourthEvent")
    FVector Fourth_Treasure_Location;

private:

    //敵の数のカウント
    int32 EnemyCount;
    //イベントの進行状況
    int32 EventNumber;

    //攻撃アイテムを使用したかどうか
    bool bIsUseAttackItem;
    //強化アイテムを使用したかどうか
    bool bIsUseBuffItem;
    //スコアアイテムを使用したかどうか
    bool bIsUseScoreItem;

    //敵のアクタークラス
    UPROPERTY(EditDefaultsOnly, Category = "Enemy")
    TSubclassOf<ATitleEnemy_White> WhiteEnemyClass;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy")
    TSubclassOf<ATitleEnemy_Green> GreenEnemyClass;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy")
    TSubclassOf<ATitleEnemy_Red> RedEnemyClass;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy")
    TSubclassOf<ATitleEnemy_Blue> BlueEnemyClass;

    UPROPERTY(EditDefaultsOnly, Category = "Treasure")
    TSubclassOf<AActor> TreasureBox;
};
