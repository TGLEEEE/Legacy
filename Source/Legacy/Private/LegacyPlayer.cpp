// Fill out your copyright notice in the Description page of Project Settings.


#include "LegacyPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "LegacyPlayerMagicComponent.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "LegacyPlayerMoveComponent.h"
#include "LegacyPlayerMagicComponent.h"

//update
#include "HeadMountedDisplayFunctionLibrary.h"
#include "LegacyGameMode.h"
#include "LegacyPlayerUIComponent.h"
#include "Components/ArrowComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"
#include "Components/PointLightComponent.h"


// Sets default values
ALegacyPlayer::ALegacyPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#pragma region Key Components
	moveComponent = CreateDefaultSubobject<ULegacyPlayerMoveComponent>(TEXT("Move Component"));
	magicComponent = CreateDefaultSubobject<ULegacyPlayerMagicComponent>(TEXT("Magic Component"));
	uIComponent = CreateDefaultSubobject<ULegacyPlayerUIComponent>(TEXT("UI Component"));
	physicsHandleComp = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("Physics Handle Component"));
	leftSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Left Sphere Component"));
	rightSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Right Sphere Component"));
#pragma endregion 

#pragma region VR
	cameraComp = CreateDefaultSubobject<UCameraComponent>("Camera Component");
	cameraComp->SetupAttachment(GetRootComponent());

	//the pawn rotation will be controlled by the camera's rotation
	cameraComp->bUsePawnControlRotation = true;

	leftHand = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Left Hand"));
	rightHand = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Right Hand"));

	leftHand->SetupAttachment(GetRootComponent());
	rightHand->SetupAttachment(GetRootComponent());

	leftHand->SetTrackingMotionSource(FName("Left"));
	rightHand->SetTrackingMotionSource(FName("Right"));

	leftHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHandMesh"));
	rightHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHandMesh"));

	leftHandMesh->SetupAttachment(leftHand);
	rightHandMesh->SetupAttachment(rightHand);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/MannequinsXR/Meshes/SKM_MannyXR_left.SKM_MannyXR_left'"));
	if (tempMesh.Succeeded()) {
		leftHandMesh->SetSkeletalMesh(tempMesh.Object);
		leftHandMesh->SetRelativeLocationAndRotation(FVector(-2.981260, -3.5, 4.561753), FRotator(-25, -180, 89.999998));

	}
	
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh2(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/MannequinsXR/Meshes/SKM_MannyXR_right.SKM_MannyXR_right'"));
	if (tempMesh2.Succeeded()) {
		rightHandMesh->SetSkeletalMesh(tempMesh2.Object);
		rightHandMesh->SetRelativeLocationAndRotation(FVector((-2.981260, 3.5, 4.561753)), FRotator(25, 0.000000, 89.999998));
	}
#pragma endregion 

#pragma region Warp Teleport
	teleportCircle = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Teleport Circle"));
	teleportCircle->SetupAttachment(RootComponent);
	teleportCircle->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	teleportCurveComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Teleport Curve Component"));
#pragma endregion 

#pragma region Wand
	wandStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wand Static Mesh"));
	wandStaticMeshComponent->SetupAttachment(rightHandMesh);
	wandStaticMeshComponent->SetRelativeLocation(FVector(8.429074, 16.892678, -2.629340));
	wandStaticMeshComponent->SetRelativeRotation(FRotator(0, 56.993283, 0));
	wandStaticMeshComponent->SetRelativeScale3D(FVector(0.06f));
	//update
	wandStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	wandStaticMeshComponent->SetCollisionObjectType(ECC_GameTraceChannel5);
	wandStaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	wandStaticMeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel6, ECR_Overlap);
	wandStaticMeshComponent->SetGenerateOverlapEvents(true);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempWandMesh(TEXT("/Script/Engine.StaticMesh'/Game/Legacy/YWP/Assets/source/ElderWand.ElderWand'"));

	//if found
	if (tempWandMesh.Succeeded()) {
		wandStaticMeshComponent->SetStaticMesh(tempWandMesh.Object);
	}
