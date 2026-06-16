#include "OceanPrototype/OceanInteractionComponent.h"

#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "OceanPrototype/OceanInteractableInterface.h"
#include "Engine/OverlapResult.h"

namespace
{
bool HasBlueprintInteractionOverride(AActor* Candidate, const FName FunctionName)
{
	const UFunction* Function = Candidate ? Candidate->FindFunction(FunctionName) : nullptr;
	return Function && !Function->GetOwnerClass()->HasAnyClassFlags(CLASS_Native);
}

bool DispatchCanOceanInteract(AActor* Candidate, AActor* Interactor, FText& FailureReason)
{
	static const FName CanInteractName(TEXT("CanOceanInteract"));
	if (HasBlueprintInteractionOverride(Candidate, CanInteractName))
	{
		return IOceanInteractableInterface::Execute_CanOceanInteract(Candidate, Interactor, FailureReason);
	}

	if (IOceanInteractableInterface* NativeInterface = Cast<IOceanInteractableInterface>(Candidate))
	{
		return NativeInterface->CanOceanInteract_Implementation(Interactor, FailureReason);
	}

	return IOceanInteractableInterface::Execute_CanOceanInteract(Candidate, Interactor, FailureReason);
}

bool DispatchExecuteOceanInteraction(AActor* Target, AActor* Interactor, FText& OutMessage)
{
	static const FName ExecuteInteractionName(TEXT("ExecuteOceanInteraction"));
	if (HasBlueprintInteractionOverride(Target, ExecuteInteractionName))
	{
		return IOceanInteractableInterface::Execute_ExecuteOceanInteraction(Target, Interactor, OutMessage);
	}

	if (IOceanInteractableInterface* NativeInterface = Cast<IOceanInteractableInterface>(Target))
	{
		return NativeInterface->ExecuteOceanInteraction_Implementation(Interactor, OutMessage);
	}

	return IOceanInteractableInterface::Execute_ExecuteOceanInteraction(Target, Interactor, OutMessage);
}
}

UOceanInteractionComponent::UOceanInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

AActor* UOceanInteractionComponent::FindBestInteractable() const
{
	const AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World)
	{
		return nullptr;
	}

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(OceanInteractionOverlap), false, Owner);
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	const FVector Origin = Owner->GetActorLocation();
	const float ClampedInteractionRadius = FMath::Max(1.0f, InteractionRadius);
	World->OverlapMultiByObjectType(
		Overlaps,
		Origin,
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(ClampedInteractionRadius),
		QueryParams);

	AActor* BestActor = nullptr;
	float BestDistanceSquared = TNumericLimits<float>::Max();
	TSet<TObjectPtr<AActor>> CheckedActors;

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Candidate = Overlap.GetActor();
		if (!IsValid(Candidate) || Candidate == Owner || CheckedActors.Contains(Candidate))
		{
			continue;
		}

		CheckedActors.Add(Candidate);

		if (!Candidate->GetClass()->ImplementsInterface(UOceanInteractableInterface::StaticClass()))
		{
			continue;
		}

		FText FailureReason;
		if (!DispatchCanOceanInteract(Candidate, const_cast<AActor*>(Owner), FailureReason))
		{
			continue;
		}

		const float DistanceSquared = FVector::DistSquared(Origin, Candidate->GetActorLocation());
		if (DistanceSquared < BestDistanceSquared)
		{
			BestDistanceSquared = DistanceSquared;
			BestActor = Candidate;
		}
	}

	return BestActor;
}

bool UOceanInteractionComponent::TryInteract(FText& OutMessage)
{
	AActor* Target = FindBestInteractable();
	if (!Target)
	{
		OutMessage = FText::FromString(TEXT("没有可交互目标"));
		return false;
	}

	return DispatchExecuteOceanInteraction(Target, GetOwner(), OutMessage);
}

void UOceanInteractionComponent::SetInteractionRadius(float NewRadius)
{
	InteractionRadius = FMath::Max(1.0f, NewRadius);
}
