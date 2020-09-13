#include "CEBattery.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"

// Sets default values
ACEBattery::ACEBattery()
{
    InitializeAssets();
    MovementCurve = Curves[ECEBatteryCurve::Movement];
    BatteryMesh = StaticMeshes[ECEBatteryStaticMesh::BatteryMedium];

    Base = CreateDefaultSubobject<USceneComponent>(TEXT("Base"));
    SetRootComponent(Base);

    Battery = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Battery"));
    Battery->SetStaticMesh(BatteryMesh);
    Battery->GetBodyInstance()->bAutoWeld = false;
    Battery->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Battery->SetupAttachment(RootComponent);

    ConnectorTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Connector trigger"));
    ConnectorTrigger->SetRelativeLocation(FVector(0.0f, 0.0f, -2.5f));
    ConnectorTrigger->InitBoxExtent(FVector(4.0f, 4.0f, 4.0f));
    ConnectorTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    ConnectorTrigger->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
    ConnectorTrigger->SetupAttachment(Battery);

    MovementTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Movement"));
}

// Called when the game starts or when spawned
void ACEBattery::BeginPlay()
{
    Super::BeginPlay();
    FOnTimelineFloat UpdateTimelineDelegate;
    UpdateTimelineDelegate.BindUFunction(this, FName(TEXT("UpdateTimelineCallback")));
    MovementTimeline->AddInterpFloat(MovementCurve, UpdateTimelineDelegate);
}

void ACEBattery::Move(bool bIsUp)
{
    if (bIsUp)
    {
        MovementTimeline->Play();
    }
    else
    {
        MovementTimeline->Reverse();
    }
}

void ACEBattery::UpdateTimelineCallback()
{
    const float PlaybackPosition = MovementTimeline->GetPlaybackPosition();
    const float Movement = MovementCurve->GetFloatValue(PlaybackPosition);
    const FVector NewLocation = FMath::Lerp(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 25.0f), Movement);
    Battery->SetRelativeLocation(NewLocation);
    const FRotator NewRotation = FMath::Lerp(FRotator(0.0f, 0.0f, 0.0f), FRotator(0.0f, 90.0f, 0.0f), Movement);
    Battery->SetRelativeRotation(NewRotation); // Move battery upwards
}

float ACEBattery::GetElectricity() const
{
    return Electricity;
}

#if WITH_EDITOR
void ACEBattery::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    const FName PropertyName = PropertyChangedEvent.GetPropertyName();
    if (PropertyName == GET_MEMBER_NAME_CHECKED(ACEBattery, BatteryMesh))
    {
        if (BatteryMesh)
        {
            Battery->SetStaticMesh(BatteryMesh);
        }
    }
}
#endif

void ACEBattery::InitializeAssets()
{
    InitializeStaticMeshes();
    InitializeCurves();
}

void ACEBattery::InitializeStaticMeshes()
{
    // SM_Battery_Small
    ConstructorHelpers::FObjectFinder<UStaticMesh> BatterySmallAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Meshes/SM_Battery_Small.SM_Battery_Small"));
    check(BatterySmallAsset.Object);
    StaticMeshes.Emplace(ECEBatteryStaticMesh::BatterySmall, BatterySmallAsset.Object);
    
    // SM_Battery_Medium
    ConstructorHelpers::FObjectFinder<UStaticMesh> BatteryMediumAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Meshes/SM_Battery_Medium.SM_Battery_Medium"));
    check(BatteryMediumAsset.Object);
    StaticMeshes.Emplace(ECEBatteryStaticMesh::BatteryMedium, BatteryMediumAsset.Object);

    // SM_Battery_Big
    ConstructorHelpers::FObjectFinder<UStaticMesh> BatteryBigAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Meshes/SM_Battery_Big.SM_Battery_Big"));
    check(BatteryBigAsset.Object);
    StaticMeshes.Emplace(ECEBatteryStaticMesh::BatteryBig, BatteryBigAsset.Object);
}

void ACEBattery::InitializeCurves()
{
    // Curve_Movement_Battery_Float 
    ConstructorHelpers::FObjectFinder<UCurveFloat> MovementCurveAsset(TEXT("/Game/ContentExamplesCPP/Core/Curve_Movement_Battery_Float.Curve_Movement_Battery_Float"));
    check(MovementCurveAsset.Object);
    Curves.Emplace(ECEBatteryCurve::Movement, MovementCurveAsset.Object); 
}
