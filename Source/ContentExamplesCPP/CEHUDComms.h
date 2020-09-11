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
    UTimelineComponent* TextPopTimeline;

    UPROPERTY()
    UCurveFloat* VerticalOffsetCurve;

    UPROPERTY()
    UMaterialInterface* ResourceMaterial;

    float VerticalOffset;

    bool bCanInteract;

    float SizeX, SizeY;
    
    bool bDoOnceFired;

    UFont* HUDFont;

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
