// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	IDLE,
	CHASE,
	ATTACK,
	INTHEAIR,
	DAMAGE,
	DIE
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEGACY_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(EditAnywhere)
	EEnemyState state;
	UPROPERTY()
	class AEnemy* me;
	UPROPERTY()
	class AAIController* ai;
	UPROPERTY(EditAnywhere)
	float maxIdleTime = 3.f;
	UPROPERTY(EditAnywhere)
	float radiusForIdleRandomLoc = 500.f;
	UPROPERTY(EditAnywhere)
	float distanceForReturnOrigin = 1500.f;
	UPROPERTY(EditAnywhere)
	float startChasingDistance = 2500.f;
	UPROPERTY(EditAnywhere)
	float attackableDistance = 200.f;
	UPROPERTY(EditAnywhere)
	float stopChaseDistance = 4000.f;
	UPROPERTY(EditAnywhere)
	float attackDelay = 2.5f;
	UPROPERTY(EditAnywhere)
	bool bIsInTheAir;
	UPROPERTY()
	class UEnemyAnim* enemyAnim;

	FVector idleRandomLoc;
	FVector originLoc;
	float idleTimer;
	float attackTimer = 2.f;
	float dieTimer;
	
	bool bIsReturning;
	bool bDoOnce;
	bool bIsMoving;
	bool bDamageAnimDoOnce;
	bool bDieCollisionOnce;

	void TickIdle();
	void TickChase();
	void TickAttack();
	void TickInTheAir();
	void TickDamage();
	void TickDie();
	void SetState(EEnemyState nextState);
	void UpdateRandomLoc(float radius, FVector& randomLoc);

};
