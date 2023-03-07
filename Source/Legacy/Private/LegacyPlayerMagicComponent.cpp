// Fill out your copyright notice in the Description page of Project Settings.


#include "LegacyPlayerMagicComponent.h"
#include "EnhancedInputComponent.h"
#include "LegacyPlayer.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Camera/CameraComponent.h"
#include "Enemy.h"
#include "MotionControllerComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"


void ULegacyPlayerMagicComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULegacyPlayerMagicComponent::SetupPlayerInput(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInput(PlayerInputComponent);

	auto inputSystem = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (inputSystem) {
		inputSystem->BindAction(me->iA_CastSpell, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionCastSpell);

		inputSystem->BindAction(me->iA_Grab, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionGrabPressed);
		inputSystem->BindAction(me->iA_Grab, ETriggerEvent::Completed, this, &ULegacyPlayerMagicComponent::OnActionGrabReleased);

		inputSystem->BindAction(me->iA_Spell1, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpell1Pressed);
		//inputSystem->BindAction(me->iA_Spell1, ETriggerEvent::Completed, this, &ULegacyPlayerMagicComponent::OnActionSpell1Released);
		inputSystem->BindAction(me->iA_Spell2, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpell2Pressed);
		//inputSystem->BindAction(me->iA_Spell2, ETriggerEvent::Completed, this, &ULegacyPlayerMagicComponent::OnActionSpell2Released);
		inputSystem->BindAction(me->iA_Spell3, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpell3Pressed);
		//inputSystem->BindAction(me->iA_Spell3, ETriggerEvent::Completed, this, &ULegacyPlayerMagicComponent::OnActionSpell3Released);
		inputSystem->BindAction(me->iA_SpellCombo, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpellComboPressed);
		inputSystem->BindAction(me->iA_SpellCancel, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpellCancelPressed);
	}
}

void ULegacyPlayerMagicComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DetectTarget();

	UpdateSpellState();

}

#pragma region Input Action
void ULegacyPlayerMagicComponent::OnActionCastSpell()
{
	isSpellCast = true;
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCastSpell - isSpellCast"));
}

void ULegacyPlayerMagicComponent::OnActionGrabPressed()
{
	isGrab = true;
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCastSpell - isGrab"));
}

void ULegacyPlayerMagicComponent::OnActionGrabReleased()
{
	isGrab = false;
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCastSpell - !isGrab"));
}

void ULegacyPlayerMagicComponent::OnActionSpell1Pressed()
{
	isLevioso = true;
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionSpell1Pressed - isLevioso"));
}

void ULegacyPlayerMagicComponent::OnActionSpell1Released()
{
	isLevioso = false;
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCasOnActionSpell1ReleasedtSpell - !isLevioso"));

}

void ULegacyPlayerMagicComponent::OnActionSpell2Pressed()
{
	isAccio = true;
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionSpell2Pressed - isAccio"));
}

//don't need this
void ULegacyPlayerMagicComponent::OnActionSpell2Released()
{
	isAccio = false;
}


void ULegacyPlayerMagicComponent::OnActionSpell3Pressed()
{
	isDepulso = true;
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionOnActionSpell3PressedCastSpell - isDepulso"));
}

void ULegacyPlayerMagicComponent::OnActionSpell3Released()
{
	isDepulso = false;
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionSpell3Released - !isDepulso"));
}

void ULegacyPlayerMagicComponent::OnActionSpellComboPressed()
{
	isSpellCombo = true;
}

//bug: temporary; dont need this because will do timer
void ULegacyPlayerMagicComponent::OnActionSpellCancelPressed()
{
	isSpellCancel = true;
}


#pragma endregion Input Action



void ULegacyPlayerMagicComponent::UpdateSpellState()
{
	switch (spellstate) {
	case SpellState::Rest:
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::UpdateSpellState - SpellState::Rest"));
		CheckSpellState();
		break;
	case SpellState::Levioso:
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::UpdateSpellState - SpellState::Levioso"));
		CastLevioso();
		break;
	case SpellState::Accio:
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::UpdateSpellState - SpellState::Accio"));
		CastAccio();
		break;
	case SpellState::Depulso:
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::UpdateSpellState - SpellState::Depulso"));
		CastDepulso();
		break;
	case SpellState::Grab:
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::UpdateSpellState - SpellState::Grab"));
		CastGrab();
		break;
	case SpellState::Cancel:
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::UpdateSpellState - SpellState::Cancel"));
		SpellCancel();
		break;
	}
}

