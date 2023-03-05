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
		inputSystem->BindAction(me->iA_Spell1, ETriggerEvent::Completed, this, &ULegacyPlayerMagicComponent::OnActionSpell1Released);
		inputSystem->BindAction(me->iA_Spell2, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpell2Pressed);
		inputSystem->BindAction(me->iA_Spell2, ETriggerEvent::Completed, this, &ULegacyPlayerMagicComponent::OnActionSpell2Released);
		inputSystem->BindAction(me->iA_Spell3, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpell3Pressed);
		inputSystem->BindAction(me->iA_Spell3, ETriggerEvent::Completed, this, &ULegacyPlayerMagicComponent::OnActionSpell3Released);
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
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCastSpell - isSpellCast"));
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

void ULegacyPlayerMagicComponent::OnActionSpell2Released()
{
	isAccio = false;
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionSpell2Released - !isAccio"));
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
		CastGrab();
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
}

void ULegacyPlayerMagicComponent::CastLevioso()
{
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::Cast Levioso"));

	me->physicsHandleComp->SetLinearDamping(5);
	me->physicsHandleComp->SetLinearStiffness(50);
	me->physicsHandleComp->SetInterpolationSpeed(60);

	if (detectedComponent && !grabbedComponent)
	{
		//make detected component the grabbed component
		grabbedComponent = detectedComponent;
		// since updated grabbedComponent component, dereference detectedComponent for the next detection
		detectedComponent = nullptr;

		//cache object's initial height
		objectInitialHeight = grabbedComponent->GetComponentLocation();				//should be at wand's offset?

		//grab the component with physics handle
		me->physicsHandleComp->GrabComponentAtLocation(grabbedComponent, NAME_None, grabbedComponent->GetComponentLocation());
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::Cast Levioso - Make detected component the grabbed component"));
	}
	else if (grabbedComponent)
	{
		me->physicsHandleComp->SetTargetLocation(objectInitialHeight + objectOffsetHeight);
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::Cast Levioso - Lifting object"));
	}


	if(!isLevioso){ spellstate = SpellState::Rest; }
}

void ULegacyPlayerMagicComponent::CastAccio()
{
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastAccio"));

	me->physicsHandleComp->SetLinearDamping(10);
	me->physicsHandleComp->SetLinearStiffness(50);
	me->physicsHandleComp->SetInterpolationSpeed(20);

	me->physicsHandleComp->SetTargetLocation(me->accioHoverRegionArrowComponent->GetComponentLocation());


	if(!isAccio){ spellstate = SpellState::Rest; }
}

void ULegacyPlayerMagicComponent::CastDepulso()
{
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastDepulso"));

	if(!isDepulso){ spellstate = SpellState::Rest; }
		
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
	if(!isGrab){
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastGrab Done"));
		//Bug: might need to take this to rest
		me->physicsHandleComp->ReleaseComponent();

		spellstate = SpellState::Rest;
		//dereference grabbedComponent
		grabbedComponent = nullptr;
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


