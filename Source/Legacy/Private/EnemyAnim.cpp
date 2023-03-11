// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnim.h"

#include "Enemy.h"
#include "EnemyWizard.h"

void UEnemyAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	me = Cast<AEnemy>(TryGetPawnOwner());
}

void UEnemyAnim::AnimNotify_AttackEnd()
{
	bDoAttack = false;
}

void UEnemyAnim::AnimNotify_AttackMagic()
{
	auto wizard = Cast<AEnemyWizard>(me);
	wizard->WizardAttack();
}

