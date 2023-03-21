// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWizard.h"
#include "Components/CapsuleComponent.h"
#include "EnemyFSM.h"
#include <Components/StaticMeshComponent.h>
#include "Kismet/GameplayStatics.h"

AEnemyWizard::AEnemyWizard()
{
	/*
	ConstructorHelpers::FObjectFinder<UAnimBlueprint>tempAnimBP(TEXT("/Script/Engine.AnimBlueprint'/Game/Legacy/TGL/Blueprint/ABP_EnemyWizard.ABP_EnemyWizard'"));
	if (tempAnimBP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnimBP.Object->GeneratedClass);
	}
	*/
	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/CityofBrass_Enemies/Meshes/Enemy/Corpse/Corpse_Sword.Corpse_Sword'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}

	wandComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wand Comp"));
	wandComp->SetupAttachment(GetMesh(), TEXT("SwordSocket"));
	wandComp->SetRelativeScale3D(FVector(0.2));
	wandComp->SetRelativeLocation(FVector(30, 0, 0));
	wandComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConstructorHelpers::FObjectFinder<UStaticMesh>tempWand(TEXT("/Script/Engine.StaticMesh'/Game/Megascans/3D_Assets/Old_Tree_Branch_wjxrbic/S_Old_Tree_Branch_wjxrbic_lod3_Var1.S_Old_Tree_Branch_wjxrbic_lod3_Var1'"));
	if (tempWand.Succeeded())
	{
		wandComp->SetStaticMesh(tempWand.Object);
	}

	GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
	GetMesh()->SetRelativeScale3D(FVector(0.9));

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("EnemyPreset"));
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
}

void AEnemyWizard::BeginPlay()
{
	Super::BeginPlay();
	enemyFSM->attackableDistance = wizardAttackRange;
	enemyFSM->attackDelay = wizardAttackDelay;
}

void AEnemyWizard::Tick(float DeltaTime)
{
	if (enemyFSM->bIsDead && !bPlayDeadOnce)
	{
		bPlayDeadOnce = true;
		int rate = FMath::RandRange(1, 3);
		if (rate == 1)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeadSound1, GetActorLocation());
		}
		else if (rate == 2)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeadSound2, GetActorLocation());
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeadSound3, GetActorLocation());
		}
	}
}

void AEnemyWizard::WizardAttack()
{
	float rate = FMath::RandRange(0.f, 1.f);
	if (rate <= rateForTornado)
	{
		GetWorld()->SpawnActor<AEnemyMagicBase>(enemyMagicFactoryTornado, wandComp->GetComponentLocation(), FRotator::ZeroRotator);
	}
	else
	{
		GetWorld()->SpawnActor<AEnemyMagicBase>(enemyMagicFactorySpark, wandComp->GetComponentLocation(), FRotator::ZeroRotator);
	}
}
