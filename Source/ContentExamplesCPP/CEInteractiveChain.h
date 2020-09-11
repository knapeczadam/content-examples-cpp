#pragma once

#include "CoreMinimal.h"
#include "CEPlayerInteractions.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "CEInteractiveChain.generated.h"

UENUM()
enum class ECEInteractiveChainMaterial
{
	OpaqueEmissive,
    OrangeShiny
};

UENUM()
enum class ECEInteractiveChainStaticMesh
{
	Box,
	Handle
};

UENUM()
enum class ECEInteractiveChainSkeletalMesh
{
    Chain
};

UENUM()
enum class ECEInteractiveChainCurve
{
	ChainHeight
};


UCLASS()
class CONTENTEXAMPLESCPP_API ACEInteractiveChain : public AActor, public ICEPlayerInteractions
{
	GENERATED_BODY()

	UPROPERTY()
	UStaticMeshComponent* ChainBase;

	UPROPERTY()
	USkeletalMeshComponent* Chain;

	UPROPERTY()
	UStaticMeshComponent* Handle;

	UPROPERTY()
	UStaticMeshComponent* HandleHighlighted;

	UPROPERTY()
	class UCapsuleComponent* InteractTrigger;

	UPROPERTY()
	class UTextRenderComponent* TextRender1;

	UPROPERTY()
	class UTimelineComponent* ChainPullTimeline;

	UPROPERTY()
	class UPhysicsHandleComponent* PhysicsHandle;

	UPROPERTY(EditAnywhere)
	class ACELEDBasic* TargetLED;

	bool bIsGrabbed;

	FVector HandleTarget;

	float DeltaSeconds;

	FVector RestingChainOffset;

	UPROPERTY()
	UCurveFloat* ChainHeightCurve;

	ETimelineDirection::Type Direction;

	UPROPERTY()
	TMap<ECEInteractiveChainMaterial, UMaterialInterface*> Materials;

	UPROPERTY()
	TMap<ECEInteractiveChainSkeletalMesh, USkeletalMesh*> SkeletalMeshes;

	UPROPERTY()
	TMap<ECEInteractiveChainStaticMesh, UStaticMesh*> StaticMeshes;

	UPROPERTY()
	TMap<ECEInteractiveChainCurve, UCurveFloat*> Curves;

	bool bA;

	bool bB;

	bool bIsOn;

private:
	void InitializeAssets();

	void InitializeStaticMeshes();

	void InitializeMaterials();

	void InitializeCurves();

	void InitializeSkeletalMeshes();

	UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void UpdateTimelineCallback();

	UFUNCTION()
	void FinishedTimelineCallback();

	void MoveChain(bool bDirection);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	ACEInteractiveChain();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual FVector GrabInteract(FVector2D PlayerAimMovement, bool bIsInteracting) override;
};