#pragma endregion

#pragma region Wand Light
	wandLightArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Wand Light Arrow Component"));
	wandLightArrowComponent->SetupAttachment(wandStaticMeshComponent);
	wandLightArrowComponent->SetRelativeLocation(FVector(500,0, 0));
#pragma endregion

#pragma region Hover Regions
	accioHoverRegionArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Accio Arrow Component"));
	accioHoverRegionArrowComponent->SetupAttachment(cameraComp);
	accioHoverRegionArrowComponent->SetRelativeLocation(FVector(10, 10, 10));

	grabHoverRegionArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Grab Arrow Component"));
	grabHoverRegionArrowComponent->SetupAttachment(wandStaticMeshComponent);
	grabHoverRegionArrowComponent->SetRelativeLocation(FVector(300, 0, 0));
#pragma endregion

#pragma region Sphere Components
	leftSphereComponent->SetupAttachment(leftHand);
	leftSphereComponent->SetSimulatePhysics(false);
	rightSphereComponent->SetupAttachment(rightHand);
	rightSphereComponent->SetSimulatePhysics(false);
#pragma endregion 

#pragma region Magic Region Collider
	magicRegionColliderComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Magic Region Collider Component"));
	magicRegionColliderComponent->SetupAttachment(cameraComp);
	magicRegionColliderComponent->SetRelativeLocation(FVector(95, 0, -28));
	magicRegionColliderComponent->SetRelativeScale3D(FVector(1, 2.5	, 3));

	magicRegionColliderComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	magicRegionColliderComponent->SetCollisionProfileName(TEXT("MagicRegionColliderPreset"));
	magicRegionColliderComponent->SetCollisionObjectType(ECC_GameTraceChannel6);
	magicRegionColliderComponent->SetGenerateOverlapEvents(true);
#pragma endregion 

#pragma region HP 3D UI
	heartPendant = CreateDefaultSubobject<UStaticMeshComponent>("Heart Pendant Static Mesh");
	heartPendant->SetupAttachment(leftHandMesh);
	heartPendant->SetSimulatePhysics(false);

	heartPendantLight = CreateDefaultSubobject<UPointLightComponent>("Heart Pendant Point Light Component");
	heartPendantLight->SetupAttachment(heartPendant);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempHeartPendant(TEXT("/Script/Engine.StaticMesh'/Game/Legacy/YWP/UI/Health/Heart/source/heart_face_low.heart_face_low'"));
	//if found
	if (tempHeartPendant.Succeeded()) {
		heartPendant->SetStaticMesh(tempHeartPendant.Object);
	}

	heartPendant->SetRelativeLocation(FVector((4.6, 6, -2)));
	heartPendant->SetRelativeRotation(FRotator((-50.7, -427.2, 248.6)));
	heartPendant->SetRelativeScale3D(FVector((0.7, 0.7, 0.7)));

	heartPendantLight->SetRelativeLocation(FVector(0.686219, -7, 7.639785));
	heartPendantLight->SetAttenuationRadius(5.2);
#pragma endregion

	//
	cameraComp->bUsePawnControlRotation = false;

	//initialize health
	currentHealth = maxHealth;
}


// Called when the game starts or when spawned
void ALegacyPlayer::BeginPlay()
{
	Super::BeginPlay();

	//bind collision methods
	magicRegionColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &ALegacyPlayer::OnMagicRegionBeginOverlap);
	magicRegionColliderComponent->OnComponentEndOverlap.AddDynamic(this, &ALegacyPlayer::OnMagicRegionEndOverlap);

