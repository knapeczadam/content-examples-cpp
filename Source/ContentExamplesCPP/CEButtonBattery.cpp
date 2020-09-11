#include "CEButtonBattery.h"
#include "CEBattery.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ACEButtonBattery::ACEButtonBattery()
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
    ButtonMesh->SetStaticMesh(StaticMeshes[ECEButtonBatteryStaticMesh::Button]);
    ButtonMesh->GetBodyInstance()->bAutoWeld = false;
    ButtonMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACEButtonBattery::BeginPlay()
{
    Super::BeginPlay();
    check(TargetBattery != nullptr);
    
    ButtonLightMaterial = ButtonMesh->CreateDynamicMaterialInstance(2); // Save a reference to the Dynamic Material Instance for use in the event graph

    TriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &ACEButtonBattery::OnTriggerBeginOverlap);
    TriggerCollision->OnComponentEndOverlap.AddDynamic(this, &ACEButtonBattery::OnTriggerEndOverlap);
}

/**
 * Overlap event fires when a pawn touches the trigger
 */
void ACEButtonBattery::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this && OtherComp)
    {
        ButtonLightMaterial->SetScalarParameterValue(FName(TEXT("State")), 1.0f); // Change a scalar value in the button material that turns it green
        TargetBattery->Move(true); // Call a custom event in the target C++ that moves it up
    }
}

/*
 * Overlap event fires when a pawn exits the trigger
 */
void ACEButtonBattery::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor != this && OtherComp)
    {
        ButtonLightMaterial->SetScalarParameterValue(FName(TEXT("State")), 0.0f); // Change a scalar value in the button material that turns it back to red
        TargetBattery->Move(false); // Reverse movement
    }
}

void ACEButtonBattery::InitializeAssets()
{
    InitializeStaticMeshes();
}

void ACEButtonBattery::InitializeStaticMeshes()
{
    // SM_Button
    ConstructorHelpers::FObjectFinder<UStaticMesh> ButtonAsset(TEXT("/Game/DemoRoom/Meshes/SM_Button.SM_Button"));
    check(ButtonAsset.Object);
    StaticMeshes.Emplace(ECEButtonBatteryStaticMesh::Button, ButtonAsset.Object);
}
