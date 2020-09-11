#pragma once


#include "CoreMinimal.h"
#include "CEPlayerInteractions.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "CEInteractiveSwitch.generated.h"

UENUM()
enum class ECEInteractiveSwitchCurve
{
	Pitch
};

UENUM()
enum class ECEInteractiveSwitchMaterial
{
	OpaqueEmissive,
	OrangeShiny
};

UENUM()
enum class ECEInteractiveSwitchStaticMesh
{
	Handle,
	Switch
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACEInteractiveSwitch : public AActor, public ICEPlayerInteractions
{
	GENERATED_BODY()

	UPROPERTY()
	UStaticMeshComponent* SwitchBase;

	UPROPERTY()
	UStaticMeshComponent* SwitchHandle;

	UPROPERTY()
	UStaticMeshComponent* SwitchHandleHighlighted;

	UPROPERTY()
	class UBoxComponent* InteractTrigger;

	UPROPERTY()
	class UTextRenderComponent* TextRender1;

	UPROPERTY()
	class UTimelineComponent* RotateSwitchTimeline;

	UPROPERTY()
	UCurveFloat* PitchCurve;

	ETimelineDirection::Type TimelineDirection;

	bool bSwitchPosition;

	UPROPERTY(EditAnywhere)
	class ACELEDBasic* TargetLED;

	UPROPERTY()
	TMap<ECEInteractiveSwitchCurve, UCurveFloat*> Curves;

	UPROPERTY()
	TMap<ECEInteractiveSwitchMaterial, UMaterialInterface*> Materials;

	UPROPERTY()
	TMap<ECEInteractiveSwitchStaticMesh, UStaticMesh*> StaticMeshes;
	
private:
	void InitializeAssets();

	void InitializeStaticMeshes();

	void InitializeCurves();

	void InitializeMaterials();

	void FlipSwitch();

	UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
    void UpdateTimelineCallback();

	UFUNCTION()
    void FinishedTimelineCallback();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	ACEInteractiveSwitch();

	virtual FVector GrabInteract(FVector2D PlayerAimMovement, bool bIsInteracting) override;
};
