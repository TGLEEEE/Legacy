// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "Enemy.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "EngineUtils.h"
#include "LegacyPlayer.h"
#include "Components/CapsuleComponent.h"

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
	
	// 시작 상태 Idle로 초기화
	SetState(EEnemyState::IDLE);
	// Enemy 접근할 일이 많을테니 캐싱
	me = Cast<AEnemy>(GetOwner());
	// ai 컨트롤러 캐스팅
	ai = Cast<AAIController>(me->GetController());
	// Idle에 필요한 Random위치 갱신
	UpdateRandomLoc(radiusForIdleRandomLoc, idleRandomLoc);
	// 시작원점 저장
	originLoc = me->GetActorLocation();
	// player 캐스팅
	player = Cast<ALegacyPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter());
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsInTheAir && !bDoOnce)
	{
		SetState(EEnemyState::INTHEAIR);
		bDoOnce = true;
	}

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
	// 일정 시간동안 멍때림
	idleTimer += GetWorld()->GetDeltaSeconds();

	// 일정 시간후 주변의 랜덤 포인트로 이동
	if (idleTimer >= maxIdleTime && !bIsReturning)
	{
		auto result = ai->MoveToLocation(idleRandomLoc);
		// 도착했다면 다시 멍때림
		if (result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			UpdateRandomLoc(radiusForIdleRandomLoc, idleRandomLoc);
			idleTimer = 0;
		}
	}

	// 플레이어 감지하면 추격상태로 전환
	if (FVector::Dist(me->GetActorLocation(), player->GetActorLocation()) < startChasingDistance)
	{
		SetState(EEnemyState::CHASE);
	}

		// 아이들 상태에서 원점과 일정거리 이상 멀어지면 원점으로 복귀
	if (FVector::Dist(originLoc, me->GetActorLocation()) >= distanceForReturnOrigin && !bIsReturning)
	{
		bIsReturning = true;
		ai->MoveToLocation(originLoc);

		FTimerHandle hd;
		GetWorld()->GetTimerManager().SetTimer(hd, FTimerDelegate::CreateLambda([&]()
			{
				// 처음 상태로 리셋
				//UE_LOG(LogTemp, Error, TEXT("Reset"));
				UpdateRandomLoc(radiusForIdleRandomLoc, idleRandomLoc);
				idleTimer = 0;
				bIsReturning = false;
			}), 5.f, false);
	}

	// for debug
	//UE_LOG(LogTemp, Warning, TEXT("timer : %f"), idleTimer);
	//UE_LOG(LogTemp, Warning, TEXT("%f"), idleRandomLoc.X);
	//UE_LOG(LogTemp, Warning, TEXT("%f"), idleRandomLoc.Y);
	//UE_LOG(LogTemp, Warning, TEXT("%f"), idleRandomLoc.Z);
}

void UEnemyFSM::TickChase()
{
	// 플레이어 위치를 향해 이동
	ai->MoveToLocation(player->GetActorLocation());
	// 공격 가능 거리가 되면 공격으로 전환
	if (FVector::Dist(me->GetActorLocation(), player->GetActorLocation()) < attackableDistance)
	{
		ai->StopMovement();
		SetState(EEnemyState::ATTACK);
	}
	// 너무 멀어지면 Idle로 전환
	if (FVector::Dist(me->GetActorLocation(), player->GetActorLocation()) > stopChaseDistance)
	{
		SetState(EEnemyState::IDLE);
		UE_LOG(LogTemp, Error, TEXT("go idle"));
	}
	//UE_LOG(LogTemp, Error, TEXT("is chasing"));
}

void UEnemyFSM::TickAttack()
{
	attackTimer += GetWorld()->GetDeltaSeconds();
	// 공격
	if (attackTimer >= attackDelay)
	{
		// 공격 애님
		UE_LOG(LogTemp, Error, TEXT("is attacking"));
		attackTimer = 0;
	}

	if (FVector::Dist(me->GetActorLocation(), player->GetActorLocation()) > attackableDistance)
	{
		SetState(EEnemyState::CHASE);
	}
}

void UEnemyFSM::TickInTheAir()
{
	// 공중에 떠서 이동 불가한 상태 (플레이어에게 Grab당한 상태 통제권x)
	UE_LOG(LogTemp, Error, TEXT("is intheair"));

	if (bIsInTheAir)
	{
		SetState(EEnemyState::IDLE);
		bDoOnce = false;
	}
	// 바둥바둥 애님

}

void UEnemyFSM::TickDamage()
{
	// 피격시

	// 애님 재생

	// 애님 끝나는 노티파이 -> die로 분기할지 결정

}

void UEnemyFSM::TickDie()
{
	// 죽을 때

	// 죽는 애님

}

void UEnemyFSM::SetState(EEnemyState nextState)
{
	state = nextState;
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
