#include "CELEDBasic.h"
#include "Components/PointLightComponent.h"
#include "Components/TimelineComponent.h"

// Sets default values
ACELEDBasic::ACELEDBasic()
{
	InitializeAssets();
	FlashCurve = Curves[ECELEDBasicCurve::Flash];
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    LED = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LED"));
	LED->SetStaticMesh(StaticMeshes[ECELEDBasicStaticMesh::Led]);
	LED->GetBodyInstance()->bAutoWeld = false;
	LED->bCastDynamicShadow = false;
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
}

// Called when the game starts or when spawned
void ACELEDBasic::BeginPlay()
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
 * Toggle Light with animation for transition
 */
void ACELEDBasic::ToggleLight(bool bIsOn)
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

/*
 * Event that changes th Light Birghtness
 */
void ACELEDBasic::ChangeLightBrightness(float NewBrightness)
{
	const FLinearColor NewColor = FLinearColor(0.3f, 0.6, 1.0f, NewBrightness); // "NewBrightness" value determines the brightness of the light
	UpdateLightColor(NewColor);
}

void ACELEDBasic::UpdateLightColor(FLinearColor NewColor)
{
	// Set Material Parameters
	LightBulbMaterial1->SetVectorParameterValue(FName(TEXT("Emissive color")), NewColor);
	LightBulbMaterial2->SetVectorParameterValue(FName(TEXT("Emissive color")), NewColor);

	// Set Light Parameters for Color and Brightness
	Light->SetLightColor(NewColor);
	Light->SetIntensity(NewColor.A * 2000.0f);
}

void ACELEDBasic::UpdateTimelineCallback()
{
	const float PlaybackPosition = LightFlashTimeline->GetPlaybackPosition();
	const float Flash = FlashCurve->GetFloatValue(PlaybackPosition);
	const FLinearColor NewColor = FLinearColor(0.3f, 0.6, 1.0f, Flash); // "Flash" value determines the brightness of the light
	UpdateLightColor(NewColor); // Custom function updates the color of the light component and light bulb materials
}

void ACELEDBasic::InitializeAssets()
{
	InitializeStaticMeshes();
	InitializeCurves();
}

void ACELEDBasic::InitializeStaticMeshes()
{
	// SM_LED
	static ConstructorHelpers::FObjectFinder<UStaticMesh> LedAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Meshes/SM_LED.SM_LED"));
	check(LedAsset.Object);
	StaticMeshes.Emplace(ECELEDBasicStaticMesh::Led, LedAsset.Object);
}

void ACELEDBasic::InitializeCurves()
{
	// Curve_Flash_LEDBasic_Float
	static ConstructorHelpers::FObjectFinder<UCurveFloat> FlashAsset(TEXT("/Game/ContentExamplesCPP/Core/Curve_Flash_LEDBasic_Float.Curve_Flash_LEDBasic_Float"));
	check(FlashAsset.Object);
	Curves.Emplace(ECELEDBasicCurve::Flash, FlashAsset.Object);
}
