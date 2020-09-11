#include "CEIceSphere.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ACEIceSphere::ACEIceSphere()
{
	InitializeAssets();
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Base = CreateDefaultSubobject<USceneComponent>(TEXT("Base"));
	RootComponent = Base;

	IceSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ice sphere"));
	IceSphere->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
	IceSphere->SetStaticMesh(StaticMeshes[ECEIceSphereStaticMesh::Sphere]);
	IceSphere->SetMaterial(0, Materials[ECEIceSphereMaterial::IceInteractive]);
	IceSphere->GetBodyInstance()->bAutoWeld = false;
	IceSphere->SetupAttachment(RootComponent);

    Drips = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Drips"));
	Drips->SetRelativeLocation(FVector(0.0f, 0.0f, -25.0f));
	Drips->Template = ParticleSystems[ECEIceSphereParticleSystem::WaterDrips];
	Drips->bAutoActivate = false;
	Drips->SetupAttachment(RootComponent);

    Steam = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Steam"));
	Steam->Template = ParticleSystems[ECEIceSphereParticleSystem::IceSteam];
	Steam->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACEIceSphere::BeginPlay()
{
	Super::BeginPlay();
	IceMaterial = IceSphere->CreateDynamicMaterialInstance(0); // Create and set Dynamic Material Instance to Mesh
}

// Called every frame
void ACEIceSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Sequence for updating material parameters
	Heat = FMath::FInterpTo(Heat, 0.0f,  DeltaTime, Wetness + 0.1f); // Gradually recude heat value to 0
	Melted = FMath::FInterpTo(Melted, Melted + Heat + Wetness * 0.5f, DeltaTime, 0.08f); // Gradually add heat and wetness to melted value
	Melted = FMath::Clamp(Melted, 0.0f, 1.0f);
	const float TargetIcicles = FMath::Clamp(Icicles + Wetness - Heat, 0.0f, 1.0f);
	Icicles = FMath::FInterpTo(Icicles, TargetIcicles, DeltaTime, 0.3f); // Modify icicles value based on heat and wetness
	Wetness = FMath::FInterpTo(Wetness, Heat * 0.5f, DeltaTime, 0.3f); // Modify wetness value based on heat
	Wetness = FMath::Clamp(Wetness, 0.0f, 1.0f);
	
	// Float values are used to drive various material effects
	IceMaterial->SetScalarParameterValue(FName(TEXT("Icicles")), Icicles);
	IceMaterial->SetScalarParameterValue(FName(TEXT("Wetness")), Wetness);
	IceMaterial->SetScalarParameterValue(FName(TEXT("Wetness")), Wetness);
	
	Drips->SetActive(Wetness > 0.2f); // Turn on a dripping effect if wetness is above a certain value
	const FVector NewRelativeScale = FMath::Lerp(FVector(1.0f, 1.0f, 1.0f), FVector(0.2f, 0.2f, 0.35f), Melted);
	SetActorRelativeScale3D(NewRelativeScale); // Decrease actor scale as melted value increases
	if (Melted >= 1.0f) // Destroy actor when it has become fully melted
	{
		Destroy();
	}
}

/*
 * Sequence for being hit with projectiles
 */
void ACEIceSphere::ElementalDamage(float HeatDamage, float WaterDamage)
{
	if (Heat > 0.1f) // Applying heat?
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ParticleSystems[ECEIceSphereParticleSystem::SteamPuff], GetActorLocation()); // Spawn a steam puff effect
	}
	Heat = FMath::Clamp(Heat + HeatDamage * 1.5f, 0.0f, 1.5f); // Increase incoming heat value
	Wetness = FMath::Clamp(Wetness + WaterDamage, 0.0f, 1.0f);
}

void ACEIceSphere::InitializeAssets()
{
	InitializeMaterials();
	InitializeParticleSystems();
	InitializeStaticMeshes();
}

void ACEIceSphere::InitializeMaterials()
{
	// M_Ice_Interactive
	ConstructorHelpers::FObjectFinder<UMaterialInterface> IceInteractiveAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Materials/M_Ice_Interactive.M_Ice_Interactive"));
	check(IceInteractiveAsset.Object);
	Materials.Emplace(ECEIceSphereMaterial::IceInteractive, IceInteractiveAsset.Object);
}

void ACEIceSphere::InitializeParticleSystems()
{
	// P_Ice_Steam
	ConstructorHelpers::FObjectFinder<UParticleSystem> IceSteamAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Effects/P_Ice_Steam.P_Ice_Steam"));
	check(IceSteamAsset.Object);
	ParticleSystems.Emplace(ECEIceSphereParticleSystem::IceSteam, IceSteamAsset.Object);

	// P_Steam_Puff
	ConstructorHelpers::FObjectFinder<UParticleSystem> SteamPuffAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Effects/P_Steam_Puff.P_Steam_Puff"));
	check(SteamPuffAsset.Object);
	ParticleSystems.Emplace(ECEIceSphereParticleSystem::SteamPuff, SteamPuffAsset.Object);

	// P_Water_Drips
	ConstructorHelpers::FObjectFinder<UParticleSystem> WaterDripsAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Effects/P_Water_Drips.P_Water_Drips"));
	check(WaterDripsAsset.Object);
	ParticleSystems.Emplace(ECEIceSphereParticleSystem::WaterDrips, WaterDripsAsset.Object);	
}

void ACEIceSphere::InitializeStaticMeshes()
{
	// EditorSphere
	ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("/Engine/EditorMeshes/EditorSphere.EditorSphere"));
	check(SphereAsset.Object);
	StaticMeshes.Emplace(ECEIceSphereStaticMesh::Sphere, SphereAsset.Object);
}
