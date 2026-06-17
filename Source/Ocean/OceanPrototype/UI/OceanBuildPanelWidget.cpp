#include "OceanPrototype/UI/OceanBuildPanelWidget.h"
#include "OceanPrototype/OceanBuildComponent.h"
#include "OceanPrototype/OceanBuildModuleDefinition.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Ocean.h"

TSharedRef<SWidget> UOceanBuildPanelWidget::RebuildWidget()
{
	UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border"));
	Border->SetBrushColor(FLinearColor(0.05f, 0.05f, 0.08f, 0.85f));
	WidgetTree->RootWidget = Border;

	RootBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("RootBox"));
	Border->AddChild(RootBox);

	ModeText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ModeText"));
	ModeText->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
	ModeText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 14));
	RootBox->AddChildToVerticalBox(Cast<UWidget>(ModeText))->SetPadding(FMargin(8, 3));

	ModuleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ModuleText"));
	ModuleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	ModuleText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 12));
	RootBox->AddChildToVerticalBox(Cast<UWidget>(ModuleText))->SetPadding(FMargin(8, 2));

	RotationText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("RotationText"));
	RotationText->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)));
	RotationText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 11));
	RootBox->AddChildToVerticalBox(Cast<UWidget>(RotationText))->SetPadding(FMargin(8, 2));

	UpdateVisuals();
	return Super::RebuildWidget();
}

void UOceanBuildPanelWidget::BindBuildComponent(UOceanBuildComponent* InBuild)
{
	BuildComponent = InBuild;
	UE_LOG(LogOcean, Log, TEXT("[TDD] OceanBuildPanel: bound=1 active=%d rotation=%d"),
		InBuild ? InBuild->IsBuildModeActive() : -1, InBuild ? InBuild->GetRotationQuarterTurns() : -1);
	UpdateVisuals();
}

void UOceanBuildPanelWidget::SelectModule(UOceanBuildModuleDefinition* ModuleDef)
{
	if (BuildComponent.IsValid()) BuildComponent->SetSelectedModule(ModuleDef);
}

UOceanBuildModuleDefinition* UOceanBuildPanelWidget::GetSelectedModule() const { return BuildComponent.IsValid() ? BuildComponent->GetSelectedModule() : nullptr; }
int32 UOceanBuildPanelWidget::GetRotationQuarterTurns() const { return BuildComponent.IsValid() ? BuildComponent->GetRotationQuarterTurns() : 0; }
bool UOceanBuildPanelWidget::IsBuildModeActive() const { return BuildComponent.IsValid() && BuildComponent->IsBuildModeActive(); }

void UOceanBuildPanelWidget::UpdateVisuals()
{
	if (ModeText) ModeText->SetText(FText::FromString(
		IsBuildModeActive() ? TEXT("建造模式: 开启") : TEXT("建造模式: B键开启")));
	if (ModuleText)
	{
		UOceanBuildModuleDefinition* M = GetSelectedModule();
		ModuleText->SetText(M ? M->DisplayName : FText::FromString(TEXT("模块: Deck_1x1")));
	}
	if (RotationText) RotationText->SetText(FText::FromString(
		FString::Printf(TEXT("旋转: %d (R键)"), GetRotationQuarterTurns() * 90)));
}

void UOceanBuildPanelWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
	if (!BuildComponent.IsValid()) return;
	bool bA = BuildComponent->IsBuildModeActive();
	int32 R = BuildComponent->GetRotationQuarterTurns();
	UOceanBuildModuleDefinition* M = BuildComponent->GetSelectedModule();
	if (bA != LastBuildModeActive || R != LastRotationQuarterTurns || M != LastSelectedModule)
	{
		LastBuildModeActive = bA; LastRotationQuarterTurns = R; LastSelectedModule = M;
		UpdateVisuals();
	}
}
