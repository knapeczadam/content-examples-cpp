#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CELightBulbBatteryPowered.generated.h"

UENUM()
enum class ECELightBulbBatteryPoweredCurve
{
	Flash
};

UENUM()
enum class ECELightBulbBatteryPoweredStaticMesh
{
	LightBulb
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACELightBulbBatteryPowered : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY()
	UStaticMeshComponent* LightBulb;

	UPROPERTY()
	class UPointLightComponent* Light;

	UPROPERTY()
	class UBoxComponent* ConnectorTrigger;

	UPROPERTY()
	class UTimelineComponent* LightFlashTimeline;

	UPROPERTY()
	UMaterialInstanceDynamic* LightBulbMaterial1;

	UPROPERTY()
	UMaterialInstanceDynamic* LightBulbMaterial2;

	UPROPERTY()
	UCurveFloat* FlashCurve;

	FLinearColor LightColor;

	float ElectricitySupplied;

	UPROPERTY()
	TMap<ECELightBulbBatteryPoweredStaticMesh, UStaticMesh*> StaticMeshes;

	UPROPERTY()
	TMap<ECELightBulbBatteryPoweredCurve, UCurveFloat*> Curves;
	
private:
	void InitializeAssets();

	void InitializeStaticMeshes();

	void InitializeCurves();
	
private:
	UFUNCTION()
    void UpdateTimelineCallback();

	UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateLightColor(FLinearColor NewColor);

	void ToggleLight(bool bIsOn);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	ACELightBulbBatteryPowered();
};
