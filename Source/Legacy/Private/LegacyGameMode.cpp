// Fill out your copyright notice in the Description page of Project Settings.


#include "LegacyGameMode.h"

#include "Blueprint/UserWidget.h"

///update
#include "HeadMountedDisplayFunctionLibrary.h"
#include "IXRTrackingSystem.h"
#include "LegacyPlayer.h"
#include "MotionControllerComponent.h"
#include "XRTrackingSystemBase.h"

ALegacyGameMode::ALegacyGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALegacyGameMode::BeginPlay()
{
	Super::BeginPlay();
	WorldTimer();
	timerWidgetUI = CreateWidget(GetWorld(), timerWidgetFactory);
	timerWidgetUI->AddToViewport();

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



	FVector currentPosition = legacyPlayer->rightHand->GetComponentLocation();
	FVector currentVelocity = legacyPlayer->rightHand->GetComponentVelocity();

//not sure if this will work
FVector relativeCureentPosition = GetTransform().InverseTransformPosition(myComponent->GetComponentLocation());


	FTransform playerTransform = legacyPlayer->GetActorTransform();

	FTransform relativeTranform = playerTransform.Inverse();
	FVector relativePosition = relativeTranform.TransformVector(currentPosition);
	FVector relativeVelocity = relativeTranform.TransformVector(currentVelocity);

	float currentTime = GetWorld()->GetTimeSeconds();

	float deltaTime = DeltaSeconds;

	FVector currentAcceleration = (relativeVelocity - previousVelocity) / deltaTime;

	previousVelocity = relativeVelocity;
	previousTime = currentTime;



	UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Current Right Hand Position: %s"), *relativePosition.ToString());
	UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Current Right Hand Velocity: %s"), *relativeVelocity.ToString());
	UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Current Right Hand Acceleration: %s"), *currentAcceleration.ToString());



	//UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Right Current Position %s"), *rightCurrentPosition.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Right Current Position %s"), *rightCurrentPosition.ToString());

	//UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Left Current Velocity %s"), *leftCurrentVelocity.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Right Current Velocity %s"), *rightCurrentVelocity.ToString());

	//UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Left Current Acceleration %s"), *leftCurrentAcceleration.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Right Current Acceleration %s"), *rightCurrentAcceleration.ToString());

}

void ALegacyGameMode::WorldTimer()
{
	GetWorldTimerManager().SetTimer(worldtimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			worldTime++;
			worldTimeSec = worldTime % 60;
			worldTimeMin = worldTime / 60;
			UE_LOG(LogTemp, Error, TEXT("world time is : %d"), worldTime);
		}), 1.f, true);
}

#pragma region Extract Data From Controller


void ALegacyGameMode::GetControllers()
{
	UHeadMountedDisplayFunctionLibrary::GetMotionControllerData(GetWorld(), EControllerHand::Left, leftControllerData);
	UHeadMountedDisplayFunctionLibrary::GetMotionControllerData(GetWorld(), EControllerHand::Right, rightControllerData);

#pragma region Debug
	if((leftControllerData.DeviceName == FName("NAME_None")) && rightControllerData.DeviceName == FName("NAME_None")){
		UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::GetControllers - Can't find both controllers"));
	}
	else if(!(leftControllerData.DeviceName == FName("NAME_None")) != !(rightControllerData.DeviceName == FName("NAME_None"))){
		UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::GetControllers - Can't find one of the controllers"));
	}
	else if((leftControllerData.DeviceName != FName("NAME_None")) && (rightControllerData.DeviceName != FName("NAME_None"))){
		UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::GetControllers - Found both controllers"));
		UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Right Controller %s"), *rightControllerData.DeviceName.ToString());
		UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::Tick - Left Controller	 %s"), *leftControllerData.DeviceName.ToString());

	}
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
