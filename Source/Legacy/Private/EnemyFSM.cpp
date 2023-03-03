// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "Enemy.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "EngineUtils.h"
#include "LegacyPlayer.h"

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();
	
	// ���� ���� Idle�� �ʱ�ȭ
	SetState(EEnemyState::IDLE);
	// Enemy ������ ���� �����״� ĳ��
	me = Cast<AEnemy>(GetOwner());
	// ai ��Ʈ�ѷ� ĳ����
	ai = Cast<AAIController>(me->GetController());
	// Idle�� �ʿ��� Random��ġ ����
	UpdateRandomLoc(radiusForIdleRandomLoc, idleRandomLoc);
	// ���ۿ��� ����
	originLoc = me->GetActorLocation();
	// player ĳ����
	player = Cast<ALegacyPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter());
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (state)
	{
	case EEnemyState::IDLE:
		TickIdle();
		break;
	case EEnemyState::CHASE:
		TickChase();
		break;
	case EEnemyState::ATTACK:
		TickAttack();
		break;
	case EEnemyState::INTHEAIR:
		TickInTheAir();
		break;
	case EEnemyState::DAMAGE:
		TickDamage();
		break;
	case EEnemyState::DIE:
		TickDie();
		break;
	}
}

void UEnemyFSM::TickIdle()
{
	// ���� �ð����� �۶���
	idleTimer += GetWorld()->GetDeltaSeconds();

	// ���� �ð��� �ֺ��� ���� ����Ʈ�� �̵�
	if (idleTimer >= maxIdleTime && !bIsReturning)
	{
		auto result = ai->MoveToLocation(idleRandomLoc);
		// �����ߴٸ� �ٽ� �۶���
		if (result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			UpdateRandomLoc(radiusForIdleRandomLoc, idleRandomLoc);
			idleTimer = 0;
		}
	}


	// ������ �����Ÿ� �̻� �־����� �������� ����
	if (FVector::Dist(originLoc, me->GetActorLocation()) >= distanceForReturnOrigin && !bIsReturning)
	{
		bIsReturning = true;
		ai->MoveToLocation(originLoc);

		FTimerHandle hd;
		GetWorld()->GetTimerManager().SetTimer(hd, FTimerDelegate::CreateLambda([&]()
			{
				// ó�� ���·� ����
				//UE_LOG(LogTemp, Error, TEXT("Reset"));
				UpdateRandomLoc(radiusForIdleRandomLoc, idleRandomLoc);
				idleTimer = 0;
				bIsReturning = false;
			}), 5.f, false);
	}

	// �÷��̾� �����ϸ� �߰ݻ��·� ��ȯ
	if (FVector::Dist(me->GetActorLocation(), player->GetActorLocation()) < startChasingDistance)
	{
		SetState(EEnemyState::CHASE);
	}
	// for debug
	//UE_LOG(LogTemp, Warning, TEXT("timer : %f"), idleTimer);
	//UE_LOG(LogTemp, Warning, TEXT("%f"), idleRandomLoc.X);
	//UE_LOG(LogTemp, Warning, TEXT("%f"), idleRandomLoc.Y);
	//UE_LOG(LogTemp, Warning, TEXT("%f"), idleRandomLoc.Z);
}

void UEnemyFSM::TickChase()
{
	// �÷��̾� ��ġ�� ���� �̵�

	// ���� ���� �Ÿ��� �Ǹ� �������� ��ȯ

	// �ʹ� �־����� Idle�� ��ȯ

	UE_LOG(LogTemp, Warning, TEXT("is chasing"));
}

void UEnemyFSM::TickAttack()
{
	// ����

}

void UEnemyFSM::TickInTheAir()
{
	// ���߿� ���� �̵� �Ұ��� ����

	// ����� �����

}

void UEnemyFSM::TickDamage()
{
	// �ǰݽ�

}

void UEnemyFSM::TickDie()
{
	// ���� ��

}

void UEnemyFSM::SetState(EEnemyState nextState)
{
	state = nextState;
}

void UEnemyFSM::OnDamageProcess(int Amount)
{
	// ������ �������� ������ ���
}

void UEnemyFSM::UpdateRandomLoc(float radius, FVector& randomLoc)
{
	UNavigationSystemV1* navSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FNavLocation navLoc;
	bool result = navSys->GetRandomPointInNavigableRadius(me->GetActorLocation(), radius, navLoc);
	if (result)
	{
		randomLoc = navLoc.Location;
	}
}

