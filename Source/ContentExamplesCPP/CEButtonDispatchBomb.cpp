#include "CEButtonDispatchBomb.h"
#include "CEBomb.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ACEButtonDispatchBomb::ACEButtonDispatchBomb()
{
	InitializeAssets();
	
	Base = CreateDefaultSubobject<USceneComponent>(TEXT("Base"));
	RootComponent = Base;

	TriggerCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger collision"));
	TriggerCollision->SetRelativeLocation(FVector(0.0f, 75.0f, 100.0f));
	TriggerCollision->InitCapsuleSize(50.0f, 100.0f);
	TriggerCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerCollision->SetupAttachment(RootComponent);

    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	ButtonMesh->SetStaticMesh(StaticMeshes[ECEButtonDispatchBombStaticMesh::Button]);
	ButtonMesh->GetBodyInstance()->bAutoWeld = false;
	ButtonMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACEButtonDispatchBomb::BeginPlay()
{
	Super::BeginPlay();
	ButtonLightMaterial = ButtonMesh->CreateDynamicMaterialInstance(2);

	TriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &ACEButtonDispatchBomb::OnTriggerBeginOverlap);
	TriggerCollision->OnComponentEndOverlap.AddDynamic(this, &ACEButtonDispatchBomb::OnTriggerEndOverlap);
}

/*
 * Custom event fires if the player is overlapping the button while it's enabled
 */
void ACEButtonDispatchBomb::Activate()
{
	bIsButtonEnabled = false; // Disable the button
	ButtonLightMaterial->SetScalarParameterValue(FName(TEXT("State")), 1.0f); // Change a scalar value in the button material that turns it green
	const FTransform SpawnTransfrom = FTransform(GetActorLocation() + SpawnPoint);
	ACEBomb* SpawnedBomb = GetWorld()->SpawnActor<ACEBomb>(ACEBomb::StaticClass(), SpawnTransfrom);
	SpawnedBomb->OnBombExploded().AddUFunction(this, FName(TEXT("BombExplodedCallback"))); // Bind a custom event to the bomb's Dispatch Event function
}

/*
 * Overlap event fires when a pawn touches the trigger
 */
void ACEButtonDispatchBomb::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		bIsOverlappingButton = true;
		if (bIsButtonEnabled)
		{
			Activate(); // Call a custom event for activating the button sequence
		}
	}
}

/*
 * Event fires when a pawn exits the trigger
 */
void ACEButtonDispatchBomb::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		bIsOverlappingButton = false;
	}
}

void ACEButtonDispatchBomb::BombExplodedCallback()
{
	GetWorldTimerManager().SetTimer(BombExplodedHandler, [this]()
    {
        ButtonLightMaterial->SetScalarParameterValue(FName(TEXT("State")), 0.0f); // Change a scalar value in the button material that turns it back to red
        bIsButtonEnabled = true; // Re-enable the button
        GetWorldTimerManager().SetTimer(BombExplodedHandler, [this]()
        {
            if (bIsOverlappingButton)
            {
                Activate(); // If player is still overlapping button, activate it again
            }
        }, 0.1f, false);
    }, 1.0f, false);
}

void ACEButtonDispatchBomb::InitializeAssets()
{
	InitializeStaticMeshes();
}

void ACEButtonDispatchBomb::InitializeStaticMeshes()
{
	// SM_Button
	ConstructorHelpers::FObjectFinder<UStaticMesh> ButtonAsset(TEXT("/Game/DemoRoom/Meshes/SM_Button.SM_Button"));
	check(ButtonAsset.Object);
	StaticMeshes.Emplace(ECEButtonDispatchBombStaticMesh::Button, ButtonAsset.Object);
}
