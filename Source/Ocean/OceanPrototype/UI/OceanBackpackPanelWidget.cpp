#include "OceanPrototype/UI/OceanBackpackPanelWidget.h"
#include "OceanPrototype/UI/OceanBackpackSlotWidget.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"
#include "Ocean.h"

TSharedRef<SWidget> UOceanBackpackPanelWidget::RebuildWidget()
{
	if (bIsInitialized) return Super::RebuildWidget();
	bIsInitialized = true;

	// Root: dark background border
	BgBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("BgBorder"));
	BgBorder->SetBrushColor(FLinearColor(0.02f, 0.06f, 0.12f, 0.92f));
	BgBorder->SetPadding(FMargin(10, 8));
	WidgetTree->RootWidget = BgBorder;

	RootVBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("RootVBox"));
	BgBorder->AddChild(RootVBox);

	// Header
	HeaderText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HeaderText"));
	HeaderText->SetText(FText::FromString(TEXT("BACKPACK [Tab/I]")));
	HeaderText->SetColorAndOpacity(FSlateColor(FLinearColor(0.3f, 1.0f, 0.5f)));
	HeaderText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 18));
	RootVBox->AddChildToVerticalBox(HeaderText)->SetPadding(FMargin(0, 0, 0, 4));

	// Slot count text
	SlotCountText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SlotCountText"));
	SlotCountText->SetColorAndOpacity(FSlateColor(FLinearColor(0.6f, 0.6f, 0.6f)));
	SlotCountText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 12));
	RootVBox->AddChildToVerticalBox(SlotCountText)->SetPadding(FMargin(0, 0, 0, 6));

	// WrapBox for 12 slot widgets (auto-wrap grid)
	SlotWrapBox = WidgetTree->ConstructWidget<UWrapBox>(UWrapBox::StaticClass(), TEXT("SlotWrapBox"));
	SlotWrapBox->SetInnerSlotPadding(FVector2D(3.0f, 3.0f));
	RootVBox->AddChildToVerticalBox(SlotWrapBox);

	// Create 12 slot widgets
	for (int32 i = 0; i < SlotGridCount; ++i)
	{
		auto* SlotWidget = WidgetTree->ConstructWidget<UOceanBackpackSlotWidget>(
			UOceanBackpackSlotWidget::StaticClass(), *FString::Printf(TEXT("Slot_%d"), i));
		SlotWidget->ClearSlot();
		SlotWrapBox->AddChildToWrapBox(SlotWidget);
		SlotWidgets.Add(SlotWidget);
	}

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBackpackPanel: created %d slot widgets"), SlotWidgets.Num());

	return Super::RebuildWidget();
}

void UOceanBackpackPanelWidget::BindInventory(UOceanInventoryComponent* InInventory, UOceanSurvivalComponent* InSurvival)
{
	InventoryComponent = InInventory;
	SurvivalComponent = InSurvival;
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBackpackPanel: bound=1 slots=%d"), InInventory ? InInventory->GetSlots().Num() : 0);
	RefreshSlots();
}

void UOceanBackpackPanelWidget::RefreshSlots()
{
	if (!SlotCountText) return;

	const auto& Slots = GetSlots();
	const int32 Occupied = Slots.Num();

	SlotCountText->SetText(FText::FromString(FString::Printf(TEXT("Items: %d/%d slots"), Occupied, SlotGridCount)));

	// Update each slot widget with data or clear it
	for (int32 i = 0; i < SlotWidgets.Num(); ++i)
	{
		if (i < Occupied)
		{
			SlotWidgets[i]->SetSlotData(i, Slots[i]);
		}
		else
		{
			SlotWidgets[i]->ClearSlot();
		}
	}

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBackpackPanel: refresh occupied=%d total=%d"), Occupied, SlotGridCount);
}

const TArray<FOceanInventorySlot>& UOceanBackpackPanelWidget::GetSlots() const
{
	static const TArray<FOceanInventorySlot> Empty;
	return InventoryComponent ? InventoryComponent->GetSlots() : Empty;
}

int32 UOceanBackpackPanelWidget::GetMaxSlots() const { return SlotGridCount; }

bool UOceanBackpackPanelWidget::TryUseItemAtSlot(int32 SlotIndex)
{
	if (!InventoryComponent || !SurvivalComponent) return false;
	bool bOk = InventoryComponent->TryUseItemAtSlot(SlotIndex, SurvivalComponent);
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBackpackPanel: use_slot=%d result=%s"), SlotIndex, bOk ? TEXT("PASS") : TEXT("FAIL"));
	if (bOk) RefreshSlots();
	return bOk;
}

EOceanInventoryDragDropResult UOceanBackpackPanelWidget::HandleSlotDrop(int32 FromSlotIndex, int32 ToSlotIndex)
{
	if (!InventoryComponent) return EOceanInventoryDragDropResult::Rejected;
	auto R = InventoryComponent->MoveOrMergeSlot(FromSlotIndex, ToSlotIndex);
	if (R != EOceanInventoryDragDropResult::Rejected) RefreshSlots();
	return R;
}

void UOceanBackpackPanelWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
	if (!InventoryComponent) return;
	int32 Count = InventoryComponent->GetSlots().Num();
	if (Count != LastCachedSlotCount) { LastCachedSlotCount = Count; RefreshSlots(); }
}