void ULegacyPlayerMagicComponent::CheckSpellState()
{
	if (!isSpellCast) { return; }								//have to press spell cast to activate other spells

	if (isLevioso) { spellstate = SpellState::Levioso; }
	else if (isAccio) { spellstate = SpellState::Accio; }
	else if (isDepulso) { spellstate = SpellState::Depulso; }
	else if (isGrab) { spellstate = SpellState::Grab; }
	else if (isSpellCancel) { spellstate = SpellState::Cancel; }
}


void ULegacyPlayerMagicComponent::CastLevioso()
{
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::Cast Levioso"));

	me->physicsHandleComp->SetLinearDamping(5);
	me->physicsHandleComp->SetLinearStiffness(50);
	me->physicsHandleComp->SetInterpolationSpeed(60);

	if (detectedComponent && !grabbedComponent){
		//make detected component the grabbed component
		grabbedComponent = detectedComponent;
		// since updated grabbedComponent component, dereference detectedComponent for the next detection
		//detectedComponent = nullptr;

		//cache object's initial height
		objectInitialHeight = grabbedComponent->GetComponentLocation();				//should be at wand's offset?

		//grab the component with physics handle
		me->physicsHandleComp->GrabComponentAtLocation(grabbedComponent, NAME_None, grabbedComponent->GetComponentLocation());
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::Cast Levioso - Make detected component the grabbed component"));
	}
	else if (grabbedComponent){
		me->physicsHandleComp->SetTargetLocation(objectInitialHeight + objectOffsetHeight);
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::Cast Levioso - Lifting object"));
	}

	if (isSpellCombo && grabbedComponent){
		if (!didCombo) { initialPositionBeforeCombo = me->physicsHandleComp->GetGrabbedComponent()->GetComponentLocation(); }
		SpellCombo();

		//bug fix: need to do isSpellCombo = false???
	}


	if (isGrab) { spellstate = SpellState::Grab; }
	if (isAccio) { spellstate = SpellState::Accio; }
	if (isDepulso) { spellstate = SpellState::Depulso; }

	if(isSpellCancel){ spellstate = SpellState::Cancel; }
}

void ULegacyPlayerMagicComponent::CastAccio()
{
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastAccio"));

	me->physicsHandleComp->SetLinearDamping(10);
	me->physicsHandleComp->SetLinearStiffness(50);
	me->physicsHandleComp->SetInterpolationSpeed(20);

	if (detectedComponent && !grabbedComponent) {
		//make detected component the grabbed component
		grabbedComponent = detectedComponent;

		//cache object's initial height
		objectInitialHeight = grabbedComponent->GetComponentLocation();				//should be at wand's offset?

		//grab the component with physics handle
		me->physicsHandleComp->GrabComponentAtLocation(grabbedComponent, NAME_None, grabbedComponent->GetComponentLocation());
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::Cast Levioso - Make detected component the grabbed component"));
	}
	else if (grabbedComponent) {
		me->physicsHandleComp->SetTargetLocation(me->accioHoverRegionArrowComponent->GetComponentLocation());

		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::Cast Levioso - Lifting object"));
	}

	if (isSpellCombo && grabbedComponent){
		if (!didCombo) { initialPositionBeforeCombo = me->physicsHandleComp->GetGrabbedComponent()->GetComponentLocation(); }
		SpellCombo();
		//bug fix: need to do isSpellCombo = false???
	}

	//bug temporary
	if (isDepulso) { spellstate = SpellState::Depulso; }
	if (isGrab) { spellstate = SpellState::Grab; }
	if (isSpellCancel) { spellstate = SpellState::Cancel; }

	//bug fix
	//timer => isAccio = false

}


void ULegacyPlayerMagicComponent::SpellCombo()
{
	FVector currentLocation = grabbedComponent->GetComponentLocation();
	FVector comboDirection = me->GetActorForwardVector();
	comboDirection.Normalize();				//maybe already normalized

	me->physicsHandleComp->SetTargetLocation(currentLocation + comboDirection * comboStrength);
}



