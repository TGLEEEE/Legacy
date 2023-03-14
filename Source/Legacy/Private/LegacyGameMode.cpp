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
#include "ClearWidget.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

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
		FTimerHandle hd;
		GetWorldTimerManager().SetTimer(hd, FTimerDelegate::CreateLambda([&]()
			{
				UGameplayStatics::SetGamePaused(GetWorld(), true);
			}), 3.f, false);
	}

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

	// 모든 작업을 약간의 딜레이를 가지고 spawncount번 반복한다
	GetWorldTimerManager().SetTimer(spawnPaladinHandle, FTimerDelegate::CreateLambda([&]()
	{
			// 플레이어 근처의 네비게이트 가능한 랜덤 위치를 뽑는다
			UNavigationSystemV1* navSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
			FNavLocation navLoc;
			FVector randomLoc;
			bool result = navSys->GetRandomPointInNavigableRadius(legacyPlayer->GetActorLocation(), 3000, navLoc);
			if (result)
			{
				randomLoc = navLoc.Location;
			}
			// 해당 랜덤 위치에 적을 스폰한다
			GetWorld()->SpawnActor<AEnemy>(enemyPaladinFactory, randomLoc, FRotator::ZeroRotator);
			// 해당 랜덤 위치에 FX를 스폰한다

			// 메인 위젯의 카운터값을 올린다
			//enemyCountTotal++;
			// 몇번 반복했는지 세보고 목표에 도달했다면 타이머를 리셋한다
			currentCountForSpawnPaladin++;
			if (currentCountForSpawnPaladin == tempCountForSpawnPaladin)
			{
				GetWorldTimerManager().ClearTimer(spawnPaladinHandle);
			}
	}), 1.f, true, 1.f);
}

void ALegacyGameMode::SpawnEnemyWizard(int spawnCount)
{
	currentCountForSpawnWizard = 0;
	tempCountForSpawnWizard = spawnCount;

	// 모든 작업을 약간의 딜레이를 가지고 spawncount번 반복한다
	GetWorldTimerManager().SetTimer(spawnWizardHandle, FTimerDelegate::CreateLambda([&]()
		{
			// 플레이어 근처의 네비게이트 가능한 랜덤 위치를 뽑는다
			UNavigationSystemV1* navSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
			FNavLocation navLoc;
			FVector randomLoc;
			bool result = navSys->GetRandomPointInNavigableRadius(legacyPlayer->GetActorLocation(), 4000, navLoc);
			if (result)
			{
				randomLoc = navLoc.Location;
			}
			// 해당 랜덤 위치에 적을 스폰한다
			GetWorld()->SpawnActor<AEnemy>(enemyWizardFactory, randomLoc, FRotator::ZeroRotator);
			// 해당 랜덤 위치에 FX를 스폰한다

			// 메인 위젯의 카운터값을 올린다
			//enemyCountTotal++;
			// 몇번 반복했는지 세보고 목표에 도달했다면 타이머를 리셋한다
			currentCountForSpawnWizard++;
			if (currentCountForSpawnWizard == tempCountForSpawnWizard)
			{
				GetWorldTimerManager().ClearTimer(spawnWizardHandle);
			}
		}), 1.f, true, 0.6f);
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
