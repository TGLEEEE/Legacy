// Fill out your copyright notice in the Description page of Project Settings.


#include "TimerWidget.h"

#include "LegacyGameMode.h"
#include "LegacyPlayer.h"
#include "Components/TextBlock.h"

void UTimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (gm->currentWave == 2 && !bDoOnce)
	{
		PlayAnimation(waveAnim);
		bDoOnce = true;
	}
	else if (gm->currentWave == 3 && bDoOnce)
	{
		PlayAnimation(waveAnim);
		bDoOnce = false;
	}

	if(gm->currentWave > 3 && !bClearOnce)
	{
		PlayAnimation(clearAnim);
		bClearOnce = true;
	}
}

void UTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// ���Ӹ�� ĳ����
	gm = Cast<ALegacyGameMode>(GetWorld()->GetAuthGameMode());
	// 0.2�ʸ��� ������Ʈ �ǰ� Ÿ�̸� ����
	GetWorld()->GetTimerManager().SetTimer(hdForTimerWidget, FTimerDelegate::CreateLambda([&]()
		{
			UpdateTimerWidget();
		}), 0.2f, true);
	// �÷��̾� ü�� �о���� ���� ĳ����
	player = Cast<ALegacyPlayer>(GetWorld()->GetFirstPlayerController());
	// �÷��̾� ������ ���ӿ��� ��µǰ� �ִ��÷���

}

void UTimerWidget::UpdateTimerWidget()
{
	WorldtimeMin->SetText(FText::AsNumber(gm->worldTimeMin));
	WorldtimeSec->SetText(FText::AsNumber(gm->worldTimeSec));
	EnemyCount->SetText(FText::AsNumber(gm->enemyKillCount));
	EnemyCountTotal->SetText(FText::AsNumber(gm->enemyCountTotal));
	WaveTextbox->SetText(FText::FromString(FString("WAVE ") + FString::FromInt(gm->currentWave)));
}
