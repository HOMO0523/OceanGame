#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanPaper2DAnimationComponent.generated.h"

class AOceanCharacter;
class UPaperFlipbook;
class UPaperFlipbookComponent;

UENUM(BlueprintType)
enum class EOceanPaper2DAnimationState : uint8
{
	Idle,
	Walk,
	Jump,
	Swim,
	Climb,
	DiveSuitDive
};

UENUM(BlueprintType)
enum class EOceanPaper2DDirection : uint8
{
	South,
	West,
	East,
	North
};

UCLASS(ClassGroup=(Ocean), meta=(BlueprintSpawnableComponent))
class OCEAN_API UOceanPaper2DAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOceanPaper2DAnimationComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Idle")
	TObjectPtr<UPaperFlipbook> IdleSouth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Idle")
	TObjectPtr<UPaperFlipbook> IdleWest;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Idle")
	TObjectPtr<UPaperFlipbook> IdleEast;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Idle")
	TObjectPtr<UPaperFlipbook> IdleNorth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Walk")
	TObjectPtr<UPaperFlipbook> WalkSouth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Walk")
	TObjectPtr<UPaperFlipbook> WalkWest;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Walk")
	TObjectPtr<UPaperFlipbook> WalkEast;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Walk")
	TObjectPtr<UPaperFlipbook> WalkNorth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Jump")
	TObjectPtr<UPaperFlipbook> JumpSouth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Jump")
	TObjectPtr<UPaperFlipbook> JumpWest;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Jump")
	TObjectPtr<UPaperFlipbook> JumpEast;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Jump")
	TObjectPtr<UPaperFlipbook> JumpNorth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Swim")
	TObjectPtr<UPaperFlipbook> SwimSouth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Swim")
	TObjectPtr<UPaperFlipbook> SwimWest;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Swim")
	TObjectPtr<UPaperFlipbook> SwimEast;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Swim")
	TObjectPtr<UPaperFlipbook> SwimNorth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Climb")
	TObjectPtr<UPaperFlipbook> ClimbSouth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Climb")
	TObjectPtr<UPaperFlipbook> ClimbWest;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Climb")
	TObjectPtr<UPaperFlipbook> ClimbEast;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Climb")
	TObjectPtr<UPaperFlipbook> ClimbNorth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Dive")
	TObjectPtr<UPaperFlipbook> DiveSuitDiveSouth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Dive")
	TObjectPtr<UPaperFlipbook> DiveSuitDiveWest;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Dive")
	TObjectPtr<UPaperFlipbook> DiveSuitDiveEast;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ocean|Paper2D|Dive")
	TObjectPtr<UPaperFlipbook> DiveSuitDiveNorth;

	UFUNCTION(BlueprintCallable, Category="Ocean|Paper2D")
	void UpdatePresentation(AOceanCharacter* Character, UPaperFlipbookComponent* VisualComponent, float DeltaSeconds);

	UFUNCTION(BlueprintPure, Category="Ocean|Paper2D")
	EOceanPaper2DAnimationState GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category="Ocean|Paper2D")
	EOceanPaper2DDirection GetCurrentDirection() const { return CurrentDirection; }

	UFUNCTION(BlueprintCallable, Category="Ocean|Paper2D")
	void SetVisualStateRequest(EOceanPaper2DAnimationState State, bool bEnabled);

	UFUNCTION(BlueprintPure, Category="Ocean|Paper2D")
	static EOceanPaper2DDirection DirectionFromWorldVector(const FVector& WorldVector, EOceanPaper2DDirection FallbackDirection, float DeadZone = 5.0f);

	UFUNCTION(BlueprintPure, Category="Ocean|Paper2D")
	static EOceanPaper2DAnimationState SelectAnimationState(bool bWantsDiveSuitDive, bool bWantsClimb, bool bWantsSwim, bool bIsFalling, float GroundSpeed, float WalkThreshold = 5.0f);

	UFUNCTION(BlueprintPure, Category="Ocean|Paper2D")
	static FRotator CalculateCameraFacingRotation(const FVector& VisualLocation, const FVector& CameraLocation);

	UPaperFlipbook* GetFlipbook(EOceanPaper2DAnimationState State, EOceanPaper2DDirection Direction) const;

private:
	UPROPERTY(VisibleInstanceOnly, Category="Ocean|Paper2D")
	EOceanPaper2DAnimationState CurrentState = EOceanPaper2DAnimationState::Idle;

	UPROPERTY(VisibleInstanceOnly, Category="Ocean|Paper2D")
	EOceanPaper2DDirection CurrentDirection = EOceanPaper2DDirection::South;

	UPROPERTY(Transient)
	EOceanPaper2DAnimationState RequestedState = EOceanPaper2DAnimationState::Idle;

	UPROPERTY(Transient)
	bool bHasRequestedState = false;
};
