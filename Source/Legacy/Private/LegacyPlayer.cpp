// Fill out your copyright notice in the Description page of Project Settings.


#include "LegacyPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "LegacyPlayerMagicComponent.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "LegacyPlayerMoveComponent.h"
#include "LegacyPlayerMagicComponent.h"

// Sets default values
ALegacyPlayer::ALegacyPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	moveComponent = CreateDefaultSubobject<ULegacyPlayerMoveComponent>(TEXT("Move Component"));
	magicComponent = CreateDefaultSubobject<ULegacyPlayerMagicComponent>(TEXT("Magic Component"));

#pragma region VR
	vRCamera = CreateDefaultSubobject<UCameraComponent>("Camera Component");
	vRCamera->SetupAttachment(GetRootComponent());

	//the pawn rotation will be controlled by the camera's rotation
	vRCamera->bUsePawnControlRotation = true;

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
		leftHandMesh->SetRelativeLocationAndRotation(FVector(-2.981260, -3.500000, 4.561753), FRotator(-25.000000, -179.999999, 89.999998));
	}

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh2(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/MannequinsXR/Meshes/SKM_MannyXR_right.SKM_MannyXR_right'"));
	if (tempMesh2.Succeeded()) {
		rightHandMesh->SetSkeletalMesh(tempMesh2.Object);
		rightHandMesh->SetRelativeLocationAndRotation(FVector(-2.981260, 3.500000, 4.561753), FRotator(25.000000, 0.000000, 89.999999));
	}
#pragma endregion VR

}

// Called when the game starts or when spawned
void ALegacyPlayer::BeginPlay()
{
	Super::BeginPlay();

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

}

// Called every frame
void ALegacyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALegacyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	setupPlayerInputDelegate.Broadcast(PlayerInputComponent);
}

