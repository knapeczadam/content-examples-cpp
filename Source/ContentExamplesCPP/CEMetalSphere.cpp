#include "CEMetalSphere.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ACEMetalSphere::ACEMetalSphere()
{
	InitializeAssets();
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    Base = CreateDefaultSubobject<USceneComponent>(TEXT("Base"));
	RootComponent = Base;

    MetalSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Metal sphere"));
	MetalSphere->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
	MetalSphere->SetStaticMesh(StaticMeshes[ECEMetalSphereStaticMesh::Sphere]);
	MetalSphere->SetMaterial(0, Materials[ECEMetalSphereMaterial::MetalInteractive]);
	MetalSphere->GetBodyInstance()->bAutoWeld = false;
	MetalSphere->SetupAttachment(RootComponent);

	Smoke = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Smoke"));
	Smoke->Template = ParticleSystems[ECEMetalSphereParticleSystem::Smoke];
	Smoke->bAutoActivate = false;
	Smoke->SetupAttachment(RootComponent);

    Drips = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Drips"));
	Drips->SetRelativeLocation(FVector(0.0f, 0.0f, -25.0f));
	Drips->Template = ParticleSystems[ECEMetalSphereParticleSystem::WaterDrips];
	Drips->bAutoActivate = false;
	Drips->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACEMetalSphere::BeginPlay()
{
	Super::BeginPlay();
	MetalMaterial = MetalSphere->CreateDynamicMaterialInstance(0); // Create and set Dynamic Material Instance
}

// Called every frame
void ACEMetalSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Update Material Parameters over time
	DeltaSeconds = DeltaTime; // Set the time between frames for use with interp nodes
	// Wetness value increases interp speed
	Heat = FMath::FInterpTo(Heat, 0.0f, DeltaSeconds, Wetness + 0.25f); // Gradually reduce heat value to 0
	Burntness = FMath::FInterpTo(Burntness, Burntness + Heat, DeltaSeconds, 0.15f); // Gradually add heat value to burntness value
	Burntness = FMath::Clamp(Burntness, 0.0f, 1.0f);
	Wetness = FMath::FInterpTo(Wetness, 0.0f, DeltaSeconds, 0.3); // Gradually reduce wetness value to 0

	// Float values are used to drive various material effects
	MetalMaterial->SetScalarParameterValue(FName(TEXT("Burntness")), Burntness);
	MetalMaterial->SetScalarParameterValue(FName(TEXT("Heat")), Heat);
	MetalMaterial->SetScalarParameterValue(FName(TEXT("Wetness")), Wetness);
	Smoke->SetActive(Heat > 0.15f); // Turn on a smoke effect if heat is above a certain value
	Drips->SetActive(Wetness > 0.2f); // Turn on a dripping effect if wetness is above a certain value
}

/*
 * Set Material Parameters when ElementalDamage is received
 * Interface event to be called by other C++s
 */
void ACEMetalSphere::ElementalDamage(float HeatDamage, float WaterDamage)
{
	// Applying water to a hot surface? - Applying heat to a wet surface?
	if ((Heat > 0.05f && WaterDamage > 0.0f) || (HeatDamage > 0.0f && Wetness > 0.2f))
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ParticleSystems[ECEMetalSphereParticleSystem::SteamPuff], GetActorLocation()); // Spawn a steam puff effect
	}
	Heat = FMath::Clamp(HeatDamage * 0.1f + Heat, 0.0f, 1.0f); // Reduce incoming heat value
	Wetness = FMath::Clamp(Wetness + WaterDamage, 0.0f, 1.0f);	
}

void ACEMetalSphere::InitializeAssets()
{
	InitializeMaterials();
	InitializeParticleSystems();
	InitializeStaticMeshes();
}

void ACEMetalSphere::InitializeMaterials()
{
	// M_Metal_Interactive
	ConstructorHelpers::FObjectFinder<UMaterialInterface> MetalInteractiveAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Materials/M_Metal_Interactive.M_Metal_Interactive"));
	check(MetalInteractiveAsset.Object);
	Materials.Emplace(ECEMetalSphereMaterial::MetalInteractive, MetalInteractiveAsset.Object);
}

void ACEMetalSphere::InitializeParticleSystems()
{
	// P_Smoke
	ConstructorHelpers::FObjectFinder<UParticleSystem> SmokeAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Effects/P_Smoke.P_Smoke"));
	check(SmokeAsset.Object);
	ParticleSystems.Emplace(ECEMetalSphereParticleSystem::Smoke, SmokeAsset.Object);

	// P_Steam_Puff
	ConstructorHelpers::FObjectFinder<UParticleSystem> SteamPuffAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Effects/P_Steam_Puff.P_Steam_Puff"));
	check(SteamPuffAsset.Object);
	ParticleSystems.Emplace(ECEMetalSphereParticleSystem::SteamPuff, SteamPuffAsset.Object);

	// P_Water_Drips
	ConstructorHelpers::FObjectFinder<UParticleSystem> WaterDripsAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Effects/P_Water_Drips.P_Water_Drips"));
	check(WaterDripsAsset.Object);
	ParticleSystems.Emplace(ECEMetalSphereParticleSystem::WaterDrips, WaterDripsAsset.Object);	
}

void ACEMetalSphere::InitializeStaticMeshes()
{
	// EditorSphere
	ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("/Engine/EditorMeshes/EditorSphere.EditorSphere"));
	check(SphereAsset.Object);
	StaticMeshes.Emplace(ECEMetalSphereStaticMesh::Sphere, SphereAsset.Object);
}
