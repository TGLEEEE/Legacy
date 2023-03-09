// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyState.h"

#include "Enemy.h"
#include "EnemyFSM.h"
#include "LegacyPlayer.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UEnemyState::UEnemyState()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyState::BeginPlay()
{
	Super::BeginPlay();

	// Enemy ������ ���� �����״� ĳ��
	me = Cast<AEnemy>(GetOwner());
	// player ĳ����
	player = Cast<ALegacyPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	originZ = me->GetActorLocation().Z;
}


// Called every frame
void UEnemyState::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetOriginalPhysicsState();

	if(bIsGrabbed){
		UE_LOG(LogTemp, Error, TEXT("UEnemyState::TickComponent - isgrab"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UEnemyState::TickComponent - !isgrab"));
	}

	if(me->enemyFSM->bIsInTheAir)
	{
		UE_LOG(LogTemp, Error, TEXT("UEnemyState::TickComponent - isinair"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UEnemyState::TickComponent - !isinair"));
	}

	if(me->GetCapsuleComponent()->IsSimulatingPhysics())
	{
		UE_LOG(LogTemp, Error, TEXT("UEnemyState::TickComponent - issimulatephys"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UEnemyState::TickComponent - !issimulatephys"));
	}
}

void UEnemyState::OnDamageProcess(int amount)
{
	// ������ ����
	hp = hp - amount;
	// damage state (�ִ� ��� ��)
	me->enemyFSM->SetState(EEnemyState::DAMAGE);
}

void UEnemyState::Throw(FVector force, int Amount)
{
	// ������ ���
	OnDamageProcess(Amount);
	// ����������
	me->enemyFSM->ai->StopMovement();
	me->enemyFSM->SetState(EEnemyState::INTHEAIR);
	me->GetCharacterMovement()->Launch((force + (FVector::UpVector * force.Length() / 10)) / mass);
	// �����ϰ� �����̼� ����
	int p = FMath::RandRange(0, 360);
	int y = FMath::RandRange(0, 360);
	int r = FMath::RandRange(0, 360);
	me->SetActorRotation(FRotator(p, y, r));

	// ������?
	FTimerHandle hd;
	GetWorld()->GetTimerManager().SetTimer(hd, FTimerDelegate::CreateLambda([&]() {
		me->enemyFSM->SetState(EEnemyState::IDLE);
		}), 1.5f, false);

	// �� �΋H���� ������?
}

void UEnemyState::SetOriginalPhysicsState()
{
	if (!me->enemyFSM->bIsInTheAir && !bIsGrabbed)
	{
		me->SetActorRotation(FRotator::ZeroRotator);
	}

	if (me->GetActorLocation().Z < originZ + 100)
	{
		FTimerHandle originHD;
		GetWorld()->GetTimerManager().SetTimer(originHD, FTimerDelegate::CreateLambda([&]() {
			me->GetCapsuleComponent()->SetSimulatePhysics(false);
			}), 0.5f, false);
	}
}

