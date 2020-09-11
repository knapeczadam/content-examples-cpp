#include "CELightBulbInterface.h"
#include "Components/PointLightComponent.h"
#include "Components/TimelineComponent.h"

// Sets default values
ACELightBulbInterface::ACELightBulbInterface()
{
    InitializeAssets();
    FlashCurve = Curves[ECELightBulbInterfaceCurve::Flash];
    
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    LightBulb = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Light bulb"));
    LightBulb->SetStaticMesh(StaticMeshes[ECELightBulbInterfaceStaticMesh::LightBulb]);
    LightBulb->GetBodyInstance()->bAutoWeld = false;
    RootComponent = LightBulb;

    Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
    Light->SetRelativeLocation(FVector(0.0f, 0.0f, 42.0f));
    Light->Intensity = 0.0f;
    Light->LightColor = FColor(255, 235, 196);
    Light->AttenuationRadius = 350.0f;
    Light->SourceRadius = 1.0f;
    Light->CastShadows = false;
    Light->CastStaticShadows = false;
    Light->SetupAttachment(RootComponent);

    LightFlashTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Light flash"));

    bIsA = true;
}

// Called when the game starts or when spawned
void ACELightBulbInterface::BeginPlay()
{
    Super::BeginPlay();

    // Create Dynamic Material instances for for each element of the Light Bulb mesh
    LightBulbMaterial1 = LightBulb->CreateDynamicMaterialInstance(0); // Create and apply Dynamic Material Instances for the light bulb mesh
    LightBulbMaterial2 = LightBulb->CreateDynamicMaterialInstance(1);

    FOnTimelineFloat UpdateTimelineDelegate;
    UpdateTimelineDelegate.BindUFunction(this, FName(TEXT("UpdateTimelineCallback")));
    LightFlashTimeline->AddInterpFloat(FlashCurve, UpdateTimelineDelegate);
}

void ACELightBulbInterface::UpdateLightColor(FLinearColor NewColor)
{
    // Set Material parameters to match lightcolor
    LightBulbMaterial1->SetVectorParameterValue(FName(TEXT("Emissive color")), NewColor);
    LightBulbMaterial2->SetVectorParameterValue(FName(TEXT("Emissive color")), NewColor);

    // Set the light color and brightness
    Light->SetLightColor(NewColor);
    Light->SetIntensity(NewColor.A * 2000.0f);
}

void ACELightBulbInterface::UpdateTimelineCallback()
{
    const float PlaybackPosition = LightFlashTimeline->GetPlaybackPosition();
    const float Flash = FlashCurve->GetFloatValue(PlaybackPosition);
    const FLinearColor NewColor = FLinearColor(0.3f, 0.6, 1.0f, Flash); // "Flash" value determines the brightness of the light
    UpdateLightColor(NewColor); // Custom function updates the color of the light component and light bulb materials
}

/*
 * Toggle Light with flicker transition
 */
void ACELightBulbInterface::PushedButton()
{
    if (bIsA)
    {
        LightFlashTimeline->PlayFromStart();
    }
    else
    {
        LightFlashTimeline->Stop(); // Interrupt timeline before turning off
        UpdateLightColor(FLinearColor::Black); // Change light color to black, effectively turning it off
    }
    bIsA = !bIsA;
}

void ACELightBulbInterface::InitializeAssets()
{
    InitializeStaticMeshes();
    InitializeCurves();
}

void ACELightBulbInterface::InitializeStaticMeshes()
{
    // SM_Lightbulb
    static ConstructorHelpers::FObjectFinder<UStaticMesh> LightBulbAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Meshes/SM_Lightbulb.SM_Lightbulb"));
    check(LightBulbAsset.Object);
    StaticMeshes.Emplace(ECELightBulbInterfaceStaticMesh::LightBulb, LightBulbAsset.Object);
}

void ACELightBulbInterface::InitializeCurves()
{
    // Curve_Flash_LightBulbInterface_Float
    static ConstructorHelpers::FObjectFinder<UCurveFloat> FlashAsset(TEXT("/Game/ContentExamplesCPP/Core/Curve_Flash_LightBulbInterface_Float.Curve_Flash_LightBulbInterface_Float"));
    check(FlashAsset.Object);
    Curves.Emplace(ECELightBulbInterfaceCurve::Flash, FlashAsset.Object);
}
