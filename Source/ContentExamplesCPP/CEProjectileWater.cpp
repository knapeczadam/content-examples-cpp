#include "CEProjectileWater.h"
#include "CEElementalProjectiles.h"
#include "Components/SphereComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavAreas/NavArea_Obstacle.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ACEProjectileWater::ACEProjectileWater()
{
	InitializeAssets();
	ScaleCurve = Curves[ECEProjectileWaterCurve::Scale];
	
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(20.0f);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Collision->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	Collision->AreaClass = UNavArea_Obstacle::StaticClass();
	RootComponent = Collision;

	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));
	Effect->Template = ParticleSystems[ECEProjectileWaterParticleSystem::Projectile];
	Effect->SetupAttachment(RootComponent);

	LifeSpanTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("LifeSpan"));

	ProjectileMovement1 = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement1"));
	ProjectileMovement1->InitialSpeed = 500.0f;
	ProjectileMovement1->bRotationFollowsVelocity = true;
}

// Called when the game starts or when spawned
void ACEProjectileWater::BeginPlay()
{
	Super::BeginPlay();
	FOnTimelineFloat UpdateTimelineDelegate;
	UpdateTimelineDelegate.BindUFunction(this, TEXT("UpdateTimelineCallback"));
	LifeSpanTimeline->AddInterpFloat(ScaleCurve, UpdateTimelineDelegate);
	
	FOnTimelineEvent FinishedTimelineDelegate;
	FinishedTimelineDelegate.BindUFunction(this, FName(TEXT("FinishedTimelineCallback")));
	LifeSpanTimeline->SetTimelineFinishedFunc(FinishedTimelineDelegate);

	// Set the speed/direction of the projectile
	ProjectileMovement1->Velocity = GetActorRotation().RotateVector(Velocity); // Orient velocity vector in the direction of the actor
	LifeSpanTimeline->Play();
}

void ACEProjectileWater::InitializeAssets()
{
	InitializeCurves();
	InitializeParticleSystems();
}

void ACEProjectileWater::InitializeCurves()
{
	// Curve_Scale_ProjectileWater_Float 
	ConstructorHelpers::FObjectFinder<UCurveFloat> ScaleCurveAsset(TEXT("/Game/ContentExamplesCPP/Core/Curve_Scale_ProjectileWater_Float.Curve_Scale_ProjectileWater_Float"));
	check(ScaleCurveAsset.Object);
	Curves.Emplace(ECEProjectileWaterCurve::Scale, ScaleCurveAsset.Object); 
}

void ACEProjectileWater::InitializeParticleSystems()
{
	// P_Water_Projectile
	ConstructorHelpers::FObjectFinder<UParticleSystem> ProjectileAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Effects/P_Water_Projectile.P_Water_Projectile"));
	check(ProjectileAsset.Object);
	ParticleSystems.Emplace(ECEProjectileWaterParticleSystem::Projectile, ProjectileAsset.Object);
	
	// P_Water_Splash
	ConstructorHelpers::FObjectFinder<UParticleSystem> SplashAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Effects/P_Water_Splash.P_Water_Splash"));
	check(SplashAsset.Object);
	ParticleSystems.Emplace(ECEProjectileWaterParticleSystem::Splash, SplashAsset.Object);
}

void ACEProjectileWater::UpdateTimelineCallback()
{
	const float PlaybackPosition = LifeSpanTimeline->GetPlaybackPosition();
	const float Scale = ScaleCurve->GetFloatValue(PlaybackPosition);
	SetActorRelativeScale3D(FVector::OneVector * Scale); // Gradually shrink the actor
}

void ACEProjectileWater::FinishedTimelineCallback()
{
	Effect->ToggleActive(); // Turn off effect to give particles a chance to fade instead of disappearing suddenly when the actor is destroyed
	GetWorldTimerManager().SetTimer(FinishedHandler, [this]()
    {
        Destroy();
    }, 0.1f, false);
}

/*
 * Event fires when the projectile overlaps a dynamic actor
 */
void ACEProjectileWater::NotifyActorBeginOverlap(AActor* OtherActor)
{
	ICEElementalProjectiles* AnotherProjectile = Cast<ICEElementalProjectiles>(OtherActor);
	if (AnotherProjectile)
	{
		AnotherProjectile->ElementalDamage(-0.5f, 0.5f); // Call this interface function on the target C++, if it implements it
	}
	UGameplayStatics::SpawnEmitterAtLocation(this, ParticleSystems[ECEProjectileWaterParticleSystem::Splash], GetActorLocation()); // Spawn an impact effect
	Destroy();
}

/*
 * Event fires when the projectile hits a static, blocking actor
 */
void ACEProjectileWater::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	UGameplayStatics::SpawnEmitterAtLocation(this, ParticleSystems[ECEProjectileWaterParticleSystem::Splash], GetActorLocation()); // Spawn an impact effect
	Destroy();
}
