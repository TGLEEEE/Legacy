// Fill out your copyright notice in the Description page of Project Settings.


#include "LegacyPlayerMagicComponent.h"
#include "EnhancedInputComponent.h"
#include "LegacyPlayer.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Camera/CameraComponent.h"
#include "Enemy.h"
#include "MotionControllerComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"


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

		inputSystem->BindAction(me->iA_Spell1, ETriggerEvent::Completed, this, &ULegacyPlayerMagicComponent::OnActionSpell1);
		inputSystem->BindAction(me->iA_Spell2, ETriggerEvent::Completed, this, &ULegacyPlayerMagicComponent::OnActionSpell2);
		inputSystem->BindAction(me->iA_Spell3, ETriggerEvent::Completed, this, &ULegacyPlayerMagicComponent::OnActionSpell3);
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

void ULegacyPlayerMagicComponent::OnActionSpell1()
{
	isLevioso = true;
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCastSpell - isLevioso"));
}

void ULegacyPlayerMagicComponent::OnActionSpell2()
{
	isAccio = true;
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCastSpell - isAccio"));
}

void ULegacyPlayerMagicComponent::OnActionSpell3()
{
	isDepulso = true;
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCastSpell - isDepulso"));
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
	if (!isSpellCast) { return; }

	if (isLevioso) { spellstate = SpellState::Levioso; }
	else if (isAccio) { spellstate = SpellState::Accio; }
	else if (isDepulso) { spellstate = SpellState::Depulso; }
	else if (isGrab) { spellstate = SpellState::Grab; }
}

void ULegacyPlayerMagicComponent::CastLevioso()
{
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastLevioso"));

	isLevioso = false;

	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastLevioso - CastLevioso Done"));
	spellstate = SpellState::Rest;
}

void ULegacyPlayerMagicComponent::CastAccio()
{
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastAccio"));

	isAccio= false;

	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastAccio Done"));
	spellstate = SpellState::Rest;
}

void ULegacyPlayerMagicComponent::CastDepulso()
{
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastDepulso"));

	isDepulso = false;

	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastDepulso Done"));
	spellstate = SpellState::Rest;
}

void ULegacyPlayerMagicComponent::CastGrab()
{
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::Cast Grab"));

	//bug:: need to reset grabbed object in rest state?

	if(detectedComponent && !grabbedComponent){
		//make detected component the grabbed component
		grabbedComponent = detectedComponent;
		// since updated grabbedComponent component, dereference detectedComponent for the next detection
		detectedComponent = nullptr;

		//cache object's initial height
		objectInitialHeight = grabbedComponent->GetComponentLocation();

		//grab the component with physics handle
		me->physicsHandleComp->GrabComponentAtLocation(grabbedComponent, NAME_None, grabbedComponent->GetComponentLocation());
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::Cast Grab - Make detected component the grabbed component"));
	}
	else if(grabbedComponent){
		me->physicsHandleComp->SetTargetLocation(objectInitialHeight + objectOffsetHeight);
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

