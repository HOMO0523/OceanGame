#include "OceanPrototype/UI/OceanHUDRootWidget.h"
#include "OceanPrototype/UI/OceanStatusPanelWidget.h"
#include "OceanPrototype/UI/OceanBackpackPanelWidget.h"
#include "OceanPrototype/UI/OceanBuildPanelWidget.h"
#include "OceanPrototype/UI/OceanTimePanelWidget.h"
#include "OceanPrototype/UI/OceanItemUseModalWidget.h"
#include "OceanPrototype/UI/OceanToastWidget.h"
#include "OceanPrototype/OceanSurvivalComponent.h"
#include "OceanPrototype/OceanInventoryComponent.h"
#include "OceanPrototype/OceanBuildComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Ocean.h"

void UOceanHUDRootWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 动态创建 WBP 中缺失的子面板
	EnsureAllPanels();
}

void UOceanHUDRootWidget::EnsureAllPanels()
{
	UCanvasPanel* TargetCanvas = CanvasRoot ? CanvasRoot.Get() : Cast<UCanvasPanel>(GetRootWidget());

	// 没有 CanvasPanel → C++ 创建一个并设为 WidgetTree 根
	if (!TargetCanvas && WidgetTree)
	{
		TargetCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("CanvasRoot"));
		if (TargetCanvas)
		{
			WidgetTree->RootWidget = TargetCanvas;
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanHUDRoot: auto-created CanvasRoot via WidgetTree"));
		}
	}

	if (!TargetCanvas)
	{
		UE_LOG(LogOcean, Warning, TEXT("[TDD] OceanHUDRoot: cannot create CanvasRoot, skip"));
		return;
	}

	// --- 左上：StatusPanel ---
	if (!StatusPanel)
	{
		StatusPanel = CreateWidget<UOceanStatusPanelWidget>(this, UOceanStatusPanelWidget::StaticClass(), TEXT("StatusPanel"));
		if (StatusPanel)
		{
			TargetCanvas->AddChildToCanvas(StatusPanel);
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(StatusPanel->Slot))
			{
				CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f));
				CanvasSlot->SetPosition(FVector2D(20.0f, 20.0f));
				CanvasSlot->SetSize(FVector2D(280.0f, 120.0f));
				CanvasSlot->SetZOrder(1);
			}
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanHUDRoot: auto-created StatusPanel"));
		}
	}

	// --- 上中：TimePanel ---
	if (!TimePanel)
	{
		TimePanel = CreateWidget<UOceanTimePanelWidget>(this, UOceanTimePanelWidget::StaticClass(), TEXT("TimePanel"));
		if (TimePanel)
		{
			TargetCanvas->AddChildToCanvas(TimePanel);
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(TimePanel->Slot))
			{
				CanvasSlot->SetAnchors(FAnchors(0.5f, 0.0f));
				CanvasSlot->SetAlignment(FVector2D(0.5f, 0.0f));
				CanvasSlot->SetPosition(FVector2D(0.0f, 10.0f));
				CanvasSlot->SetSize(FVector2D(200.0f, 50.0f));
				CanvasSlot->SetZOrder(1);
			}
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanHUDRoot: auto-created TimePanel"));
		}
	}

	// --- 右侧：BackpackPanel (初始隐藏，Tab打开) ---
	if (!BackpackPanel)
	{
		BackpackPanel = CreateWidget<UOceanBackpackPanelWidget>(this, UOceanBackpackPanelWidget::StaticClass(), TEXT("BackpackPanel"));
		if (BackpackPanel)
		{
			TargetCanvas->AddChildToCanvas(BackpackPanel);
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(BackpackPanel->Slot))
			{
				CanvasSlot->SetAnchors(FAnchors(1.0f, 0.0f, 1.0f, 1.0f));
				CanvasSlot->SetAlignment(FVector2D(1.0f, 0.0f));
				CanvasSlot->SetPosition(FVector2D(0.0f, 0.0f));
				CanvasSlot->SetSize(FVector2D(320.0f, 0.0f));
				CanvasSlot->SetZOrder(5);
			}
			BackpackPanel->SetVisibility(ESlateVisibility::Collapsed);
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanHUDRoot: auto-created BackpackPanel"));
		}
	}

	// --- 左下：BuildPanel (建造时可见) ---
	if (!BuildPanel)
	{
		BuildPanel = CreateWidget<UOceanBuildPanelWidget>(this, UOceanBuildPanelWidget::StaticClass(), TEXT("BuildPanel"));
		if (BuildPanel)
		{
			TargetCanvas->AddChildToCanvas(BuildPanel);
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(BuildPanel->Slot))
			{
				CanvasSlot->SetAnchors(FAnchors(0.0f, 1.0f));
				CanvasSlot->SetAlignment(FVector2D(0.0f, 1.0f));
				CanvasSlot->SetPosition(FVector2D(10.0f, -80.0f));
				CanvasSlot->SetSize(FVector2D(300.0f, 70.0f));
				CanvasSlot->SetZOrder(10);
			}
			BuildPanel->SetVisibility(ESlateVisibility::Collapsed);
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanHUDRoot: auto-created BuildPanel"));
		}
	}

	// --- 全屏居中：ItemUseModal (弹窗时可见) ---
	if (!ItemUseModal)
	{
		ItemUseModal = CreateWidget<UOceanItemUseModalWidget>(this, UOceanItemUseModalWidget::StaticClass(), TEXT("ItemUseModal"));
		if (ItemUseModal)
		{
			TargetCanvas->AddChildToCanvas(ItemUseModal);
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ItemUseModal->Slot))
			{
				CanvasSlot->SetAnchors(FAnchors(0.5f, 0.5f));
				CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
				CanvasSlot->SetPosition(FVector2D(0.0f, 0.0f));
				CanvasSlot->SetSize(FVector2D(400.0f, 200.0f));
				CanvasSlot->SetZOrder(100);
			}
			ItemUseModal->SetVisibility(ESlateVisibility::Collapsed);
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanHUDRoot: auto-created ItemUseModal"));
		}
	}

	// --- 顶部居中：Toast ---
	if (!Toast)
	{
		Toast = CreateWidget<UOceanToastWidget>(this, UOceanToastWidget::StaticClass(), TEXT("Toast"));
		if (Toast)
		{
			TargetCanvas->AddChildToCanvas(Toast);
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Toast->Slot))
			{
				CanvasSlot->SetAnchors(FAnchors(0.5f, 0.15f));
				CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
				CanvasSlot->SetPosition(FVector2D(0.0f, 0.0f));
				CanvasSlot->SetSize(FVector2D(300.0f, 50.0f));
				CanvasSlot->SetZOrder(200);
			}
			Toast->SetVisibility(ESlateVisibility::Collapsed);
			UE_LOG(LogOcean, Log, TEXT("[TDD] OceanHUDRoot: auto-created Toast"));
		}
	}

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanHUDRoot: panels_ready status=%d backpack=%d build=%d time=%d modal=%d toast=%d"),
		StatusPanel != nullptr,
		BackpackPanel != nullptr,
		BuildPanel != nullptr,
		TimePanel != nullptr,
		ItemUseModal != nullptr,
		Toast != nullptr);
}

