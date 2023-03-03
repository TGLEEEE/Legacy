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


	// 원점과 일정거리 이상 멀어지면 원점으로 복귀
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

	// 플레이어 감지하면 추격상태로 전환
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
	// 플레이어 위치를 향해 이동

	// 공격 가능 거리가 되면 공격으로 전환

	// 너무 멀어지면 Idle로 전환

	UE_LOG(LogTemp, Warning, TEXT("is chasing"));
}

void UEnemyFSM::TickAttack()
{
	// 공격

}

void UEnemyFSM::TickInTheAir()
{
	// 공중에 떠서 이동 불가한 상태

	// 허우적 허우적

}

void UEnemyFSM::TickDamage()
{
	// 피격시

}

void UEnemyFSM::TickDie()
{
	// 죽을 때

}

void UEnemyFSM::SetState(EEnemyState nextState)
{
	state = nextState;
}

void UEnemyFSM::OnDamageProcess(int Amount)
{
	// 데미지 들어왔을때 죽을지 계산
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

