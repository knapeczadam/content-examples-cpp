#include "CEButtonLightBulbBasic.h"
#include "CELightBulbBasic.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ACEButtonLightBulbBasic::ACEButtonLightBulbBasic()
{
    InitializeAssets();
    
    Base = CreateDefaultSubobject<USceneComponent>(TEXT("Base"));
    RootComponent = Base;
    
    TriggerCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger collision"));
    TriggerCollision->SetRelativeLocation(FVector(0.0f,75.0f,100.0f));
    TriggerCollision->InitCapsuleSize(50.0f, 100.0f);
    TriggerCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    TriggerCollision->SetupAttachment(RootComponent);
    
    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    ButtonMesh->SetStaticMesh(StaticMeshes[ECEButtonLightBulbBasicStaticMesh::Button]);
    ButtonMesh->GetBodyInstance()->bAutoWeld = false;
    ButtonMesh->SetupAttachment(RootComponent);

    AutoReceiveInput = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ACEButtonLightBulbBasic::BeginPlay()
{
    Super::BeginPlay();
    check(TargetLight != nullptr);
    
    ButtonLightMaterial = ButtonMesh->CreateDynamicMaterialInstance(2); // Save a reference to the Dynamic Material Instance for use in the event graph

    TriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &ACEButtonLightBulbBasic::OnTriggerBeginOverlap);
    TriggerCollision->OnComponentEndOverlap.AddDynamic(this, &ACEButtonLightBulbBasic::OnTriggerEndOverlap);
}

/**
 * Overlap event fires when a pawn touches the trigger
 */
void ACEButtonLightBulbBasic::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this && OtherComp)
    {
        ButtonLightMaterial->SetScalarParameterValue(FName(TEXT("State")), 1.0f); // Change a scalar value in the button material that turns it green
        TargetLight->ToggleLight(true); // Call a custom event in the target C++ to turn its light on
    }
}

/**
 * Overlap event fires when a pawn exits the trigger
 */
void ACEButtonLightBulbBasic::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor != this && OtherComp)
    {
        ButtonLightMaterial->SetScalarParameterValue(FName(TEXT("State")), 0.0f); // Change a scalar value in the button material that turns it back to red
        TargetLight->ToggleLight(false); // Turn the light off
    }
}

void ACEButtonLightBulbBasic::InitializeAssets()
{
    InitializeStaticMeshes();
}

void ACEButtonLightBulbBasic::InitializeStaticMeshes()
{
    // SM_Button
    ConstructorHelpers::FObjectFinder<UStaticMesh> ButtonAsset(TEXT("/Game/DemoRoom/Meshes/SM_Button.SM_Button"));
    check(ButtonAsset.Object);
    StaticMeshes.Emplace(ECEButtonLightBulbBasicStaticMesh::Button, ButtonAsset.Object);
}
