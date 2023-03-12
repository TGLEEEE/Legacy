// Fill out your copyright notice in the Description page of Project Settings.


#include "TimerWidget.h"

#include "LegacyGameMode.h"
#include "Components/TextBlock.h"

void UTimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
}

void UTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 게임모드 캐스팅
	gm = Cast<ALegacyGameMode>(GetWorld()->GetAuthGameMode());
	// 0.2초마다 업데이트 되게 타이머 세팅
	GetWorld()->GetTimerManager().SetTimer(hdForTimerWidget, FTimerDelegate::CreateLambda([&]()
		{
			UpdateTimerWidget();
		}), 0.2f, true);
}

void UTimerWidget::UpdateTimerWidget()
{
	WorldtimeMin->SetText(FText::AsNumber(gm->worldTimeMin));
	WorldtimeSec->SetText(FText::AsNumber(gm->worldTimeSec));
	EnemyCount->SetText(FText::AsNumber(gm->enemyCount));
	EnemyCountTotal->SetText(FText::AsNumber(gm->enemyCountTotal));
}
