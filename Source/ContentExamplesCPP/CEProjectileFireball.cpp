#include "CEProjectileFireball.h"
#include "CEElementalProjectiles.h"
#include "Components/SphereComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavAreas/NavArea_Obstacle.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ACEProjectileFireball::ACEProjectileFireball()
{
	InitializeAssets();
	ScaleCurve = Curves[ECEProjectileFireballCurve::Scale];
	
    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(20.0f);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Collision->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	Collision->AreaClass = UNavArea_Obstacle::StaticClass();
	RootComponent = Collision;

    Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));
	Effect->Template = ParticleSystems[ECEProjectileFireballParticleSystem::Fireball];
	Effect->SetupAttachment(RootComponent);

    LifeSpanTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("LifeSpan"));

    ProjectileMovement1 = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement1"));
	ProjectileMovement1->InitialSpeed = 500.0f;
	ProjectileMovement1->bRotationFollowsVelocity = true;
}

// Called when the game starts or when spawned
void ACEProjectileFireball::BeginPlay()
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

void ACEProjectileFireball::UpdateTimelineCallback()
{
	const float PlaybackPosition = LifeSpanTimeline->GetPlaybackPosition();
	const float Scale = ScaleCurve->GetFloatValue(PlaybackPosition);
	SetActorRelativeScale3D(FVector::OneVector * Scale); // Gradually shrink the actor
}

void ACEProjectileFireball::FinishedTimelineCallback()
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
void ACEProjectileFireball::NotifyActorBeginOverlap(AActor* OtherActor)
{
	ICEElementalProjectiles* AnotherProjectile = Cast<ICEElementalProjectiles>(OtherActor);
	if (AnotherProjectile)
	{
		AnotherProjectile->ElementalDamage(0.4f, -0.5f); // Call this interface function on the target C++, if it implements it
	}
	UGameplayStatics::SpawnEmitterAtLocation(this, ParticleSystems[ECEProjectileFireballParticleSystem::Explosion], GetActorLocation());
	Destroy();
}

/*
 * Event fires when the projectile hits a static, blocking actor
 */
void ACEProjectileFireball::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystems[ECEProjectileFireballParticleSystem::Explosion], GetActorLocation()); // Spawn an impact effect
	Destroy();
}

void ACEProjectileFireball::InitializeAssets()
{
	InitializeCurves();
	InitializeParticleSystems();
}

void ACEProjectileFireball::InitializeCurves()
{
	// Curve_Scale_ProjectileFireball_Float 
	ConstructorHelpers::FObjectFinder<UCurveFloat> ScaleCurveAsset(TEXT("/Game/ContentExamplesCPP/Core/Curve_Scale_ProjectileFireball_Float.Curve_Scale_ProjectileFireball_Float"));
	check(ScaleCurveAsset.Object);
	Curves.Emplace(ECEProjectileFireballCurve::Scale, ScaleCurveAsset.Object); 
}

void ACEProjectileFireball::InitializeParticleSystems()
{
	// P_Explosion_Fireball
	ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Effects/P_Explosion_Fireball.P_Explosion_Fireball"));
	check(ExplosionAsset.Object);
	ParticleSystems.Emplace(ECEProjectileFireballParticleSystem::Explosion, ExplosionAsset.Object);
	
	// P_Fireball_Projectile
	ConstructorHelpers::FObjectFinder<UParticleSystem> FireballAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Effects/P_Fireball_Projectile.P_Fireball_Projectile"));
	check(FireballAsset.Object);
	ParticleSystems.Emplace(ECEProjectileFireballParticleSystem::Fireball, FireballAsset.Object);
}
