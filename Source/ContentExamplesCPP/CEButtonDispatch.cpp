#include "CEButtonDispatch.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ACEButtonDispatch::ACEButtonDispatch()
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
	ButtonMesh->SetStaticMesh(StaticMeshes[ECEButtonDispatchStaticMesh::Button]);
	ButtonMesh->GetBodyInstance()->bAutoWeld = false;
	ButtonMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACEButtonDispatch::BeginPlay()
{
	Super::BeginPlay();

	ButtonLightMaterial = ButtonMesh->CreateDynamicMaterialInstance(2); // Save a reference to the Dynamic Material Instance for use in the event graph

	TriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &ACEButtonDispatch::OnTriggerBeginOverlap);
	TriggerCollision->OnComponentEndOverlap.AddDynamic(this, &ACEButtonDispatch::OnTriggerEndOverlap);
}

/*
 * Overlap event fires when a pawn touches the trigger
 */
void ACEButtonDispatch::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		ButtonLightMaterial->SetScalarParameterValue(FName("State"), 1.0f); // Change a scalar value in the button material that turns it green
		BroadcastButtonToggled(true);
	}
}

/*
 * Overlap event fires when a pawn exits the trigger
 */
void ACEButtonDispatch::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		ButtonLightMaterial->SetScalarParameterValue(FName("State"), 0.0f); // Change a scalar value in the button material that turns it back to red
		BroadcastButtonToggled(false);
	}
}

void ACEButtonDispatch::InitializeAssets()
{
	InitializeStaticMeshes();
}

void ACEButtonDispatch::InitializeStaticMeshes()
{
	// SM_Button
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ButtonAsset(TEXT("/Game/DemoRoom/Meshes/SM_Button.SM_Button"));
	check(ButtonAsset.Object);
	StaticMeshes.Emplace(ECEButtonDispatchStaticMesh::Button, ButtonAsset.Object);
}
