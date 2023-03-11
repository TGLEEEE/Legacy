// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyState.h"

#include "Enemy.h"
#include "EnemyFSM.h"
#include "LegacyPlayer.h"
#include "AIController.h"
#include "EnemyAnim.h"
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
}


// Called every frame
void UEnemyState::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bReadyToAir && bIsGrabbed)
	{
		me->enemyFSM->bIsInTheAir = true;
		bReadyToAir = false;
	}

	if (!bReadyToAir && !bIsGrabbed)
	{
		// bIsInTheAir ���°� �ٴڿ� �������� ��
		bReadyToAir = true;
	}
}

void UEnemyState::OnDamageProcess(int amount)
{
	// ������ ����
	hp = hp - amount;
	// damage state (�ִ� ��� ��)
	me->enemyFSM->SetState(EEnemyState::DAMAGE);
	// �� �� (���� ó�� �ʿ�)
	//me->PlayAnimMontage(me->enemyFSM->enemyAnim->montage_Paladin, 1, FName("Die"));
	me->PlayAnimMontage(me->enemyFSM->enemyAnim->montage_Paladin, 1, FName("Damage"));
}

void UEnemyState::Throw(FVector force, int Amount)
{
	// ������ ���
	OnDamageProcess(Amount);
	// ����������
	//me->GetCapsuleComponent()->AddForce(force * mass);

	// �����ϰ� �����̼� ����
	int p = FMath::RandRange(0, 360);
	int y = FMath::RandRange(0, 360);
	int r = FMath::RandRange(0, 360);
	//me->SetActorRotation(FRotator(p, y, r));

}

void UEnemyState::SetOriginalPhysicsState()
{
	me->enemyFSM->bIsInTheAir = false;
	me->GetCapsuleComponent()->SetSimulatePhysics(false);
	me->SetActorRotation(FRotator::ZeroRotator);
	me->enemyFSM->SetState(EEnemyState::IDLE);
}

