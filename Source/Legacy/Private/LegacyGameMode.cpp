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
#include "TimerWidget.h"
#include "Components/SphereComponent.h"

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

	//find VR Controllers
	if(isHMDActivated && !isControllersFound){ GetControllers(); }

	//FVector leftCurrentPosition = CalculateControllerPosition(leftControllerData);
	//FVector rightCurrentPosition = CalculateControllerPosition(rightControllerData);

	//FVector leftCurrentVelocity = CalculateControllerVelocity(leftCurrentPosition);
	//FVector rightCurrentVelocity = CalculateControllerAcceleration(rightCurrentPosition);

	//FVector leftCurrentAcceleration = CalculateControllerVelocity(leftCurrentVelocity);
	//FVector rightCurrentAcceleration = CalculateControllerAcceleration(rightCurrentVelocity);



	FVector leftCurrentPosition = legacyPlayer->leftSphereComponent->GetComponentLocation();
	FVector rightCurrentPosition = legacyPlayer->rightSphereComponent->GetComponentLocation();
	FVector leftCurrentVelocity = legacyPlayer->leftSphereComponent->GetPhysicsLinearVelocity();
	FVector rightCurrentVelocity = legacyPlayer->rightSphereComponent->GetPhysicsLinearVelocity();

	//UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Current Right Hand Position: %s"), *relativePosition.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Current Right Hand Velocity: %s"), *relativeVelocity.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Current Right Hand Acceleration: %s"), *currentAcceleration.ToString());



	//UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Left Current Position %s"), *leftCurrentPosition.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Right Current Position %s"), *rightCurrentPosition.ToString());

	//UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Left Current Velocity %s"), *leftCurrentVelocity.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Right Current Velocity %s"), *rightCurrentVelocity.ToString());

	//UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Left Current Acceleration %s"), *leftCurrentAcceleration.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Right Current Acceleration %s"), *rightCurrentAcceleration.ToString());

	// Wave System
	if (currentWave > 0 && !bIsInWave && currentWave < 4)
	{
		enemyKillCount = 0;
		enemyCountTotal = 0;
		WaveStageManager(currentWave);
		bIsInWave = true;
	}

	if (enemyCountTotal > 0 && enemyKillCount == enemyCountTotal && bIsInWave)
	{
		currentWave++;
		bIsInWave = false;
	}

	if (currentWave > 3 && !bIsInWave)
	{
		// 엔딩처리 (멈춰!)
		UE_LOG(LogTemp, Warning, TEXT("CCLEARRRRRRRRRRRRRRRRRRRRRR"));
	}
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
			bool result = navSys->GetRandomPointInNavigableRadius(legacyPlayer->GetActorLocation(), 5000, navLoc);
			if (result)
			{
				randomLoc = navLoc.Location;
			}
			// 해당 랜덤 위치에 적을 스폰한다
			GetWorld()->SpawnActor<AEnemy>(enemyPaladinFactory, randomLoc, FRotator::ZeroRotator);
			// 해당 랜덤 위치에 FX를 스폰한다

			// 메인 위젯의 카운터값을 올린다
			enemyCountTotal++;
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
			bool result = navSys->GetRandomPointInNavigableRadius(legacyPlayer->GetActorLocation(), 5000, navLoc);
			if (result)
			{
				randomLoc = navLoc.Location;
			}
			// 해당 랜덤 위치에 적을 스폰한다
			GetWorld()->SpawnActor<AEnemy>(enemyWizardFactory, randomLoc, FRotator::ZeroRotator);
			// 해당 랜덤 위치에 FX를 스폰한다

			// 메인 위젯의 카운터값을 올린다
			enemyCountTotal++;
			// 몇번 반복했는지 세보고 목표에 도달했다면 타이머를 리셋한다
			currentCountForSpawnWizard++;
			if (currentCountForSpawnWizard == tempCountForSpawnWizard)
			{
				GetWorldTimerManager().ClearTimer(spawnWizardHandle);
			}
		}), 1.f, true, 1.f);
}

void ALegacyGameMode::WaveStageManager(int wave)
{
	SpawnEnemyPaladin(wave * 2);
	SpawnEnemyWizard(wave * wave);
}

#pragma region Extract Data From Controller


void ALegacyGameMode::GetControllers()
{
	UHeadMountedDisplayFunctionLibrary::GetMotionControllerData(GetWorld(), EControllerHand::Left, leftControllerData);
	UHeadMountedDisplayFunctionLibrary::GetMotionControllerData(GetWorld(), EControllerHand::Right, rightControllerData);

#pragma region Debug
	//if((leftControllerData.DeviceName == FName("NAME_None")) && rightControllerData.DeviceName == FName("NAME_None")){
	//	UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::GetControllers - Can't find both controllers"));
	//}
	//else if(!(leftControllerData.DeviceName == FName("NAME_None")) != !(rightControllerData.DeviceName == FName("NAME_None"))){
	//	UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::GetControllers - Can't find one of the controllers"));
	//}
	//else if((leftControllerData.DeviceName != FName("NAME_None")) && (rightControllerData.DeviceName != FName("NAME_None"))){
	//	UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::GetControllers - Found both controllers"));
	//	UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Right Controller %s"), *rightControllerData.DeviceName.ToString());
	//	UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Left Controller	 %s"), *leftControllerData.DeviceName.ToString());

	//}
#pragma endregion 
}

//takes controller data and returns the position of the controller
FVector ALegacyGameMode::CalculateControllerPosition(FXRMotionControllerData& controllerData)
{
	FVector controllerPosition = controllerData.AimPosition;

	return controllerPosition;
}

FVector ALegacyGameMode::CalculateControllerVelocity(FVector currentPosition)
{
	FVector controllerVelocity;

	controllerVelocity = (currentPosition - previousPosition) / GetWorld()->DeltaTimeSeconds;

	//update previous position
	previousPosition = currentPosition;

	return controllerVelocity;
}

FVector ALegacyGameMode::CalculateControllerAcceleration(FVector currentVelocity)
{
	FVector controllerAcceleration;

	controllerAcceleration = (currentVelocity - previousVelocity) / GetWorld()->DeltaTimeSeconds;


	//update previous velocity
	previousVelocity = currentVelocity;

	return controllerAcceleration;
}
#pragma endregion
