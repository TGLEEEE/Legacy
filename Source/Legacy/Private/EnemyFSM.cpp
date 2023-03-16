// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "Enemy.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "EnemyAnim.h"
#include "EngineUtils.h"
#include "LegacyGameMode.h"
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
	// anim instance 캐스팅
	enemyAnim = Cast<UEnemyAnim>(me->GetMesh()->GetAnimInstance());
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	enemyAnim->bIsInTheAir = bIsInTheAir;

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
	if (!bIsMoving)
	{
		enemyAnim->animState = EEnemyState::IDLE;
	}
	else
	{
		enemyAnim->animState = EEnemyState::CHASE;
	}

	// 일정 시간동안 멍때림
	idleTimer += GetWorld()->GetDeltaSeconds();

	// 일정 시간후 주변의 랜덤 포인트로 이동
	if (idleTimer >= maxIdleTime && !bIsReturning)
	{
		auto result = ai->MoveToLocation(idleRandomLoc);
		bIsMoving = true;
		// 도착했다면 다시 멍때림
		if (result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			UpdateRandomLoc(radiusForIdleRandomLoc, idleRandomLoc);
			idleTimer = 0;
			bIsMoving = false;
		}
	}

	// 플레이어 감지하면 추격상태로 전환
	if (FVector::Dist(me->GetActorLocation(), me->player->GetActorLocation()) < startChasingDistance)
	{
		SetState(EEnemyState::CHASE);
	}

	// 아이들 상태에서 원점과 일정거리 이상 멀어지면 원점으로 복귀
	if (FVector::Dist(originLoc, me->GetActorLocation()) >= distanceForReturnOrigin && !bIsReturning)
	{
		bIsReturning = true;
		ai->MoveToLocation(originLoc);
		bIsMoving = true;

		FTimerHandle hd;
		GetWorld()->GetTimerManager().SetTimer(hd, FTimerDelegate::CreateLambda([&]()
			{
				UpdateRandomLoc(radiusForIdleRandomLoc, idleRandomLoc);
				idleTimer = 0;
				bIsReturning = false;
				bIsMoving = false;
			}), 5.f, false);
	}
}

void UEnemyFSM::TickChase()
{
	enemyAnim->animState = EEnemyState::CHASE;

	// 플레이어 위치를 향해 이동
	ai->MoveToLocation(me->player->GetActorLocation());
	// 공격 가능 거리가 되면 공격으로 전환
	if (FVector::Dist(me->GetActorLocation(), me->player->GetActorLocation()) < attackableDistance)
	{
		ai->StopMovement();
		SetState(EEnemyState::ATTACK);
	}
	// 너무 멀어지면 Idle로 전환
	if (FVector::Dist(me->GetActorLocation(), me->player->GetActorLocation()) > stopChaseDistance)
	{
		SetState(EEnemyState::IDLE);
		UE_LOG(LogTemp, Error, TEXT("go idle"));
	}
}

void UEnemyFSM::TickAttack()
{
	enemyAnim->animState = EEnemyState::ATTACK;
	attackTimer += GetWorld()->GetDeltaSeconds();

	// 딜레이마다 한번씩 공격
	if (attackTimer >= attackDelay)
	{
		enemyAnim->bDoAttack = true;
		attackTimer = 0;
	}

	// 거리 멀어지면 쫓아가게
	if (FVector::Dist(me->GetActorLocation(), me->player->GetActorLocation()) > attackableDistance)
	{
		SetState(EEnemyState::CHASE);
	}

	// 공격중 플레이어 방향 바라보게
	me->SetActorRotation((me->player->GetActorLocation() - me->GetActorLocation()).GetSafeNormal().Rotation());
}

void UEnemyFSM::TickInTheAir()
{
	enemyAnim->animState = EEnemyState::INTHEAIR;
	// 공중에 떠서 이동 불가한 상태 (플레이어에게 Grab당한 상태 통제권x)
	if (bIsInTheAir)
	{
		SetState(EEnemyState::IDLE);
		bDoOnce = false;
	}
}

void UEnemyFSM::TickDamage()
{
	if (!bDamageAnimDoOnce)
	{
		bDamageAnimDoOnce = true;
		enemyAnim->animState = EEnemyState::DAMAGE;
		FTimerHandle hd;
		GetWorld()->GetTimerManager().SetTimer(hd, FTimerDelegate::CreateLambda([&]()
			{
				SetState(EEnemyState::IDLE);
				bDamageAnimDoOnce = false;
			}), 0.8f, false);
	}
}

void UEnemyFSM::TickDie()
{
	enemyAnim->animState = EEnemyState::DIE;
	dieTimer += GetWorld()->DeltaTimeSeconds;
	if (!bDieCollisionOnce)
	{
		bDieCollisionOnce = true;
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (dieTimer > 3.f)
	{
		FVector P0 = me->GetActorLocation();
		FVector vt = FVector::DownVector * 100.f * GetWorld()->DeltaTimeSeconds;
		FVector P = P0 + vt;
		me->SetActorLocation(P);
		if (P.Z < -100.0f)
		{
			me->gm->enemyKillCount++;
			me->Destroy();
		}
	}
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

