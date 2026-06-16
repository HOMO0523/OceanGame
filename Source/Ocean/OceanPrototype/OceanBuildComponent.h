#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanBuildComponent.generated.h"

class AOceanBuildModuleActor;
class AOceanFloatingPlatform;
class UOceanBuildModuleDefinition;

UCLASS(ClassGroup = (Ocean), meta = (BlueprintSpawnableComponent))
class OCEAN_API UOceanBuildComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanBuildComponent();

	UFUNCTION(BlueprintCallable, Category = "Ocean|Build")
	void ToggleBuildMode();

	UFUNCTION(BlueprintCallable, Category = "Ocean|Build")
	void SetBuildModeActive(bool bNewBuildModeActive);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Build")
	void RotatePreview();

	UFUNCTION(BlueprintCallable, Category = "Ocean|Build")
	bool TryPlaceSelectedModuleAtWorld(const FVector& WorldLocation, FText& OutMessage);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Build")
	void SetSelectedModule(UOceanBuildModuleDefinition* ModuleDefinition);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Build")
	void SetTargetPlatform(AOceanFloatingPlatform* Platform);

	UFUNCTION(BlueprintCallable, Category = "Ocean|Build")
	void SetFallbackModuleActorClass(TSubclassOf<AOceanBuildModuleActor> ActorClass);

	UFUNCTION(BlueprintPure, Category = "Ocean|Build")
	bool IsBuildModeActive() const { return bBuildModeActive; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Build")
	int32 GetRotationQuarterTurns() const { return RotationQuarterTurns; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Build")
	UOceanBuildModuleDefinition* GetSelectedModule() const { return SelectedModule; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Build")
	AOceanFloatingPlatform* GetTargetPlatform() const { return TargetPlatform; }

	UFUNCTION(BlueprintPure, Category = "Ocean|Build")
	AOceanBuildModuleActor* GetLastPlacedModuleActor() const { return LastPlacedModuleActor; }

private:
	UPROPERTY(EditAnywhere, Category = "Ocean|Build")
	TObjectPtr<UOceanBuildModuleDefinition> SelectedModule;

	UPROPERTY(EditAnywhere, Category = "Ocean|Build")
	TObjectPtr<AOceanFloatingPlatform> TargetPlatform;

	UPROPERTY(EditAnywhere, Category = "Ocean|Build")
	TSubclassOf<AOceanBuildModuleActor> FallbackModuleActorClass;

	UPROPERTY(VisibleAnywhere, Category = "Ocean|Build")
	TObjectPtr<AOceanBuildModuleActor> LastPlacedModuleActor;

	AOceanFloatingPlatform* ResolveTargetPlatform();

	bool bBuildModeActive = false;
	int32 RotationQuarterTurns = 0;
};
