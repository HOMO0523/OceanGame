#include "OceanPrototype/UI/OceanBackpackPanelWidget.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Ocean.h"

TSharedRef<SWidget> UOceanBackpackPanelWidget::RebuildWidget()
{
	UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border"));
	Border->SetBrushColor(FLinearColor(0.02f, 0.06f, 0.12f, 0.85f));
	WidgetTree->RootWidget = Border;

	SlotContainer = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("SlotContainer"));
	Border->AddChild(SlotContainer);

	HeaderText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HeaderText"));
	HeaderText->SetText(FText::FromString(TEXT("背包 (Tab/I)")));
	HeaderText->SetColorAndOpacity(FSlateColor(FLinearColor(0.3f, 1.0f, 0.5f)));
	HeaderText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 20));
	SlotContainer->AddChildToVerticalBox(Cast<UWidget>(HeaderText))->SetPadding(FMargin(10, 5));

	SlotCountText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SlotCountText"));
	SlotCountText->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)));
	SlotCountText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 12));
	SlotContainer->AddChildToVerticalBox(Cast<UWidget>(SlotCountText))->SetPadding(FMargin(10, 2));

	return Super::RebuildWidget();
}

void UOceanBackpackPanelWidget::BindInventory(UOceanInventoryComponent* InInventory, UOceanSurvivalComponent* InSurvival)
{
	InventoryComponent = InInventory;
	SurvivalComponent = InSurvival;
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBackpackPanel: bound=1 slot_count=%d max_slots=%d"),
		InInventory ? InInventory->GetSlots().Num() : 0, GetMaxSlots());
	RefreshSlots();
}

void UOceanBackpackPanelWidget::RefreshSlots()
{
	if (SlotCountText && InventoryComponent)
	{
		int32 Count = InventoryComponent->GetSlots().Num();
		SlotCountText->SetText(FText::FromString(FString::Printf(TEXT("物品: %d/%d 槽"), Count, 12)));
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBackpackPanel: refresh slots=%d"), Count);
	}
}

const TArray<FOceanInventorySlot>& UOceanBackpackPanelWidget::GetSlots() const
{
	static const TArray<FOceanInventorySlot> Empty;
	return InventoryComponent ? InventoryComponent->GetSlots() : Empty;
}

int32 UOceanBackpackPanelWidget::GetMaxSlots() const { return InventoryComponent ? InventoryComponent->GetSlots().Num() : 12; }

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
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBackpackPanel: drop from=%d to=%d result=%d"), FromSlotIndex, ToSlotIndex, static_cast<int32>(R));
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
