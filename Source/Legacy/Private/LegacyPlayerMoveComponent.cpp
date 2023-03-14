// Fill out your copyright notice in the Description page of Project Settings.


#include "LegacyPlayerMoveComponent.h"
#include "EnhancedInputComponent.h"
#include "LegacyGameMode.h"
#include "LegacyPlayer.h"
#include "MotionControllerComponent.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Camera/CameraComponent.h"

void ULegacyPlayerMoveComponent::BeginPlay()
{
	Super::BeginPlay();

	ResetTeleport();
}

void ULegacyPlayerMoveComponent::SetupPlayerInput(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInput(PlayerInputComponent);


	auto inputSystem = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (inputSystem) {
		inputSystem->BindAction(me->iA_VRMove, ETriggerEvent::Triggered, this, &ULegacyPlayerMoveComponent::Move);
		inputSystem->BindAction(me->iA_Mouse, ETriggerEvent::Triggered, this, &ULegacyPlayerMoveComponent::Look);

		inputSystem->BindAction(me->iA_Warp, ETriggerEvent::Started, this, &ULegacyPlayerMoveComponent::StartWarp);
		inputSystem->BindAction(me->iA_Warp, ETriggerEvent::Completed, this, &ULegacyPlayerMoveComponent::EndWarp);
	}
	
}

void ULegacyPlayerMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(isWarping){

		DrawCurvedTeleport();

		//draw curves
		if (me->teleportCurveComp) {
			UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(me->teleportCurveComp, FName("User.PointArray"), curveVectorPoints);
		}
	}
}


#pragma region PC Input
void ULegacyPlayerMoveComponent::Move(const FInputActionValue& values)
{
	FVector2D axis = values.Get<FVector2D>();

	if (me->legacyGameMode->isHMDActivated) {
		me->AddMovementInput(me->cameraComp->GetRightVector(), axis.X);
		me->AddMovementInput(me->cameraComp->GetForwardVector(), axis.Y);
	}
	else{
		me->AddMovementInput(me->GetActorForwardVector(), axis.X);
		me->AddMovementInput(me->GetActorRightVector(), axis.Y);
	}
}

void ULegacyPlayerMoveComponent::Look(const FInputActionValue& values)
{
	FVector2D axis = values.Get<FVector2D>();
	me->AddControllerYawInput(axis.X);
	me->AddControllerPitchInput(axis.Y);
}
#pragma endregion

void ULegacyPlayerMoveComponent::StartWarp(const FInputActionValue& value)
{
	//flag isTeleporting when pressing the teleport button
	isWarping = true;

	//turn on teleport curve
	me->teleportCurveComp->SetVisibility(true);
}

void ULegacyPlayerMoveComponent::EndWarp(const FInputActionValue& value)
{
	//if cant teleport
	if(!ResetTeleport()){
		return;
	}

	if(isWarping){
		Warp();
		return;
	}

	me->SetActorLocation(teleportLocation + FVector::UpVector * me->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
}

bool ULegacyPlayerMoveComponent::LineTraceHit(FVector previousPosition, FVector currentPosition, FHitResult& hitResult)
{
	FCollisionQueryParams params;
	params.AddIgnoredActor(me);
	bool isHit = GetWorld()->LineTraceSingleByChannel(hitResult, previousPosition, currentPosition, ECollisionChannel::ECC_Visibility, params);

	return isHit;
}

bool ULegacyPlayerMoveComponent::CheckTeleportHit(FVector previousPosition, FVector& currentPosition)
{
	FHitResult hitResult;
	bool isHit = LineTraceHit(previousPosition, currentPosition, hitResult);

	//check if the line trace hits an object called Floor
	if (isHit && (hitResult.GetActor()->GetActorNameOrLabel() == FString("NewFloor"))) {
		currentPosition = hitResult.Location;

		me->teleportCircle->SetVisibility(true);
		me->teleportCircle->SetWorldLocation(currentPosition);

		teleportLocation = currentPosition;
	}
	else{
		me->teleportCircle->SetVisibility(false);
	}

	return isHit;
}



bool ULegacyPlayerMoveComponent::ResetTeleport()
{
	//can teleport only if the teleport circle is enabled
	bool canTeleport = me->teleportCircle->GetVisibleFlag();

	//disable teleport circle's visibility
	me->teleportCircle->SetVisibility(false);

	//unflag isTeleporting
	isWarping = false;

	//turn off teleport curve
	me->teleportCurveComp->SetVisibility(false);

	return canTeleport;
}

void ULegacyPlayerMoveComponent::DrawCurvedTeleport()
{
	//reset the curveVectorPoints
	curveVectorPoints.RemoveAt(0, curveVectorPoints.Num());

	//throw with given starting point, direction, and force
	//potential bug
	FVector position = me->leftHand->GetComponentLocation();
	FVector direction = me->leftHand->GetForwardVector() * curveTeleportForce;

	//record the starting position
	curveVectorPoints.Add(position);

	//as the projectile is moving repeatedly
	for(int i = 0; i < curveSmoothness; ++i){
		//record the last position
		FVector prevPosition = position;

		//v = v0 +at
		direction += FVector::UpVector * gravity * simulatedTime;
		//p = p0 + vt
		position += direction * simulatedTime;

		if(CheckTeleportHit(prevPosition, position)){
			//record the current position
			curveVectorPoints.Add(position);

			break;
		}
		//record the current position
		curveVectorPoints.Add(position);
	}
}


void ULegacyPlayerMoveComponent::Warp()
{
	//if isWarping is not flagged, then don't run method
	if(!isWarping){ return; }

	//reset time
	currentTime = 0;

	me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//lambda function
	GetWorld()->GetTimerManager().SetTimer(warpTimerHandle, FTimerDelegate::CreateLambda(
[this]()->void
		{
			//body; time runs, keep adding time
			currentTime += GetWorld()->DeltaTimeSeconds;
			FVector currentPosition = me->GetActorLocation();
			FVector endPosition = teleportLocation + FVector::UpVector + me->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

			//calculate position to go to
			currentPosition = FMath::Lerp<FVector>(currentPosition, endPosition, currentTime / warpTime);
			//move actor
			me->SetActorLocation(currentPosition);

			if(currentTime >=warpTime){
				//assign that position
				me->SetActorLocation(endPosition);
				//turn off timer
				GetWorld()->GetTimerManager().ClearTimer(warpTimerHandle);
				me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			}
		}
	), 0.02f, true);
	
}

