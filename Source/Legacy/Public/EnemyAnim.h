// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyFSM.h"
#include "EnemyAnim.generated.h"

/**
 * 
 */
UCLASS()
class LEGACY_API UEnemyAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeBeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEnemyState animState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDoAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsInTheAir;
	UPROPERTY()
	class AEnemy* me;
	UPROPERTY(EditAnywhere)
	class USoundBase* paladinAttackSound;

	UFUNCTION()
	void AnimNotify_AttackEnd();
	UFUNCTION()
	void AnimNotify_AttackMagic();
	UFUNCTION()
	void AnimNotify_AttackSoundPlay();

	UPROPERTY(EditAnywhere)
	class UAnimMontage* montage_Paladin;

};
