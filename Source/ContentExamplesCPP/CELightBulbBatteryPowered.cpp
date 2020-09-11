#include "CELightBulbBatteryPowered.h"
#include "CEBattery.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/TimelineComponent.h"

// Sets default values
ACELightBulbBatteryPowered::ACELightBulbBatteryPowered()
{
	InitializeAssets();
	FlashCurve = Curves[ECELightBulbBatteryPoweredCurve::Flash];
	
    LightBulb = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Light bulb"));
	LightBulb->SetStaticMesh(StaticMeshes[ECELightBulbBatteryPoweredStaticMesh::LightBulb]);
	LightBulb->GetBodyInstance()->bAutoWeld = false;
	RootComponent = LightBulb;

    Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	Light->SetRelativeLocation(FVector(0.0f, 0.0f, 42.0f));
	Light->SetIntensity(0.0f);
	Light->LightColor = FColor(255, 235, 196);
	Light->AttenuationRadius = 350.0f;
	Light->SourceRadius = 1.0f;
	Light->CastShadows = false;
	Light->CastStaticShadows = false;
	Light->SetupAttachment(RootComponent);

	ConnectorTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Connector trigger"));
	ConnectorTrigger->SetRelativeLocation(FVector(0.0f, 0.0f, 2.5f));
	ConnectorTrigger->InitBoxExtent(FVector(2.5f, 2.5f, 2.5f));
	ConnectorTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	ConnectorTrigger->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	ConnectorTrigger->SetupAttachment(RootComponent);

	LightFlashTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Light flash"));

	ElectricitySupplied = 0.0f;
	LightColor = FLinearColor(1,0.7f,0.3f,10);
}

// Called when the game starts or when spawned
void ACELightBulbBatteryPowered::BeginPlay()
{
	Super::BeginPlay();
	// Create Dynamic Material instances for for each element of the Light Bulb mesh
	LightBulbMaterial1 = LightBulb->CreateDynamicMaterialInstance(0); // Create and apply Dynamic Material Instances for the light bulb mesh
	LightBulbMaterial2 = LightBulb->CreateDynamicMaterialInstance(1);

	ConnectorTrigger->OnComponentBeginOverlap.AddDynamic(this, &ACELightBulbBatteryPowered::OnTriggerBeginOverlap);
	ConnectorTrigger->OnComponentEndOverlap.AddDynamic(this, &ACELightBulbBatteryPowered::OnTriggerEndOverlap);

	// Check for overlapping batteries on play
	TArray<AActor*> OverlappingActors;
	ConnectorTrigger->GetOverlappingActors(OverlappingActors);
	for (AActor* OverlappingActor : OverlappingActors)
	{
		ACEBattery* Battery = Cast<ACEBattery>(OverlappingActor);
		if (Battery)
		{
			ElectricitySupplied += Battery->GetElectricity();
			bool bIsOn = ElectricitySupplied >= 0; // toggle light depending on Electricity Supplied
			ToggleLight(bIsOn); // Fire the custom event for turning the light on/off
		}
	}

	FOnTimelineFloat UpdateTimelineDelegate;
	UpdateTimelineDelegate.BindUFunction(this, FName(TEXT("UpdateTimelineCallback")));
	LightFlashTimeline->AddInterpFloat(FlashCurve, UpdateTimelineDelegate);
}

void ACELightBulbBatteryPowered::UpdateLightColor(FLinearColor NewColor)
{
	LightBulbMaterial1->SetVectorParameterValue(FName(TEXT("Emissive color")), NewColor);
	LightBulbMaterial2->SetVectorParameterValue(FName(TEXT("Emissive color")), NewColor);
	Light->SetLightColor(NewColor);
	Light->SetIntensity(NewColor.A * 2000.0f);
}

/*
 * Toggle Light with flicker transition
 */
void ACELightBulbBatteryPowered::ToggleLight(bool bIsOn)
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
 * Dynamic Light toggle depending on the other actors Electricity Value
 * Event fires when an actor overlaps the trigger
 */
void ACELightBulbBatteryPowered::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		ACEBattery* Battery = Cast<ACEBattery>(OtherActor);
		if (Battery) // Check whether the actor is a battery
		{
			// // Float value retrieved from the battery
			ElectricitySupplied += Battery->GetElectricity(); // Add battery value to the lightbulb's value
			bool bIsOn = ElectricitySupplied >= 0;
			ToggleLight(bIsOn);
		}	
	}
}

/*
 * Dynamic Light toggle depending on the other actors Electricity Value
 * Event fires when an actor exits the trigger
 */
void ACELightBulbBatteryPowered::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		ACEBattery* Battery = Cast<ACEBattery>(OtherActor);
		if (Battery)
		{
			ElectricitySupplied -= Battery->GetElectricity(); // Subtract battery value
			bool bIsOn = ElectricitySupplied >= 0;
			ToggleLight(bIsOn);
		}	
	}
}

void ACELightBulbBatteryPowered::UpdateTimelineCallback()
{
	const float PlaybackPosition = LightFlashTimeline->GetPlaybackPosition();
	const float Flash = FlashCurve->GetFloatValue(PlaybackPosition);
	const float NewAlpha = FMath::Lerp(ElectricitySupplied, ElectricitySupplied * 2.0f, Flash); // Electricity value modifies the light brightness
	const FLinearColor NewColor = FLinearColor(1.0f, 0.7f, 0.3f, NewAlpha); // "A" value determines the brightness of the light
	UpdateLightColor(NewColor); // Custom function updates the color of the light component and light bulb materials
}

void ACELightBulbBatteryPowered::InitializeAssets()
{
	InitializeStaticMeshes();
	InitializeCurves();
}

void ACELightBulbBatteryPowered::InitializeStaticMeshes()
{
	// SM_Lightbulb
	static ConstructorHelpers::FObjectFinder<UStaticMesh> LightBulbAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Meshes/SM_Lightbulb.SM_Lightbulb"));
	check(LightBulbAsset.Object);
	StaticMeshes.Emplace(ECELightBulbBatteryPoweredStaticMesh::LightBulb, LightBulbAsset.Object);
}

void ACELightBulbBatteryPowered::InitializeCurves()
{
	//  Curve_Flash_LightBulbBatteryPowered_Float
	static ConstructorHelpers::FObjectFinder<UCurveFloat> FlashAsset(TEXT("/Game/ContentExamplesCPP/Core/Curve_Flash_LightBulbBatteryPowered_Float.Curve_Flash_LightBulbBatteryPowered_Float"));
	check(FlashAsset.Object);
	Curves.Emplace(ECELightBulbBatteryPoweredCurve::Flash, FlashAsset.Object);
}
