// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPaladin.h"
#include "Components/CapsuleComponent.h"

AEnemyPaladin::AEnemyPaladin()
{
	/*
	ConstructorHelpers::FObjectFinder<UAnimBlueprint>tempAnimBP(TEXT("/Script/Engine.AnimBlueprint'/Game/Legacy/TGL/Blueprint/ABP_Enemy.ABP_Enemy'"));
	if (tempAnimBP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnimBP.Object->GeneratedClass);
	}
	*/
	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Legacy/TGL/Assets/Char/Paladin/PaladinWPropJNordstrom_UE.PaladinWPropJNordstrom_UE'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}

	GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
	GetMesh()->SetRelativeScale3D(FVector(1.1));

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("EnemyPreset"));
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
}
