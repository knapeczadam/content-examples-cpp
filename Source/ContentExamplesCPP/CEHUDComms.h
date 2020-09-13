#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/HUD.h"
#include "CEHUDComms.generated.h"

UENUM()
enum class ECEHUDCommsFont
{
    RobotoDistanceField
};

UENUM()
enum class ECEHUDCommsMaterial 
{
    CrossHair
};

UENUM()
enum class ECEHUDCommsCurve 
{
    VerticalOffset
};

/**
 * 
 */
UCLASS()
class CONTENTEXAMPLESCPP_API ACEHUDComms : public AHUD
{
    GENERATED_BODY()

    UPROPERTY()
    UTimelineComponent* TextPopTimeline = nullptr;

    UPROPERTY()
    UCurveFloat* VerticalOffsetCurve = nullptr;

    UPROPERTY()
    UMaterialInterface* ResourceMaterial = nullptr;

    float VerticalOffset = 0.0f;

    bool bCanInteract = false;

    float SizeX = 0.0f;

    float SizeY = 0.0f;
    
    bool bDoOnceFired = false;

    UPROPERTY()
    UFont* HUDFont = nullptr;

    UPROPERTY()
    TMap<ECEHUDCommsFont, UFont*> Fonts;

    UPROPERTY()
    TMap<ECEHUDCommsMaterial, UMaterialInterface*> Materials;

    UPROPERTY()
    TMap<ECEHUDCommsCurve, UCurveFloat*> Curves;

public:
    bool bPowersEnabled;
    
private:
    FORCEINLINE void ResetDoOnce() { bDoOnceFired = false; }

    UFUNCTION()
    void UpdateTimelineCallback();

    void InitializeAssets();

    void InitializeMaterials();

    void InitializeFonts();

    void InitializeCurves();
    
protected:
    virtual void BeginPlay() override;
    
public:
    ACEHUDComms();
    
    virtual void DrawHUD() override;
};
