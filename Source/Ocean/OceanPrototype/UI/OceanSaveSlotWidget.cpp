#include "OceanPrototype/UI/OceanSaveSlotWidget.h"
#include "OceanPrototype/OceanSaveManager.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Engine/GameInstance.h"
#include "Ocean.h"

int32 UOceanSaveSlotWidget::PendingLoadSlot = 0;

TSharedRef<SWidget> UOceanSaveSlotWidget::RebuildWidget()
{
	if (bIsInitialized) return Super::RebuildWidget();
	bIsInitialized = true;

	auto* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("Row"));
	WidgetTree->RootWidget = Row;

	SlotLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SlotLabel"));
	SlotLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	SlotLabel->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14));
	Row->AddChild(SlotLabel);

	LoadButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("LoadButton"));
	auto* LText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("LText"));
	LText->SetText(FText::FromString(TEXT("Load")));
	LText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	LText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14));
	LoadButton->AddChild(LText);
	Row->AddChild(LoadButton);

	DeleteButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("DeleteButton"));
	auto* DText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DText"));
	DText->SetText(FText::FromString(TEXT("Delete")));
	DText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	DText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14));
	DeleteButton->AddChild(DText);
	Row->AddChild(DeleteButton);

	return Super::RebuildWidget();
}

void UOceanSaveSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (LoadButton)
	{
		LoadButton->OnClicked.AddUniqueDynamic(this, &UOceanSaveSlotWidget::OnLoadButtonClicked);
	}
	if (DeleteButton)
	{
		DeleteButton->OnClicked.AddUniqueDynamic(this, &UOceanSaveSlotWidget::OnDeleteButtonClicked);
	}

	RefreshInfo();

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanSaveSlotBindings: slot=%d load=%d delete=%d"),
		SlotIndex,
		(LoadButton && LoadButton->OnClicked.IsBound()) ? 1 : 0,
		(DeleteButton && DeleteButton->OnClicked.IsBound()) ? 1 : 0);
}

void UOceanSaveSlotWidget::SetSlotIndex(int32 Index)
{
	SlotIndex = FMath::Clamp(Index, 1, 3);
	RefreshInfo();
}

void UOceanSaveSlotWidget::RefreshInfo()
{
	int32 Day = 0;
	FString Timestamp;
	int32 Events = 0;

	bool bExists = false;
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UOceanSaveManager* SM = GI->GetSubsystem<UOceanSaveManager>())
		{
			bExists = SM->GetSlotInfo(SlotIndex, Day, Timestamp, Events);
		}
	}

	if (SlotLabel)
	{
		if (bExists)
		{
			SlotLabel->SetText(FText::FromString(FString::Printf(TEXT("Slot %d - Day %d - %s"), SlotIndex, Day, *Timestamp)));
		}
		else
		{
			SlotLabel->SetText(FText::FromString(FString::Printf(TEXT("Slot %d - Empty"), SlotIndex)));
		}
	}

	if (LoadButton) LoadButton->SetIsEnabled(bExists);
	if (DeleteButton) DeleteButton->SetIsEnabled(bExists);
}

void UOceanSaveSlotWidget::OnLoadButtonClicked()
{
	OnLoadClicked.Broadcast(SlotIndex);
}

void UOceanSaveSlotWidget::OnDeleteButtonClicked()
{
	OnDeleteClicked.Broadcast(SlotIndex);
}
