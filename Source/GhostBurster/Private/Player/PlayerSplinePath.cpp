// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerSplinePath.h"
#include "Components/SplineComponent.h"
#include "Player/VRPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerSplinePath::APlayerSplinePath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//�X�v���C���R���|�[�l���g�̍쐬
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponent;

	//�X�v���C���o�H�̌��ݒn�̏�����
	CurrentSplineDistance = 0.0f;	//�X�v���C�����n�܂��Ă���̋����@0���n�_�ʒu�ɂȂ�
	MovementSpeed = 100.0f;	//�ړ����x�@���ƂŐ���Ȃ����x�ɕύX����
}

// Called when the game starts or when spawned
void APlayerSplinePath::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerSplinePath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveAlongSpline(DeltaTime);
}

void APlayerSplinePath::SetMovementSpeed(float Speed)
{
	MovementSpeed = Speed;
}

void APlayerSplinePath::StopMovement()
{
	MovementSpeed = 0.0f;
}

void APlayerSplinePath::ResumeMovement()
{
	MovementSpeed = 100.0f;
}

FVector APlayerSplinePath::GetLocationAtCurrentDistance() const
{
	return SplineComponent->GetLocationAtDistanceAlongSpline(CurrentSplineDistance, ESplineCoordinateSpace::World);
}

void APlayerSplinePath::MoveAlongSpline(float DeltaTime)
{
	if (MovementSpeed > 0.0f)
	{
		CurrentSplineDistance += MovementSpeed * DeltaTime;
		FVector NewLocation = SplineComponent->GetLocationAtDistanceAlongSpline(CurrentSplineDistance, ESplineCoordinateSpace::World);
	
		// VRPlayerCharacter ��V�����ʒu�Ɉړ�
		if (AVRPlayerCharacter* PlayerCharacter = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
		{
			PlayerCharacter->SetActorLocation(NewLocation);
		}
	}
}