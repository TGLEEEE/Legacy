// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "Enemy.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "EnemyAnim.h"
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
	// anim instance ĳ����
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

	// ���� �ð����� �۶���
	idleTimer += GetWorld()->GetDeltaSeconds();

	// ���� �ð��� �ֺ��� ���� ����Ʈ�� �̵�
	if (idleTimer >= maxIdleTime && !bIsReturning)
	{
		auto result = ai->MoveToLocation(idleRandomLoc);
		bIsMoving = true;
		// �����ߴٸ� �ٽ� �۶���
		if (result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			UpdateRandomLoc(radiusForIdleRandomLoc, idleRandomLoc);
			idleTimer = 0;
			bIsMoving = false;
		}
	}

	// �÷��̾� �����ϸ� �߰ݻ��·� ��ȯ
	if (FVector::Dist(me->GetActorLocation(), player->GetActorLocation()) < startChasingDistance)
	{
		SetState(EEnemyState::CHASE);
	}

		// ���̵� ���¿��� ������ �����Ÿ� �̻� �־����� �������� ����
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

	// �÷��̾� ��ġ�� ���� �̵�
	ai->MoveToLocation(player->GetActorLocation());
	// ���� ���� �Ÿ��� �Ǹ� �������� ��ȯ
	if (FVector::Dist(me->GetActorLocation(), player->GetActorLocation()) < attackableDistance)
	{
		ai->StopMovement();
		SetState(EEnemyState::ATTACK);
	}
	// �ʹ� �־����� Idle�� ��ȯ
	if (FVector::Dist(me->GetActorLocation(), player->GetActorLocation()) > stopChaseDistance)
	{
		SetState(EEnemyState::IDLE);
		UE_LOG(LogTemp, Error, TEXT("go idle"));
	}
}

void UEnemyFSM::TickAttack()
{
	enemyAnim->animState = EEnemyState::ATTACK;

	attackTimer += GetWorld()->GetDeltaSeconds();
	// ����
	if (attackTimer >= attackDelay)
	{
		// ���� �ִ�
		enemyAnim->bDoAttack = true;
		attackTimer = 0;
	}

	if (FVector::Dist(me->GetActorLocation(), player->GetActorLocation()) > attackableDistance)
	{
		SetState(EEnemyState::CHASE);
	}
}

void UEnemyFSM::TickInTheAir()
{
	enemyAnim->animState = EEnemyState::INTHEAIR;
	// ���߿� ���� �̵� �Ұ��� ���� (�÷��̾�� Grab���� ���� ������x)
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

		EEnemyState temp = state;
		enemyAnim->animState = EEnemyState::DAMAGE;

		FTimerHandle hd;
		GetWorld()->GetTimerManager().SetTimer(hd, FTimerDelegate::CreateLambda([&]()
			{
				SetState(temp);
				bDamageAnimDoOnce = false;
			}), 0.2f, false);
	}
}

void UEnemyFSM::TickDie()
{
	enemyAnim->animState = EEnemyState::DIE;

	// ���� ��

	// �״� �ִ�

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
