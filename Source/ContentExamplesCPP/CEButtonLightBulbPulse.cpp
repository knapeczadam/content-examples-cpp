#include "CEButtonLightBulbPulse.h"
#include "CELightBulbPulse.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACEButtonLightBulbPulse::ACEButtonLightBulbPulse()
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
	ButtonMesh->SetStaticMesh(StaticMeshes[ECEButtonLightBulbPulseStaticMesh::Button]);
	ButtonMesh->GetBodyInstance()->bAutoWeld = false;
	ButtonMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACEButtonLightBulbPulse::BeginPlay()
{
	Super::BeginPlay();

	ButtonLightMaterial = ButtonMesh->CreateDynamicMaterialInstance(2); // Create as set the Dynamic Material Instance for the button

	TriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &ACEButtonLightBulbPulse::OnTriggerBeginOverlap);
	TriggerCollision->OnComponentEndOverlap.AddDynamic(this, &ACEButtonLightBulbPulse::OnTriggerEndOverlap);
}

/*
 * Overlap event fires when a pawn touches the trigger
 */
void ACEButtonLightBulbPulse::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		ButtonLightMaterial->SetScalarParameterValue(FName("State"), 1.0f); // Change a scalar value in the button material that turns it green

		// Get an array of all actors in the level that match the specified class
		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(this, ACELightBulbPulse::StaticClass(), AllActors);
		for (AActor* Actor : AllActors) // Execute for each item in the array
		{
			ACELightBulbPulse* Bulb = CastChecked<ACELightBulbPulse>(Actor); // Cast to the light bulb pulse class in order to access its functions
			Bulb->ToggleLight(true); // Call a custom event in the target to turn its light on
		}
	}
}

/*
 * Overlap event fires when a pawn exits the trigger
 */
void ACEButtonLightBulbPulse::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		ButtonLightMaterial->SetScalarParameterValue(FName("State"), 0.0f); // Change a scalar value in the button material that turns it back to red

		// Get an array of all actors in the level that match the specified class
		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(this, ACELightBulbPulse::StaticClass(), AllActors);
		for (AActor* Actor : AllActors)
		{
			ACELightBulbPulse* Bulb = CastChecked<ACELightBulbPulse>(Actor);
			Bulb->ToggleLight(false); // Turn the light off
		}
	}
}

void ACEButtonLightBulbPulse::InitializeAssets()
{
	InitializeStaticMeshes();
}

void ACEButtonLightBulbPulse::InitializeStaticMeshes()
{
	// SM_Button
	ConstructorHelpers::FObjectFinder<UStaticMesh> ButtonAsset(TEXT("/Game/DemoRoom/Meshes/SM_Button.SM_Button"));
	check(ButtonAsset.Object);
	StaticMeshes.Emplace(ECEButtonLightBulbPulseStaticMesh::Button, ButtonAsset.Object);
}
