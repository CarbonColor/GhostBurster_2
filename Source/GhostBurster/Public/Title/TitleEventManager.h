// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TitleEnemy_White.h"
#include "TitleEnemy_Green.h"
#include "TitleEnemy_Red.h"
#include "TitleEnemy_Blue.h"
#include "TitleEventManager.generated.h"

// �f���Q�[�g�p�̃}�N��
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

    // �f�B�X�p�b�`���[
    UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
    FFirstEventDispatcher FirstEventDispatcher;
    UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
    FSecondEventDispatcher SecondEventDispatcher;
    UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
    FThirdEventDispatcher ThirdEventDispatcher;
    UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
    FFourthEventDispatcher FourthEventDispatcher;


    //�G�����S�����ۂɌĂ΂�郁�\�b�h
    UFUNCTION(BlueprintCallable, Category = "Function")
    void EnemyDeadFunction();

    //���x���u���[�v�����g�ŌĂԃC�x���g���\�b�h
    UFUNCTION(BlueprintCallable, Category = "Event")
    void FirstEvent();  //�S��ނ̓G�𐶐��i�S�Ă̓G��|���j
    UFUNCTION(BlueprintCallable, Category = "Event")
    void SecondEvent();  //��������̓G�𐶐��i�A�C�e���̎g�p���m�F�����玟�j
    UFUNCTION(BlueprintCallable, Category = "Event")
    void ThirdEvent();  //���������G�𐶐��i�A�C�e���̎g�p���m�F���A�G��|�����玟�j
    UFUNCTION(BlueprintCallable, Category = "Event")
    void FourthEvent();  //�����o���Ȃ��i�A�C�e���̎g�p���m�F�����玟�j

    UFUNCTION(BlueprintCallable, Category = "Event")
    void IsUseAttackItem();
    UFUNCTION(BlueprintCallable, Category = "Event")
    void IsUseBuffItem();
    UFUNCTION(BlueprintCallable, Category = "Event")
    void IsUseScoreItem();

    UFUNCTION(BlueprintCallable, Category = "Event")
    void NextEventNumber();

    //�G�̐����ʒu
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

    //�G�̐��̃J�E���g
    int32 EnemyCount;
    //�C�x���g�̐i�s��
    int32 EventNumber;

    //�U���A�C�e�����g�p�������ǂ���
    bool bIsUseAttackItem;
    //�����A�C�e�����g�p�������ǂ���
    bool bIsUseBuffItem;
    //�X�R�A�A�C�e�����g�p�������ǂ���
    bool bIsUseScoreItem;

    //�G�̃A�N�^�[�N���X
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
