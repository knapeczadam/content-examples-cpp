#include "CELightBulbPulse.h"
#include "Components/PointLightComponent.h"
#include "Components/TimelineComponent.h"

// Sets default values
ACELightBulbPulse::ACELightBulbPulse()
{
	InitializeAssets();
	FlashCurve = Curves[ECELightBulbPulseCurve::Flash];

    LightBulb = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Light bulb"));
	LightBulb->SetStaticMesh(StaticMeshes[ECELightBulbPulseStaticMesh::LightBulb]);
	LightBulb->GetBodyInstance()->bAutoWeld = false;
	RootComponent = LightBulb;

    Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	Light->SetRelativeLocation(FVector(0.0f, 0.0f, 42.0f));
	Light->Intensity = 0.0f;
	Light->LightColor = FColor(255, 235, 196);
	Light->AttenuationRadius = 300.0f;
	Light->SourceRadius = 1.0f;
	Light->CastStaticShadows = false;
	Light->SetupAttachment(RootComponent);

    LightPulseTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Light pulse"));
}

// Called when the game starts or when spawned
void ACELightBulbPulse::BeginPlay()
{
	Super::BeginPlay();

	// Create Dynamic Material instances for for each element of the Light Bulb mesh
	LightBulbMaterial1 = LightBulb->CreateDynamicMaterialInstance(0); // Create and apply Dynamic Material Instances for the light bulb mesh
	LightBulbMaterial2 = LightBulb->CreateDynamicMaterialInstance(1);

	FOnTimelineFloat UpdateTimelineDelegate;
	UpdateTimelineDelegate.BindUFunction(this, FName(TEXT("UpdateTimelineCallback")));
	LightPulseTimeline->AddInterpFloat(FlashCurve, UpdateTimelineDelegate);
	LightPulseTimeline->SetLooping(true);
}

/*
 * Toggle Light with flicker transition
 */
void ACELightBulbPulse::ToggleLight(bool bIsOn)
{
	if (bIsOn)
	{
		const FLinearColor ColorA = FLinearColor(1.0f, 0.7f, 0.25f, 1.0f);
		const FLinearColor ColorB = FLinearColor(1.0f, 0.5f, 0.2f, 1.0f);
		const float Alpha = FMath::RandRange(0.0f, 1.0f);
		const FLinearColor NewColor = FMath::Lerp(ColorA, ColorB, Alpha); // Blend between two colors
		LightColor = NewColor; // Set random light color to use
		const float NewRate = FMath::RandRange(0.4f, 0.8f);
		LightPulseTimeline->SetPlayRate(NewRate); // Set a random play rate for the timeline
		LightPulseTimeline->PlayFromStart();
	}
	else
	{
		LightPulseTimeline->Stop(); // Interrupt timeline before turning off
		UpdateLightColor(FLinearColor::Black); // Change light color to black, effectively turning it off
	}
}

void ACELightBulbPulse::UpdateLightColor(FLinearColor NewColor)
{
	// Set Material parameters to match lightcolor
	LightBulbMaterial1->SetVectorParameterValue(FName("Emissive color"), NewColor);
	LightBulbMaterial2->SetVectorParameterValue(FName("Emissive color"), NewColor);
	
	// Set the light color and brightness
	Light->SetLightColor(NewColor);
	Light->SetIntensity(NewColor.A * 1500.0f);
}

void ACELightBulbPulse::UpdateTimelineCallback()
{
	const float PlaybackPosition = LightPulseTimeline->GetPlaybackPosition();
	const float Flash = FlashCurve->GetFloatValue(PlaybackPosition);

	// Use only the RGB values
	const float NewAlpha = FMath::Lerp(0.0f, 10.0f, Flash);
	const FLinearColor NewColor = FLinearColor(LightColor.R, LightColor.G, LightColor.B, NewAlpha); // "NewAlpha" value determines the brightness of the light
	UpdateLightColor(NewColor); // Custom function updates the color of the light component and light bulb materials
}

void ACELightBulbPulse::InitializeAssets()
{
	InitializeStaticMeshes();
	InitializeCurves();
}

void ACELightBulbPulse::InitializeStaticMeshes()
{
	// SM_Lightbulb
	static ConstructorHelpers::FObjectFinder<UStaticMesh> LightBulbAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Meshes/SM_Lightbulb.SM_Lightbulb"));
	check(LightBulbAsset.Object);
	StaticMeshes.Emplace(ECELightBulbPulseStaticMesh::LightBulb, LightBulbAsset.Object);
}

void ACELightBulbPulse::InitializeCurves()
{
	//  Curve_Flash_LightBulbPulse_Float
	static ConstructorHelpers::FObjectFinder<UCurveFloat> FlashAsset(TEXT("/Game/ContentExamplesCPP/Core/Curve_Flash_LightBulbPulse_Float.Curve_Flash_LightBulbPulse_Float"));
	check(FlashAsset.Object);
	Curves.Emplace(ECELightBulbPulseCurve::Flash, FlashAsset.Object);
}