#pragma region Enhanced Input
	playerController = Cast<APlayerController>(GetController());
	if (playerController) {
		UEnhancedInputLocalPlayerSubsystem* inputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer());
		if (inputSubsystem) {
			inputSubsystem->AddMappingContext(iMC_VRInput, 0);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMoveComponent::SetupPlayerInput - can't find inputSubsystem"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMoveComponent::SetupPlayerInput - can't find player controller"));
	}
#pragma endregion

#pragma region Checking Platform - Set Camera Control
	legacyGameMode = Cast<ALegacyGameMode>(GetWorld()->GetAuthGameMode());
	if(legacyGameMode){
		//PC
		if (!legacyGameMode->isHMDActivated) {
			//place hand where you can see them
			rightHand->SetRelativeLocation(FVector(50, 30, 80));
			rightHand->SetRelativeRotation(FRotator(90, 50, 40));
			//turn on use pawn control rotation
			//cameraComp->bUsePawnControlRotation = true;						//this to turn on PawnControlRotation - Mouse Not Looking Up; seems like the computer doesnt VRPC as HMD Connected
			cameraComp->bUsePawnControlRotation = false;
		}
		//if connected
		else {
			//set the tracking offset ; basically setting the height 
			UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Eye);
			cameraComp->bUsePawnControlRotation = false;
		}
	} 
#pragma endregion

	//timer to get controller data; so that it doesn't run every tick
	GetWorld()->GetTimerManager().SetTimer(controllerDataTimer, this, &ALegacyPlayer::GetControllerData, controllerTickSeconds,true);

	heartLightIntensity = maxHeartLightIntensity;
	heartPendantLight->SetIntensity(heartLightIntensity);
}

#pragma region Overlap
void ALegacyPlayer::OnMagicRegionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	hitComponent = Cast<UStaticMeshComponent>(OtherComp);

	if (hitComponent == wandStaticMeshComponent) {
		isInMagicRegion = true;
	}
}

void ALegacyPlayer::OnMagicRegionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	hitComponent = Cast<UStaticMeshComponent>(OtherComp);

	if (hitComponent == wandStaticMeshComponent) {

		isInMagicRegion = false;
	}
}
#pragma endregion 



// Called to bind functionality to input
void ALegacyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	setupPlayerInputDelegate.Broadcast(PlayerInputComponent);
}


// Called every frame
void ALegacyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//update
	if (!UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled()) {
		rightHand->SetRelativeRotation(cameraComp->GetRelativeRotation());
	}

	UnPossessOnDie();
}


