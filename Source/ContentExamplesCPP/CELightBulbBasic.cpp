#include "CELightBulbBasic.h"

#include "Components/PointLightComponent.h"
#include "Components/TimelineComponent.h"

// Sets default values
ACELightBulbBasic::ACELightBulbBasic()
{
    InitializeAssets();
    FlashCurve = Curves[ECELightBulbBasicCurve::Flash];

    LightBulb = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Light bulb"));
    LightBulb->SetStaticMesh(StaticMeshes[ECELightBulbBasicStaticMesh::LightBulb]);
    LightBulb->GetBodyInstance()->bAutoWeld = false;
    RootComponent = LightBulb;

    Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
    Light->SetRelativeLocation(FVector(0.0f, 0.0f, 42.0f));
    Light->LightColor = FColor(255, 235, 196);
    Light->AttenuationRadius = 350.0f;
    Light->SourceRadius = 1.0f;
    Light->CastShadows = false;
    Light->CastStaticShadows = false;
    Light->SetupAttachment(LightBulb);

    LightFlashTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Light flash"));
}

// Called when the game starts or when spawned
void ACELightBulbBasic::BeginPlay()
{
    Super::BeginPlay();

    /* Create Dynamic Material instances for for each element of the Light Bulb mesh */
    LightBulbMaterial1 = LightBulb->CreateDynamicMaterialInstance(0);
    LightBulbMaterial2 = LightBulb->CreateDynamicMaterialInstance(1); // Create and apply Dynamic Material Instances for the light bulb mesh

    FOnTimelineFloat UpdateTimelineDelegate;
    UpdateTimelineDelegate.BindUFunction(this, FName(TEXT("UpdateTimelineCallback")));
    LightFlashTimeline->AddInterpFloat(FlashCurve, UpdateTimelineDelegate);
}

/**
 * Toggle Light with flicker transition
 */
void ACELightBulbBasic::ToggleLight(bool bIsOn)
{
    if (bIsOn)
    {
        LightFlashTimeline->PlayFromStart();
    }
    else
    {
        LightFlashTimeline->Stop(); // Interrupt timeline before turning off
        UpdateLightColor(FLinearColor::Black); // Change light color to black, effectively turning it off
    }
}

void ACELightBulbBasic::UpdateLightColor(FLinearColor NewColor)
{

    /* Set Material parameters to match lightcolor */
    LightBulbMaterial1->SetVectorParameterValue(FName(TEXT("Emissive Color")), NewColor); 
    LightBulbMaterial2->SetVectorParameterValue(FName(TEXT("Emissive Color")), NewColor);

    /* Set the light color and brightness */
    Light->SetLightColor(NewColor);
    Light->SetIntensity(NewColor.A * 2000.0f);
}

void ACELightBulbBasic::UpdateTimelineCallback()
{
    const float PlaybackPosition = LightFlashTimeline->GetPlaybackPosition();
    const float Flash = FlashCurve->GetFloatValue(PlaybackPosition);
    const FLinearColor NewColor = FLinearColor(1.0f, 0.7f, 0.3f, Flash); // "InA" value determines the brightness of the light
    UpdateLightColor(NewColor); // Custom function updates the color of the light component and light bulb materials
}

void ACELightBulbBasic::InitializeAssets()
{
    InitializeCurves();
    InitializeStaticMeshes();
}

void ACELightBulbBasic::InitializeStaticMeshes()
{
    // SM_Lightbulb
    static ConstructorHelpers::FObjectFinder<UStaticMesh> LightBulbAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Meshes/SM_Lightbulb.SM_Lightbulb"));
    check(LightBulbAsset.Object);
    StaticMeshes.Emplace(ECELightBulbBasicStaticMesh::LightBulb, LightBulbAsset.Object);
}

void ACELightBulbBasic::InitializeCurves()
{
    // Curve_Flash_LightBulbBasic_Float
    static ConstructorHelpers::FObjectFinder<UCurveFloat> FlashAsset(TEXT("/Game/ContentExamplesCPP/Core/Curve_Flash_LightBulbBasic_Float.Curve_Flash_LightBulbBasic_Float"));
    check(FlashAsset.Object);
    Curves.Emplace(ECELightBulbBasicCurve::Flash, FlashAsset.Object);
}
