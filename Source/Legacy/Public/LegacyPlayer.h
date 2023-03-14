// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "LegacyPlayer.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FSetupPlayerInputDelegate, class UInputComponent*)

UCLASS()
class LEGACY_API ALegacyPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALegacyPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region Input
	UPROPERTY(EditAnywhere, Category = "Player Settings | Inputs")
	class UInputMappingContext* iMC_VRInput;

	UPROPERTY(EditAnywhere, Category = "Player Settings | Inputs | Movement")
	class UInputAction* iA_VRMove;
	UPROPERTY(EditAnywhere, Category = "Player Settings | Inputs | Movement")
	class UInputAction* iA_Mouse;
	UPROPERTY(EditAnywhere, Category = "Player Settings | Inputs | Movement")
	class UInputAction* iA_Warp;

	UPROPERTY(EditAnywhere, Category = "Player Settings | Inputs | Magic")
	class UInputAction* iA_CastSpell;
	UPROPERTY(EditAnywhere, Category = "Player Settings | Inputs | Magic")
	class UInputAction* iA_SpellCombo;
	UPROPERTY(EditAnywhere, Category = "Player Settings | Inputs | Magic")
	class UInputAction* iA_SpellCancel;
	UPROPERTY(EditAnywhere, Category = "Player Settings | Inputs | Magic")
	class UInputAction* iA_Grab;
	UPROPERTY(EditAnywhere, Category = "Player Settings | Inputs | Magic")
	class UInputAction* iA_Spell1;
	UPROPERTY(EditAnywhere, Category = "Player Settings | Inputs | Magic")
	class UInputAction* iA_Spell2;
	UPROPERTY(EditAnywhere, Category = "Player Settings | Inputs | Magic")
	class UInputAction* iA_Spell3;

	//update
	UPROPERTY(EditAnywhere, Category = "Player Settings | Inputs | Menu")
	class UInputAction* iA_UIActivation;
	UPROPERTY(EditAnywhere, Category = "Player Settings | Inputs | Menu")
	class UInputAction* iA_UISelection;


#pragma endregion 

#pragma region Components
	UPROPERTY(EditAnywhere)
	class UCameraComponent* cameraComp;

	UPROPERTY(EditAnywhere)
	class ULegacyPlayerMoveComponent* moveComponent;
	UPROPERTY(EditAnywhere)
	class ULegacyPlayerMagicComponent* magicComponent;

	UPROPERTY(EditAnywhere)
	class UMotionControllerComponent* leftHand;
	UPROPERTY(EditAnywhere)
	class UMotionControllerComponent* rightHand;
	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* leftHandMesh;
	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* rightHandMesh;

	UPROPERTY(EditAnywhere)
	class UPhysicsHandleComponent* physicsHandleComp;

	//update
	UPROPERTY(EditAnywhere)
	class ULegacyPlayerUIComponent* uIComponent;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* wandStaticMeshComponent;

	UPROPERTY(EditAnywhere)
	class UArrowComponent* accioHoverRegionArrowComponent;
	UPROPERTY(EditAnywhere)
	class UArrowComponent* grabHoverRegionArrowComponent;

	UPROPERTY(EditAnywhere)
	float grabHoverRegionOffset = 300;

	UPROPERTY(EditAnywhere)
	class USphereComponent* leftSphereComponent;
	UPROPERTY(EditAnywhere)
	class USphereComponent* rightSphereComponent;


	FSetupPlayerInputDelegate setupPlayerInputDelegate;
#pragma endregion Components

#pragma region Warp Teleport
	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* teleportCircle;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* teleportCurveComp;
#pragma endregion 

	void TakeDamageFromEnemy(int32 damagePoints);

	int32 currentHealth;

	UPROPERTY()
	class ALegacyGameMode* legacyGameMode;

private:
	UPROPERTY(EditAnywhere, Category = "Player Settings | Health", meta = (AllowPrivateAccess = true))
	int32 maxHealth = 10;
};