#pragma region Controller Data
void ALegacyPlayer::GetControllerData(){
	//FVector leftCurrentPosition = leftSphereComponent->GetComponentLocation();
	//FVector rightCurrentPosition = rightSphereComponent->GetComponentLocation();
	//FVector leftCurrentVelocity = leftSphereComponent->GetPhysicsLinearVelocity();
	//FVector leftCurrentAcceleration = CalculateControllerLinearAcceleration(leftCurrentVelocity);
	FVector rightCurrentVelocity = rightSphereComponent->GetPhysicsLinearVelocity();
	FVector rightCurrentAcceleration = CalculateControllerLinearAcceleration(rightCurrentVelocity);
	FVector rightCurrentAccelerationDifference = CalculateLinearAccelerationDifference(rightCurrentAcceleration);

	//FVector leftCurrentAngularVelocity = leftSphereComponent->GetPhysicsAngularVelocityInDegrees();
	//FVector leftCurrentAngularAcceleration = CalculateControllerAngularAcceleration(leftCurrentAngularVelocity);
	FVector rightCurrentAngularVelocity = rightSphereComponent->GetPhysicsAngularVelocityInDegrees();
	FVector rightCurrentAngularAcceleration = CalculateControllerAngularAcceleration(rightCurrentAngularVelocity);

	//float leftCurrentVelocityMagnitude = leftCurrentVelocity.Size();
	//float leftCurrentAccelerationMagnitude = leftCurrentAcceleration.Size();

	//linear velocity and acceleration in float
	rightCurrentVelocityMagnitude = rightCurrentVelocity.Size();
	rightCurrentAccelerationMagnitude = rightCurrentAcceleration.Size();
	rightCurrentAccelerationDifferenceMagnitude = rightCurrentAccelerationDifference.Size();
	//angular velocity and acceleration in float
	rightCurrentAngularVelocityMagnitude = rightCurrentAngularVelocity.Size();
	rightCurrentAngularAccelerationMagnitude = rightCurrentAngularAcceleration.Size();


#pragma region Debug
	//UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::GetControllerData - Left Current Velocity %f"), leftCurrentVelocityMagnitude);
	//UE_LOG(LogTemp, Warning, TEXT("ALegacyGameMode::GetControllerData - Right Current Velocity Magnitude %f"), rightCurrentVelocityMagnitude);
	//UE_LOG(LogTemp, Warning, TEXT("ALegacyPlayer::GetControllerData - Left Current Acceleration %f"), leftCurrentAccelerationMagnitude);
	//UE_LOG(LogTemp, Warning, TEXT("ALegacyPlayer::GetControllerData - Right Current Acceleration Magnitude %f"), rightCurrentAccelerationMagnitude);
	UE_LOG(LogTemp, Error, TEXT("ALegacyPlayer::GetControllerData - Right Current Acceleration Difference Magnitude %.0f"), rightCurrentAccelerationDifferenceMagnitude);
	//UE_LOG(LogTemp, Error, TEXT("ALegacyPlayer::GetControllerData - Right Current Angular Velocity Magnitude %f"), rightCurrentAngularVelocityMagnitude);
	//UE_LOG(LogTemp, Error, TEXT("ALegacyPlayer::GetControllerData - Right Current Angular Acceleration Magnitude %f"), rightCurrentAccelerationMagnitude);

#pragma endregion
}


//calculates linear acceleration from current and previous velocity
FVector ALegacyPlayer::CalculateControllerLinearAcceleration(FVector& currentVelocity)
{
	FVector controllerAcceleration;

	//calculate linear acceleration
	controllerAcceleration = (currentVelocity - previousVelocity) / GetWorld()->DeltaTimeSeconds;

	//update previous velocity
	previousVelocity = currentVelocity;

	return controllerAcceleration;
}

FVector ALegacyPlayer::CalculateLinearAccelerationDifference(FVector& currentAcceleration)
{
	FVector accelerationDifference;

	//calculate linear acceleration
	accelerationDifference = currentAcceleration - previousAcceleration;

	//update previous velocity
	previousAcceleration = currentAcceleration;

	return accelerationDifference;
}

//calculates angular acceleration from current and previous velocity
FVector ALegacyPlayer::CalculateControllerAngularAcceleration(FVector& currentAngularVelocity)
{
	FVector angularControllerAcceleration;

	//calculate angular acceleration
	angularControllerAcceleration = (currentAngularVelocity - previousAngularAcceleration) / GetWorld()->DeltaTimeSeconds;

	//update previous angular velocity
	previousAngularVelocity = currentAngularVelocity;

	return angularControllerAcceleration;
}
#pragma endregion

void ALegacyPlayer::TakeDamageFromEnemy(int32 damagePoints)
{
	//update current health
	currentHealth -= damagePoints;


	//update health light intensity
	heartLightIntensity /= 2;
	heartPendantLight->SetIntensity(heartLightIntensity);

	//haptic feedback
	playerController->PlayHapticEffect(hFC_TakeDamage, EControllerHand::Left);
}

void ALegacyPlayer::UnPossessOnDie()
{
	if(currentHealth <= 0){
		//Set heart light to 0
		heartPendantLight->SetIntensity(0);

		//Play haptic effect
		playerController->PlayHapticEffect(hFC_Dead, EControllerHand::Left);
		playerController->PlayHapticEffect(hFC_Dead, EControllerHand::Right);

		//unposses player controller
		playerController->UnPossess();
	}
}