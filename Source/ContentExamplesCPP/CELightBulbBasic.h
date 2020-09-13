#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CELightBulbBasic.generated.h"

UENUM()
enum class ECELightBulbBasicCurve
{
    Flash
};

UENUM()
enum class ECELightBulbBasicStaticMesh
{
    LightBulb
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACELightBulbBasic : public AActor
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category="Light Bulb")
    UStaticMeshComponent* LightBulb = nullptr;

    UPROPERTY(EditAnywhere, Category="Light Bulb")
    class UPointLightComponent* Light = nullptr;

    UPROPERTY()
    class UTimelineComponent* LightFlashTimeline = nullptr;

    UPROPERTY()
    UMaterialInstanceDynamic* LightBulbMaterial1 = nullptr;

    UPROPERTY()
    UMaterialInstanceDynamic* LightBulbMaterial2 = nullptr;

    UPROPERTY()
    UCurveFloat* FlashCurve = nullptr;

    UPROPERTY()
    TMap<ECELightBulbBasicStaticMesh, UStaticMesh*> StaticMeshes;

    UPROPERTY()
    TMap<ECELightBulbBasicCurve, UCurveFloat*> Curves;

private:
    void InitializeAssets();

    void InitializeStaticMeshes();

    void InitializeCurves();

    UFUNCTION()
    void UpdateTimelineCallback();

    void UpdateLightColor(FLinearColor NewColor);
    
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Sets default values for this actor's properties
    ACELightBulbBasic();

    UFUNCTION()
    void ToggleLight(bool bIsOn);
};
