// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/PlayerSplinePath.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerSplinePath::APlayerSplinePath()
{
	PrimaryActorTick.bCanEverTick = true;

	// �X�v���C���R���|�[�l���g�̍쐬
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponent;

	//��]�ϐ��̏�����
	bIsRotating = false;
	RotationDuration = 3.0f;
	CurrentRotationTime = 0.0f;
	bIsRotatePoint = false;
	bIsStairsPoint = false;

	bIsMoving = true;
}

// Called when the game starts or when spawned
void APlayerSplinePath::BeginPlay()
{
	Super::BeginPlay();

	// �X�v���C���o�H�̌��ݒn�̏�����
	CurrentSplineDistance = 0.0f; // �X�v���C�����n�܂��Ă���̋����B0���n�_�ʒu�ɂȂ�
	MovementSpeed = DefaultSpeed; // �ړ����x�B���ƂŐ���Ȃ����x�ɕύX����

	//��U�X�v���C���̃|�C���g�����ׂĂȂ���
	SplineComponent->ClearSplinePoints(true);

	// �X�v���C���̃|�C���g�̐ݒ�
	for (FVector Point : SplinePointLocation)
	{
		SplineComponent->AddSplinePoint(Point, ESplineCoordinateSpace::World);
	}

	// �X�v���C���̃^�C�v�̐ݒ�
	for (int i = 0; i < SplineComponent->GetNumberOfSplinePoints(); ++i)
	{
		SplineComponent->SetSplinePointType(i, ESplinePointType::Linear);	//�ړ��o�H�������ɂȂ�悤�ɐݒ�
	}

	//�ŏ��͓����Ȃ�
	StopMovement();

	//�v���C���[�|�[�����擾
	PlayerCharacter = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	
	//�J�������擾
	Camera = PlayerCharacter->FindComponentByClass<UCameraComponent>();
}

// Called every frame
void APlayerSplinePath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//��]����
	if (bIsRotating)
	{
		CurrentRotationTime += DeltaTime;
		float Alpha = FMath::Clamp(CurrentRotationTime / RotationDuration, 0.0f, 1.0f);

		FRotator NewRotation = FMath::Lerp(StartRotation, TargetRotation, Alpha);
		PlayerCharacter->SetActorRotation(NewRotation);
		Camera->SetWorldRotation(NewRotation);
		if (Alpha >= 1.0f)
		{
			//��]������������A�ړ����ĊJ
			bIsRotating = false;
			StartMovement();
		}
	}
	else
	{
		MoveAlongSpline(DeltaTime);
	}
}

void APlayerSplinePath::SetMovementSpeed(float Speed)
{
	MovementSpeed = Speed;
}

void APlayerSplinePath::StopMovement()
{
	MovementSpeed = 0.0f;
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Silver, FString::Printf(TEXT("Stop Movement (%f)"), MovementSpeed));
	bIsMoving = false;

	//�{�X�킾������{�X�Ƀv���C���[�̏���n��


}

void APlayerSplinePath::StartMovement()
{
	MovementSpeed = DefaultSpeed; // �����l�ɐݒ�
	//GEngine->AddOnScreenDebugMessage(-1, 03.0f, FColor::Silver, FString::Printf(TEXT("Start Movement (%f)"), MovementSpeed));
	bIsMoving = true;
}

bool APlayerSplinePath::IsMoving()
{
	return bIsMoving;
}

FVector APlayerSplinePath::GetLocationAtCurrentDistance() const
{
	return SplineComponent->GetLocationAtDistanceAlongSpline(CurrentSplineDistance, ESplineCoordinateSpace::World);
}

void APlayerSplinePath::MoveAlongSpline(float DeltaTime)
{
	if (bIsMoving)
	{
		CurrentSplineDistance += MovementSpeed * DeltaTime;
		FVector NewLocation = SplineComponent->GetLocationAtDistanceAlongSpline(CurrentSplineDistance, ESplineCoordinateSpace::World);

		// VRPlayerCharacter ��V�����ʒu�Ɉړ�
		if (PlayerCharacter)
		{
			PlayerCharacter->SetActorLocation(NewLocation);

			////���ɋȂ���n�_�ɓ��B�������`�F�b�N
			//if (bIsRotatePoint == false)
			//{
			//	if (CurrentSplineDistance >= SplineComponent->GetDistanceAlongSplineAtSplinePoint(1))
			//	{
			//		//��]�����̏���
			//		bIsRotating = true;
			//		bIsRotatePoint = true;
			//		StartRotation = PlayerCharacter->GetActorRotation();
			//		TargetRotation = StartRotation + FRotator(0.0f, -90.0f, 0.0f);
			//		CurrentRotationTime = 0.0f;
			//		//�ړ����~�߂�
			//		StopMovement();
			//	}
			//}
			////�K�i�������n�_�ɓ��B�������`�F�b�N
			//if (bIsRotatePoint == false)
			//{
			//	if (CurrentSplineDistance >= SplineComponent->GetDistanceAlongSplineAtSplinePoint(2))
			//	{
			//		//���x�𔼕��ɗ��Ƃ�
			//		bIsStairsPoint = true;
			//		SetMovementSpeed(DefaultSpeed / 2.0f);
			//	}
			//}
			////�K�i�����肫�����n�_�ɓ��B�������`�F�b�N
			//else
			//{
			//	if (CurrentSplineDistance >= SplineComponent->GetDistanceAlongSplineAtSplinePoint(3))
			//	{
			//		//���x�����Ƃɖ߂�
			//		SetMovementSpeed(DefaultSpeed);
			//	}
			//}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, TEXT("Player is none"));
		}
	}
}