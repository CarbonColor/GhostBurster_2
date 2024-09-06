// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "TreasureBox.generated.h"

UENUM(BlueprintType)
enum class ETreasureBoxDirection : uint8
{
	//�񋓎q���L�q����
	Y = 0	UMETA(DisplayName = "Y"),
	X_ = 1	UMETA(DisplayName = "-X"),
	Y_ = 2	UMETA(DisplayName = "-Y"),
	X = 3	UMETA(DisplayName = "X")
};


UCLASS()
class GHOSTBURSTER_API ATreasureBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATreasureBox();

	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USceneComponent> DefaultComponent;

	UPROPERTY(EditAnywhere)
		TObjectPtr<UStaticMeshComponent> TreasureBox;

	UPROPERTY(EditAnywhere)
		TObjectPtr<UStaticMeshComponent> TreasureCover;

	UPROPERTY(EditAnywhere)
		TObjectPtr<UBoxComponent> CollisionComponent;

	UPROPERTY(EditAnywhere, Category = "Angle")
		ETreasureBoxDirection TreasureAngle;

	UPROPERTY(EditAnywhere, Category = "SE")
		TObjectPtr<USoundCue> OpenBoxSound;

	//�W�̖ڕW���W
	UPROPERTY(EditAnywhere)
		FVector TargetCoverLocation;
	//�W�̖ڕW�p�x
	UPROPERTY(EditAnywhere)
		FRotator TargetCoverRotation;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//�W���J����֐�
	UFUNCTION(BlueprintCallable, Category = "Treasure")
		void OpenTreasureBox();

	UFUNCTION(BlueprintCallable, Category = "Treasure")
		bool IsOpenedTreasure();

private:

	//�A�C�e���̃u���[�v�����g������ϐ�
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> ItemActor;

	//�W�̏������W
	FVector InitialCoverLocation;
	//�W�̏����p�x
	FRotator InitialCoverRotation;


	//�A�C�e���̍��W�v���Z�b�g
	FVector ItemLocationPreset;


	//���łɊJ�������ǂ���
	bool bIsOpened;

	//�^�C�����C��
	FTimeline CoverTimeline;

	//�^�C�����C���̃J�[�u
	UPROPERTY(EditAnywhere, Category = "Timeline")
		TObjectPtr<UCurveFloat> CoverRotation;

	UPROPERTY(EditAnywhere, Category = "Timeline")
		TObjectPtr<UCurveFloat> CoverLocationY;

	UPROPERTY(EditAnywhere, Category = "Timeline")
		TObjectPtr<UCurveFloat> CoverLocationZ;

	//�^�C�����C���̍X�V
	UFUNCTION()
		void HandleCoverProgress(float Value);

	//�^�C�����C���̊�������
	UFUNCTION()
		void OnTimelineFinished();

};