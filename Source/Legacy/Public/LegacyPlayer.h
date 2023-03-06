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
	UPROPERTY(EditAnywhere, Category = "Player Settings | Inputs | Movement")
	class UInputMappingContext* iMC_VRInput;

	UPROPERTY(EditAnywhere, Category = "Player Settings | Inputs | Movement")
	class UInputAction* iA_VRMove;
	UPROPERTY(EditAnywhere, Category = "Player Settings | Inputs | Movement")
	class UInputAction* iA_Mouse;

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
#pragma endregion 

#pragma region Components
	UPROPERTY(EditAnywhere, Category = "Player Settings | Components")
	class UCameraComponent* cameraComp;

	UPROPERTY(EditAnywhere, Category = "Player Settings | Components")
	class ULegacyPlayerMoveComponent* moveComponent;
	UPROPERTY(EditAnywhere, Category = "Player Settings | Components")
	class ULegacyPlayerMagicComponent* magicComponent;

	UPROPERTY(EditAnywhere)
	class UPhysicsHandleComponent* physicsHandleComp;

	UPROPERTY(EditAnywhere)
	class UArrowComponent* accioHoverRegionArrowComponent;
	UPROPERTY(EditAnywhere)
	class UArrowComponent* grabHoverRegionArrowComponent;


	UPROPERTY(EditAnywhere, Category = "Player Settings | Components")
	class UMotionControllerComponent* leftHand;
	UPROPERTY(EditAnywhere, Category = "Player Settings | Components")
	class UMotionControllerComponent* rightHand;
	UPROPERTY(EditAnywhere, Category = "Player Settings | Components")
	class USkeletalMeshComponent* leftHandMesh;
	UPROPERTY(EditAnywhere, Category = "Player Settings | Components")
	class USkeletalMeshComponent* rightHandMesh;

	FSetupPlayerInputDelegate setupPlayerInputDelegate;
#pragma endregion Components


protected:

#pragma region VR

#pragma endregion 

};
