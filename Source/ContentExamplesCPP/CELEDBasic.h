#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CELEDBasic.generated.h"

UENUM()
enum class ECELEDBasicCurve
{
	Flash
};

UENUM()
enum class ECELEDBasicStaticMesh
{
	Led
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACELEDBasic : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* LED = nullptr;

	UPROPERTY()
	class UPointLightComponent* Light = nullptr;

	UPROPERTY()
	class UTimelineComponent* LightFlashTimeline = nullptr;

	UPROPERTY()
	UCurveFloat* FlashCurve = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* LightBulbMaterial1 = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* LightBulbMaterial2 = nullptr;

	UPROPERTY()
	TMap<ECELEDBasicCurve, UCurveFloat*> Curves;

	UPROPERTY()
	TMap<ECELEDBasicStaticMesh, UStaticMesh*> StaticMeshes;
	
private:
	void InitializeAssets();

	void InitializeStaticMeshes();

	void InitializeCurves();

	void UpdateLightColor(FLinearColor NewColor);

	UFUNCTION()
    void UpdateTimelineCallback();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	ACELEDBasic();

	void ChangeLightBrightness(float NewBrightness);

	void ToggleLight(bool bIsOn);
};
