// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyFSM.h"
#include "EnemyState.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	enemyFSM = CreateDefaultSubobject<UEnemyFSM>(TEXT("FSM"));
	enemyState = CreateDefaultSubobject<UEnemyState>(TEXT("Enemy State Comp"));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::Throw(FVector force, int Amount)
{
	// 데미지 계산
	enemyState->OnDamageProcess(Amount);
	// 날려버리자
	enemyFSM->ai->StopMovement();
	enemyFSM->SetState(EEnemyState::INTHEAIR);
	GetCharacterMovement()->Launch((force + (FVector::UpVector * force.Length() / 6)) / enemyState->mass);
	// 랜덤하게 로테이션 변경
	int p = FMath::RandRange(0, 360);
	int y = FMath::RandRange(0, 360);
	int r = FMath::RandRange(0, 360);
	SetActorRotation(FRotator(p, y, r));
	
	// 날린후?
	FTimerHandle hd;
	GetWorldTimerManager().SetTimer(hd, FTimerDelegate::CreateLambda([&]() {
		SetActorRotation(FRotator::ZeroRotator);
		enemyFSM->SetState(EEnemyState::IDLE);
		}), 1.5f, false);
	
	// 벽 부딫힐때 데미지?

}