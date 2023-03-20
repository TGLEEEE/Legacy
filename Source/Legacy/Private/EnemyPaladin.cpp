// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPaladin.h"

#include "EnemyAnim.h"
#include "EnemyFSM.h"
#include "LegacyPlayer.h"
#include "Components/BoxComponent.h"
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

	swordComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Sword Comp for Damage"));
	swordComp->SetupAttachment(GetMesh(), TEXT("Sword_jointSocket"));
	swordComp->SetRelativeScale3D(FVector(0.1, 0.1, 1));
	swordComp->SetRelativeLocationAndRotation(FVector(-12.39f, 2.72f, 48.67f), FRotator(15, 0, 5));
	swordComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AEnemyPaladin::BeginPlay()
{
	Super::BeginPlay();
	swordComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemyPaladin::OnOverlap);
}

void AEnemyPaladin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (enemyFSM->enemyAnim->bDoAttack && !bDoDamageOnce)
	{
		bDoDamageOnce = true;
		SwordCollisionManager();
	}
	
}

void AEnemyPaladin::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto legacyPlayer = Cast<ALegacyPlayer>(OtherActor);
	if (legacyPlayer)
	{
		//UE_LOG(LogTemp, Error, TEXT("Sword Attackkkkkk!!!!!!!!!!!!!!!!!!"));
		legacyPlayer->TakeDamageFromEnemy(1);
	}
}

void AEnemyPaladin::SwordCollisionManager()
{
	swordComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FTimerHandle swordHandle;
	GetWorldTimerManager().SetTimer(swordHandle, FTimerDelegate::CreateLambda([&]()
		{
			bDoDamageOnce = false;
			swordComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}), 1.4f, false);
}
