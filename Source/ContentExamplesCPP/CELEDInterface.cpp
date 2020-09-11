#include "CELEDInterface.h"
#include "Components/PointLightComponent.h"
#include "Components/TimelineComponent.h"

// Sets default values
ACELEDInterface::ACELEDInterface()
{
    InitializeAssets();
    FlashCurve = Curves[ECELEDInterfaceCurve::Flash];
    
    LED = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LED"));
    LED->SetStaticMesh(StaticMeshes[ECELEDInterfaceStaticMesh::Led]);
    LED->GetBodyInstance()->bAutoWeld = false;
    RootComponent = LED;

    Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
    Light->SetRelativeLocation(FVector(0.0f, 0.0f, 5.0f));
    Light->Intensity = 0.0f;
    Light->LightColor = FColor(255, 235, 196);
    Light->AttenuationRadius = 150.0f;
    Light->SourceRadius = 1.0f;
    Light->CastShadows = false;
    Light->CastStaticShadows = false;
    Light->SetupAttachment(RootComponent);

    LightFlashTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Light flash"));

    bIsA = true;
}

// Called when the game starts or when spawned
void ACELEDInterface::BeginPlay()
{
    Super::BeginPlay();

    // Create and set Dynamic Material Instance for element 0 and 1 on Staticmesh
    LightBulbMaterial1 = LED->CreateDynamicMaterialInstance(1);
    LightBulbMaterial2 = LED->CreateDynamicMaterialInstance(0);

    FOnTimelineFloat UpdateTimelineDelegate;
    UpdateTimelineDelegate.BindUFunction(this, FName(TEXT("UpdateTimelineCallback")));
    LightFlashTimeline->AddInterpFloat(FlashCurve, UpdateTimelineDelegate);
}

/*
 * Event to Toggle Light with animation for transition
 */
void ACELEDInterface::PushedButton()
{
    if (bIsA)
    {
        LightFlashTimeline->PlayFromStart();
    }
    else
    {
        LightFlashTimeline->Stop(); // Interrupt the timeline before turning off
        UpdateLightColor(FLinearColor::Black); // Change light color to black, effectively turning it off
    }
    bIsA = !bIsA;
}

void ACELEDInterface::UpdateLightColor(FLinearColor NewColor)
{
    LightBulbMaterial1->SetVectorParameterValue(FName(TEXT("Emissive color")), NewColor);
    LightBulbMaterial2->SetVectorParameterValue(FName(TEXT("Emissive color")), NewColor);
    Light->SetLightColor(NewColor);
    Light->SetIntensity(NewColor.A * 2000.0f);
}

void ACELEDInterface::UpdateTimelineCallback()
{
    const float PlaybackPosition = LightFlashTimeline->GetPlaybackPosition();
    const float Flash = FlashCurve->GetFloatValue(PlaybackPosition);
    const float Alpha = FMath::Lerp(3.0f, 8.0f, Flash);
    const FLinearColor NewColor = FLinearColor(0.3f, 0.6f, 1.0f, Alpha); // "Alpha" value determines the brightness of the light
    UpdateLightColor(NewColor); // Custom function updates the color of the light component and light bulb materials
}

void ACELEDInterface::InitializeAssets()
{
    InitializeStaticMeshes();
    InitializeCurves();
}

void ACELEDInterface::InitializeStaticMeshes()
{
    // SM_LED
    static ConstructorHelpers::FObjectFinder<UStaticMesh> LedAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Meshes/SM_LED.SM_LED"));
    check(LedAsset.Object);
    StaticMeshes.Emplace(ECELEDInterfaceStaticMesh::Led, LedAsset.Object);
}

void ACELEDInterface::InitializeCurves()
{
    // Curve_Flash_LEDInterface_Float
    static ConstructorHelpers::FObjectFinder<UCurveFloat> FlashAsset(TEXT("/Game/ContentExamplesCPP/Core/Curve_Flash_LEDInterface_Float.Curve_Flash_LEDInterface_Float"));
    check(FlashAsset.Object);
    Curves.Emplace(ECELEDInterfaceCurve::Flash, FlashAsset.Object);
}
