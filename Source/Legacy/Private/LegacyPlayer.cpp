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


// Sets default values
ALegacyPlayer::ALegacyPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	moveComponent = CreateDefaultSubobject<ULegacyPlayerMoveComponent>(TEXT("Move Component"));
	magicComponent = CreateDefaultSubobject<ULegacyPlayerMagicComponent>(TEXT("Magic Component"));
	uIComponent = CreateDefaultSubobject<ULegacyPlayerUIComponent>(TEXT("UI Component"));

	physicsHandleComp = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("Physics Handle Component"));



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
#pragma endregion VR

	//update

	staticMeshCompWand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Box Component Wand"));
	staticMeshCompWand->SetupAttachment(rightHandMesh);
	staticMeshCompWand->SetRelativeLocation(FVector(18.512654, 31.897307, -5.600729));
	staticMeshCompWand->SetRelativeRotation(FRotator(0.000000, 56.993283, 0.000000));
	//Temporary
	staticMeshCompWand->SetRelativeScale3D(FVector(0.611458, 0.025521, 0.040060));

	accioHoverRegionArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Accio Arrow Component"));
	accioHoverRegionArrowComponent->SetupAttachment(GetRootComponent());
	accioHoverRegionArrowComponent->SetRelativeLocation(FVector(10, 10, 10));

	grabHoverRegionArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Grab Arrow Component"));
	grabHoverRegionArrowComponent->SetupAttachment(staticMeshCompWand);
	grabHoverRegionArrowComponent->SetRelativeLocation(FVector(300, 0, 0));


	cameraComp->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void ALegacyPlayer::BeginPlay()
{
	Super::BeginPlay();

#pragma region Enhanced Input
	APlayerController* playerController = Cast<APlayerController>(GetController());
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

#pragma region Checking Platform
	legacyGameMode = Cast<ALegacyGameMode>(GetWorld()->GetAuthGameMode());
	if(legacyGameMode){
		if (!legacyGameMode->isHMDActivated) {
			//place hand where you can see them
			rightHand->SetRelativeLocation(FVector(20, 20, 82));
			//turn on use pawn control rotation
			cameraComp->bUsePawnControlRotation = true;
		}
		//if connected
		else {
			//set the tracking offset ; basically setting the height 
			UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Eye);
		}
	} 
#pragma endregion
}

// Called every frame
void ALegacyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//update
	if (!UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled()) {
		rightHand->SetRelativeRotation(cameraComp->GetRelativeRotation());
	}
}

// Called to bind functionality to input
void ALegacyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	setupPlayerInputDelegate.Broadcast(PlayerInputComponent);
}

