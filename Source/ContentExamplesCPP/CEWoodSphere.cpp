#include "CEWoodSphere.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ACEWoodSphere::ACEWoodSphere()
{
	InitializeAssets();
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    Base = CreateDefaultSubobject<USceneComponent>(TEXT("Base"));
	RootComponent = Base;

    WoodSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wood sphere"));
	WoodSphere->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
	WoodSphere->SetStaticMesh(StaticMeshes[ECEWoodSphereStaticMesh::Sphere]);
	WoodSphere->SetMaterial(0, Materials[ECEWoodSphereMaterial::WoodInteractive]);
	WoodSphere->GetBodyInstance()->bAutoWeld = false;
	WoodSphere->SetupAttachment(RootComponent);

	Fire = CreateDefaultSubobject<UParticleSystemComponent>(FName(TEXT("Fire")));
	Fire->Template = ParticleSystems[ECEWoodSphereParticleSystem::Flames];
	Fire->bAutoActivate = false;
	Fire->SetupAttachment(RootComponent);

    Smoke = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Smoke"));
	Smoke->Template = ParticleSystems[ECEWoodSphereParticleSystem::Smoke];
	Smoke->bAutoActivate = false;
    Smoke->SetupAttachment(RootComponent);

    Drips = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Drips"));
	Drips->SetRelativeLocation(FVector(0.0f, 0.0f, -25.0f));
	Drips->Template = ParticleSystems[ECEWoodSphereParticleSystem::WaterDrips];
	Drips->bAutoActivate = false;
	Drips->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACEWoodSphere::BeginPlay()
{
	Super::BeginPlay();
	WoodMaterial = WoodSphere->CreateDynamicMaterialInstance(0);
}

// Called every frame
void ACEWoodSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Sequence for updating material parameters
	// Wetness value increases interp speed
	Heat = FMath::FInterpTo(Heat, 0.0f, DeltaTime, Wetness + 0.25f); // Gradually reduce heat value to 0
	Burntness = FMath::FInterpTo(Burntness, Burntness + Heat, DeltaTime, 0.06f); // Gradually add heat value to burntness value
	Burntness = FMath::Clamp(Burntness, 0.0f, 1.0f);
	Wetness = FMath::FInterpTo(Wetness, 0.0f, DeltaTime, 0.18f); // Gradually reduce wetness value to 0

	// Float values are used to drive various material effects
	WoodMaterial->SetScalarParameterValue(FName(TEXT("Burntness")), Burntness);
	WoodMaterial->SetScalarParameterValue(FName(TEXT("Heat")), Heat);
	WoodMaterial->SetScalarParameterValue(FName(TEXT("Wetness")), Wetness);
	
	Smoke->SetActive(Heat > 0.05f); // Turn on a smoke effect if heat is above a certain value
	Fire->SetActive(Heat >= 0.25f); // Turn on a fire effect if heat is above a certain value
	Drips->SetActive(Wetness > 0.35f); // Turn on a dripping effect if wetness is above a certain value
}

/*
 * Sequence for being hit with projectiles
 * Interface event to be called by other C++s
 */
void ACEWoodSphere::ElementalDamage(float HeatDamage, float WaterDamage)
{
	// Applying water to a hot surface? - Applying heat to a wet surface?
	if ((Heat > 0.1f && WaterDamage > 0.3f) || (HeatDamage > 0.3f && Wetness > 0.3f))
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ParticleSystems[ECEWoodSphereParticleSystem::SteamPuff], GetActorLocation()); // Spawn a steam puff effect
	}
	Heat = FMath::Clamp(HeatDamage + Heat, 0.0f, 1.5f);
	Wetness = FMath::Clamp(Wetness + WaterDamage, 0.0f, 1.0f);	
}

void ACEWoodSphere::InitializeAssets()
{
	InitializeMaterials();
	InitializeParticleSystems();
	InitializeStaticMeshes();
}

void ACEWoodSphere::InitializeMaterials()
{
	// M_Wood_Interactive
	ConstructorHelpers::FObjectFinder<UMaterialInterface> WoodInteractiveAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Materials/M_Wood_Interactive.M_Wood_Interactive"));
	check(WoodInteractiveAsset.Object);
	Materials.Emplace(ECEWoodSphereMaterial::WoodInteractive, WoodInteractiveAsset.Object);
}

void ACEWoodSphere::InitializeParticleSystems()
{
	// P_Flames
	ConstructorHelpers::FObjectFinder<UParticleSystem> FlamesAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Effects/P_Flames.P_Flames"));
	check(FlamesAsset.Object);
	ParticleSystems.Emplace(ECEWoodSphereParticleSystem::Flames, FlamesAsset.Object);
	
	// P_Smoke
	ConstructorHelpers::FObjectFinder<UParticleSystem> SmokeAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Effects/P_Smoke.P_Smoke"));
	check(SmokeAsset.Object);
	ParticleSystems.Emplace(ECEWoodSphereParticleSystem::Smoke, SmokeAsset.Object);

	// P_Steam_Puff
	ConstructorHelpers::FObjectFinder<UParticleSystem> SteamPuffAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Effects/P_Steam_Puff.P_Steam_Puff"));
	check(SteamPuffAsset.Object);
	ParticleSystems.Emplace(ECEWoodSphereParticleSystem::SteamPuff, SteamPuffAsset.Object);

	// P_Water_Drips
	ConstructorHelpers::FObjectFinder<UParticleSystem> WaterDripsAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Effects/P_Water_Drips.P_Water_Drips"));
	check(WaterDripsAsset.Object);
	ParticleSystems.Emplace(ECEWoodSphereParticleSystem::WaterDrips, WaterDripsAsset.Object);	
}

void ACEWoodSphere::InitializeStaticMeshes()
{
	// EditorSphere
	ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("/Engine/EditorMeshes/EditorSphere.EditorSphere"));
	check(SphereAsset.Object);
	StaticMeshes.Emplace(ECEWoodSphereStaticMesh::Sphere, SphereAsset.Object);
}
