// Fill out your copyright notice in the Description page of Project Settings.


#include "LegacyGameMode.h"

#include "Blueprint/UserWidget.h"

///update
#include "Enemy.h"
#include "EngineUtils.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "LegacyPlayer.h"
#include "MotionControllerComponent.h"
#include "NavigationSystem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

ALegacyGameMode::ALegacyGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALegacyGameMode::BeginPlay()
{
	Super::BeginPlay();

	//checks if HMD is activated
	isHMDActivated = (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled()) ? true : false;

	legacyPlayer = Cast<ALegacyPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter()); 
	if (!legacyPlayer) { UE_LOG(LogTemp, Warning, TEXT("Can't find Legacy Player")); }

	
}

void ALegacyGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


	// Wave System
	if (currentWave > 0 && !bIsInWave && currentWave < 4)
	{
		enemyKillCount = 0;
		enemyCountTotal = 0;
		WaveStageManager(currentWave);
		bIsInWave = true;
	}

	if (enemyCountTotal > 0 && enemyKillCount == enemyCountTotal)
	{
		currentWave++;
		bIsInWave = false;
	}

	if (currentWave > 3 && !bIsInWave)
	{
		/*
		FTimerHandle hd;
		GetWorldTimerManager().SetTimer(hd, FTimerDelegate::CreateLambda([&]()
			{
				UGameplayStatics::SetGamePaused(GetWorld(), true);
			}), 20.f, false);
		*/
	}

	// ������ Enemay ī��Ʈ �Լ�
	UpdateEnemyCountTotal();
}

void ALegacyGameMode::WaveStart()
{
	WorldTimer();
	currentWave++;
}

void ALegacyGameMode::WorldTimer()
{
	GetWorldTimerManager().SetTimer(worldtimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			worldTime++;
			worldTimeSec = worldTime % 60;
			worldTimeMin = worldTime / 60;
		}), 1.f, true);
}

void ALegacyGameMode::SpawnEnemyPaladin(int spawnCount)
{
	currentCountForSpawnPaladin = 0;
	tempCountForSpawnPaladin = spawnCount;

	// ��� �۾��� �ణ�� �����̸� ������ spawncount�� �ݺ��Ѵ�
	GetWorldTimerManager().SetTimer(spawnPaladinHandle, FTimerDelegate::CreateLambda([&]()
	{
			// �÷��̾� ��ó�� �׺����Ʈ ������ ���� ��ġ�� �̴´�
			UNavigationSystemV1* navSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
			FNavLocation navLoc;
			FVector randomLoc;
			bool result = navSys->GetRandomPointInNavigableRadius(legacyPlayer->GetActorLocation(), 3000, navLoc);
			if (result)
			{
				randomLoc = navLoc.Location;
			}
			randomLoc.Z = 100;
			// �ش� ���� ��ġ�� ���� �����Ѵ�
			GetWorld()->SpawnActor<AEnemy>(enemyPaladinFactory, randomLoc, FRotator::ZeroRotator);
			// ��� �ݺ��ߴ��� ������ ��ǥ�� �����ߴٸ� Ÿ�̸Ӹ� �����Ѵ�
			currentCountForSpawnPaladin++;
			if (currentCountForSpawnPaladin == tempCountForSpawnPaladin)
			{
				GetWorldTimerManager().ClearTimer(spawnPaladinHandle);
			}
	}), 0.5f, true, 1.f);
}

void ALegacyGameMode::SpawnEnemyWizard(int spawnCount)
{
	currentCountForSpawnWizard = 0;
	tempCountForSpawnWizard = spawnCount;

	// ��� �۾��� �ణ�� �����̸� ������ spawncount�� �ݺ��Ѵ�
	GetWorldTimerManager().SetTimer(spawnWizardHandle, FTimerDelegate::CreateLambda([&]()
		{
			// �÷��̾� ��ó�� �׺����Ʈ ������ ���� ��ġ�� �̴´�
			UNavigationSystemV1* navSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
			FNavLocation navLoc;
			FVector randomLoc;
			bool result = navSys->GetRandomPointInNavigableRadius(legacyPlayer->GetActorLocation(), 4000, navLoc);
			if (result)
			{
				randomLoc = navLoc.Location;
			}
			randomLoc.Z = 100;
			// �ش� ���� ��ġ�� ���� �����Ѵ�
			GetWorld()->SpawnActor<AEnemy>(enemyWizardFactory, randomLoc, FRotator::ZeroRotator);
			// ��� �ݺ��ߴ��� ������ ��ǥ�� �����ߴٸ� Ÿ�̸Ӹ� �����Ѵ�
			currentCountForSpawnWizard++;
			if (currentCountForSpawnWizard == tempCountForSpawnWizard)
			{
				GetWorldTimerManager().ClearTimer(spawnWizardHandle);
			}
		}), 0.5f, true);
}

void ALegacyGameMode::WaveStageManager(int wave)
{
	SpawnEnemyWizard(wave * 2 - 1);
	SpawnEnemyPaladin(wave);
}

void ALegacyGameMode::UpdateEnemyCountTotal()
{
	arrForCount.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), arrForCount);
	if (enemyCountTotal < arrForCount.Num())
	{
		enemyCountTotal = arrForCount.Num();
	}
}
