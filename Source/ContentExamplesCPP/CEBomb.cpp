#include "CEBomb.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ACEBomb::ACEBomb()
{
	InitializeAssets();
	FuseCurve = Curves[ECEBombCurve::Fuse];
	
	Bomb = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bomb"));
	Bomb->SetStaticMesh(StaticMeshes[ECEBombStaticMesh::Bomb]);
	Bomb->SetSimulatePhysics(true);
	Bomb->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	RootComponent = Bomb;
	
	FuseEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("fuse effect"));
	FuseEffect->SetRelativeLocation(FVector(0.0f, -4.0f, 40.0f));
	FuseEffect->Template = ParticleSystems[ECEBombParticleSystem::Fuse];
	FuseEffect->SetupAttachment(RootComponent);

	FuseTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Fuse"));
}

// Called when the game starts or when spawned
void ACEBomb::BeginPlay()
{
	Super::BeginPlay();
	FuseMaterial = Bomb->CreateDynamicMaterialInstance(1); // Create and set Dynamic Material Instance for Fuse

	FOnTimelineFloat UpdateTimelineDelegate;
	UpdateTimelineDelegate.BindUFunction(this, FName(TEXT("UpdateTimelineCallback")));
	FuseTimeline->AddInterpFloat(FuseCurve, UpdateTimelineDelegate);

	FOnTimelineEvent FinishedTimelineDelegate;
	FinishedTimelineDelegate.BindUFunction(this, FName(TEXT("FinishedTimelineCallback")));
	FuseTimeline->SetTimelineFinishedFunc(FinishedTimelineDelegate);

	const FVector NewAngularVelocity = FMath::VRand() * 5.0f;
	Bomb->SetPhysicsAngularVelocityInDegrees(NewAngularVelocity); // Start spinning
	FuseTimeline->PlayFromStart();
}

/*
 * Animate Fuse
 */
void ACEBomb::UpdateTimelineCallback()
{
	const float PlaybackPosition = FuseTimeline->GetPlaybackPosition();
	const float Fuse = FuseCurve->GetFloatValue(PlaybackPosition);

	FuseMaterial->SetScalarParameterValue(FName(TEXT("Burnt")), Fuse); // Update the fuse material to make it look like it's burning down
	const FVector NewLocation = FMath::Lerp(FVector(0.0f, -4.0f, 40.0f), FVector(0.0f, 0.0f, 27.0f), Fuse);
	FuseEffect->SetRelativeLocation(NewLocation); // Move the fuse sparks along the fuse mesh
}

/*
 * Explode
 */
void ACEBomb::FinishedTimelineCallback()
{
	FuseEffect->SetActive(false, false); // Turn off fuse
	GetWorldTimerManager().SetTimer(FinishedHandler, [this]()
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ParticleSystems[ECEBombParticleSystem::Explosion], GetActorLocation()); // Spawn explosion effect
		BroadcastBombExploded(); // Call the Event Dispatcher function
		Destroy();
	}, 0.15f, false);
}

void ACEBomb::InitializeAssets()
{
	InitializeStaticMeshes();
	InitializeCurves();
	InitializeParticleSystems();
}

void ACEBomb::InitializeStaticMeshes()
{
	// SM_Bomb
	ConstructorHelpers::FObjectFinder<UStaticMesh> BombAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Meshes/SM_Bomb.SM_Bomb"));
	check(BombAsset.Object);
	StaticMeshes.Emplace(ECEBombStaticMesh::Bomb, BombAsset.Object);
}

void ACEBomb::InitializeCurves()
{
	// Curve_Fuse_Bomb_Float
	static ConstructorHelpers::FObjectFinder<UCurveFloat> FuseAsset(TEXT("/Game/ContentExamplesCPP/Core/Curve_Fuse_Bomb_Float.Curve_Fuse_Bomb_Float"));
	check(FuseAsset.Object); 	
	Curves.Emplace(ECEBombCurve::Fuse, FuseAsset.Object);
}


void ACEBomb::InitializeParticleSystems()
{
	// P_Explosion_Bomb
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Effects/P_Explosion_Bomb.P_Explosion_Bomb"));
	check(ExplosionAsset.Object);
	ParticleSystems.Emplace(ECEBombParticleSystem::Explosion, ExplosionAsset.Object);

	// P_Bomb_Fuse
	static ConstructorHelpers::FObjectFinder<UParticleSystem> FuseAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Effects/P_Bomb_Fuse.P_Bomb_Fuse"));
	check(FuseAsset.Object);
	ParticleSystems.Emplace(ECEBombParticleSystem::Fuse, FuseAsset.Object);
}
