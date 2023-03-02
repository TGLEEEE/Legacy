// Fill out your copyright notice in the Description page of Project Settings.


#include "LegacyPlayerMagicComponent.h"

#include "EnhancedInputComponent.h"
#include "LegacyPlayer.h"


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
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCastSpell - isGrab"));
}

void ULegacyPlayerMagicComponent::OnActionGrabReleased()
{
	isGrab = false;
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCastSpell - !isGrab"));
}

void ULegacyPlayerMagicComponent::OnActionSpell1()
{
	isLevioso = true;
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCastSpell - isLevioso"));

}

void ULegacyPlayerMagicComponent::OnActionSpell2()
{
	isAccio = true;
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCastSpell - isAccio"));

}

void ULegacyPlayerMagicComponent::OnActionSpell3()
{
	isDepulso = true;
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCastSpell - isDepulso"));

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

	//Grab

	if(!isGrab){
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastGrab Done"));
		spellstate = SpellState::Rest;
	}
}