void ULegacyPlayerMagicComponent::CastDepulso()
{
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastDepulso"));

	
	enemy = Cast<AEnemy>(detectedComponent->GetOwner());
	if(enemy){
		if(grabbedComponent){ me->physicsHandleComp->ReleaseComponent();  }

		enemy->GetCapsuleComponent()->SetSimulatePhysics(false);

		FVector throwDirection = me->GetActorForwardVector();// - enemy->GetActorLocation();
		throwDirection.Normalize();
		enemy->Throw(throwDirection * 300000, 1);

		isDepulso = false;
		spellstate = SpellState::Rest;
	}

		
}

void ULegacyPlayerMagicComponent::CastGrab()
{
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::Cast Grab"));

	me->physicsHandleComp->SetLinearDamping(5);
	me->physicsHandleComp->SetLinearStiffness(50);
	me->physicsHandleComp->SetInterpolationSpeed(60);

	//bug:: need to reset grabbed object in rest state?

	if(detectedComponent && !grabbedComponent){
		//make detected component the grabbed component
		grabbedComponent = detectedComponent;
		// since updated grabbedComponent component, dereference detectedComponent for the next detection
		detectedComponent = nullptr;

		//cache object's initial height
		objectInitialHeight = grabbedComponent->GetComponentLocation();				//should be at wand's offset?

		//grab the component with physics handle
		me->physicsHandleComp->GrabComponentAtLocation(grabbedComponent, NAME_None, grabbedComponent->GetComponentLocation());
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::Cast Grab - Make detected component the grabbed component"));
	}
	else if(grabbedComponent){
		//me->physicsHandleComp->SetTargetLocation(objectInitialHeight + objectOffsetHeight);
		me->physicsHandleComp->SetTargetLocation(me->grabHoverRegionArrowComponent->GetComponentLocation());
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::Cast Grab - Lifting object"));
	}

	//Grab
	if(!isGrab || isSpellCancel){
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastGrab Done"));

		spellstate = SpellState::Cancel;
	}
}



void ULegacyPlayerMagicComponent::DetectTarget()
{
	//if the player casts a spell on a detected component, return
	if (grabbedComponent) { return; }

	FVector wandHandPosition = me->rightHand->GetComponentLocation();
	FVector traceStartLocation = wandHandPosition + me->rightHand->GetForwardVector() * detectionRadius;				//add with detection radius so that the trace doesn't start from the back of the camera
	FVector traceEndLocation = traceStartLocation + me->rightHand->GetForwardVector() * 100000;

	TEnumAsByte<ECollisionChannel> traceChannel;
	traceChannel = ECollisionChannel::ECC_Visibility;
	TArray<AActor*> actorsToIgnore;

	FHitResult hitResult;

	//do a sphere trace
	bool isHit = UKismetSystemLibrary::SphereTraceSingle(this, traceStartLocation, traceEndLocation, detectionRadius, UEngineTypes::ConvertToTraceType(traceChannel), true, actorsToIgnore, EDrawDebugTrace::ForOneFrame, hitResult, true);

	//if it hits something
	if (isHit) {
		enemy = Cast<AEnemy>(hitResult.GetActor());

		//bug temporary
		detectedComponent = hitResult.GetComponent();

		if(enemy){
			#pragma region Debug
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::DetectTarget - found enemy"));
			#pragma endregion 
			detectedComponent = hitResult.GetComponent();
			#pragma region Debug
			if (detectedComponent) {
				UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::DetectTarget - grabbedComponent"));
			}
			#pragma endregion
		}
		#pragma region Debug
		else{
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::DetectTarget - can't find enemy"));
		}
		#pragma endregion
	}
}

void ULegacyPlayerMagicComponent::DereferenceVariables()
{
	//Bug: might need to take this to rest
	me->physicsHandleComp->ReleaseComponent();

	isAccio = false;
	isLevioso = false;
	isDepulso = false;
	isGrab = false;
	isSpellCombo = false;

	isSpellCancel = false;

	didCombo = false;

	//dereference grabbedComponent
	grabbedComponent = nullptr;
}




void ULegacyPlayerMagicComponent::SpellCancel()
{

	DereferenceVariables();

	spellstate = SpellState::Rest;
}

