#include "CEButtonInterface.h"
#include "CEPlayerInteractions.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ACEButtonInterface::ACEButtonInterface()
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
    ButtonMesh->SetStaticMesh(StaticMeshes[ECEButtonInterfaceStaticMesh::Button]);
    ButtonMesh->GetBodyInstance()->bAutoWeld = false;
    ButtonMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACEButtonInterface::BeginPlay()
{
    Super::BeginPlay();

    ButtonLightMaterial = ButtonMesh->CreateDynamicMaterialInstance(2); // Save a reference to the Dynamic Material Instance for use in the event graph

    TriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &ACEButtonInterface::OnTriggerBeginOverlap);
    TriggerCollision->OnComponentEndOverlap.AddDynamic(this, &ACEButtonInterface::OnTriggerEndOverlap);
}

/*
 * Overlap event fires when a pawn touches the trigger
 */
void ACEButtonInterface::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this && OtherComp)
    {
        ButtonLightMaterial->SetScalarParameterValue(FName(TEXT("State")), 1.0f); // Change a scalar value in the button material that turns it green
        for (AActor* Target : Targets)
        {
            ICEPlayerInteractions* ITarget = Cast<ICEPlayerInteractions>(Target);
            if (ITarget)
            {
                // The function will be called for every item in the array
                ITarget->PushedButton(); // This Interface Message function will fire a corresponding event if it's present in the target C++s
            }
        }
    }
}

/*
 * Overlap event fires when a pawn exits the trigger
 */
void ACEButtonInterface::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor != this && OtherComp)
    {
        ButtonLightMaterial->SetScalarParameterValue(FName(TEXT("State")), 0.0f); // Change a scalar value in the button material that turns it back to red
        for (AActor* Target : Targets)
        {
            ICEPlayerInteractions* ITarget = Cast<ICEPlayerInteractions>(Target);
            if (ITarget)
            {
                // The function will be called for every item in the array
                ITarget->PushedButton(); // This Interface Message function will fire a corresponding event if it's present in the target C++s
            }
        }
    }
}

void ACEButtonInterface::InitializeAssets()
{
    InitializeStaticMeshes();
}

void ACEButtonInterface::InitializeStaticMeshes()
{
    // SM_Button
    ConstructorHelpers::FObjectFinder<UStaticMesh> ButtonAsset(TEXT("/Game/DemoRoom/Meshes/SM_Button.SM_Button"));
    check(ButtonAsset.Object);
    StaticMeshes.Emplace(ECEButtonInterfaceStaticMesh::Button, ButtonAsset.Object);
}
