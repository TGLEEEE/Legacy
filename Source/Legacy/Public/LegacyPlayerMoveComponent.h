// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LegacyPlayerBaseComponent.h"
#include "LegacyPlayerMoveComponent.generated.h"

/**
 * 
 */
UCLASS()
class LEGACY_API ULegacyPlayerMoveComponent : public ULegacyPlayerBaseComponent
{
	GENERATED_BODY()
	
	
public:
	virtual void SetupPlayerInput(class UInputComponent* PlayerInputComponent) override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BeginPlay() override;

#pragma region Movement
	UPROPERTY(EditAnywhere, Category = "Player Settings | Inputs | Movement")
	float moveSpeed = 500;

	UFUNCTION()
	void Move(const FInputActionValue& values);
	UFUNCTION()
	void Look(const FInputActionValue& values);
#pragma endregion Movement

private:

#pragma region Warp Teleport
	void StartTeleport(const FInputActionValue& value);
	void EndTeleport(const FInputActionValue& value);

	bool ResetTeleport();

	bool CheckTeleportHit(FVector PreviousPosition, FVector& currentPosition);
	bool LineTraceHit(FVector previousPosition, FVector currentPosition, FHitResult& hitInfo);

	UPROPERTY(VisibleAnywhere, Category = "Player Settings | Inputs | Warp Teleport", meta = (AllowPrivateAccess = true))
	bool isWarping = true;
	
	bool isTeleporting = false;

	FTimerHandle warpTimerHandle;

	FVector teleportLocation;
	
	UPROPERTY(EditAnywhere, Category = "Movement", meta = (AllowPrivateAccess = true))
	float warpTime = 0.2f;
	float currentTime;
	
	void Warp();
	void DrawCurvedTeleport();

	UPROPERTY(EditAnywhere, Category = "Player Settings | VR Settings", meta = (AllowPrivateAccess = true))
	float curveTeleportForce = 1500;
	UPROPERTY(EditAnywhere, Category = "Player Settings | VR Settings", meta = (AllowPrivateAccess = true))
	float gravity = -5000;
	UPROPERTY(EditAnywhere, Category = "Player Settings | VR Settings", meta = (AllowPrivateAccess = true))
	float simulatedTime = 0.02f;
	UPROPERTY(EditAnywhere, Category = "Player Settings | VR Settings", meta = (AllowPrivateAccess = true))
	int32 curveSmoothness = 40;
	UPROPERTY(EditAnywhere, Category = "Player Settings | VR Settings", meta = (AllowPrivateAccess = true))
	bool isTeleportCurve = true;

	UPROPERTY()
	TArray<FVector> curveVectorPoints;
#pragma endregion 
};
