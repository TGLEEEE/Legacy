// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyState.h"

#include "Enemy.h"
#include "LegacyPlayer.h"

// Sets default values for this component's properties
UEnemyState::UEnemyState()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyState::BeginPlay()
{
	Super::BeginPlay();

	// Enemy ������ ���� �����״� ĳ��
	me = Cast<AEnemy>(GetOwner());
	// player ĳ����
	player = Cast<ALegacyPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	
}


// Called every frame
void UEnemyState::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEnemyState::OnDamageProcess(int Amount)
{
	// ������ �Ծ����� ���

}