void UOceanHUDRootWidget::SetBackpackOpen(bool bNewBackpackOpen)
{
	if (bBackpackOpen == bNewBackpackOpen) return;
	bBackpackOpen = bNewBackpackOpen;

	if (BackpackPanel)
	{
		BackpackPanel->SetVisibility(bBackpackOpen ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanUIToggleBackpack: open=%d"), bBackpackOpen ? 1 : 0);
	OnBackpackOpenChanged(bBackpackOpen);
}

void UOceanHUDRootWidget::ToggleBackpack()
{
	SetBackpackOpen(!bBackpackOpen);
}

void UOceanHUDRootWidget::BindSurvivalToStatusPanel(UOceanSurvivalComponent* InSurvival)
{
	if (StatusPanel)
	{
		StatusPanel->BindSurvivalComponent(InSurvival);
	}
}

void UOceanHUDRootWidget::BindInventoryToBackpackPanel(UOceanInventoryComponent* InInventory, UOceanSurvivalComponent* InSurvival)
{
	if (BackpackPanel)
	{
		BackpackPanel->BindInventory(InInventory, InSurvival);
	}
}

void UOceanHUDRootWidget::BindBuildToBuildPanel(UOceanBuildComponent* InBuild)
{
	if (BuildPanel)
	{
		BuildPanel->BindBuildComponent(InBuild);
	}
}

void UOceanHUDRootWidget::ShowToast(const FText& Message, float Duration)
{
	if (Toast)
	{
		Toast->Show(Message, Duration);
	}
}

void UOceanHUDRootWidget::SetBuildPanelVisible(bool bVisible)
{
	if (BuildPanel)
	{
		BuildPanel->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBuildPanel: visible=%d"), bVisible ? 1 : 0);
	}
}
