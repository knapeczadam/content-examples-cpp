#include "CEInteractiveSwitch.h"
#include "CELEDBasic.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACEInteractiveSwitch::ACEInteractiveSwitch()
{
    InitializeAssets();
    PitchCurve = Curves[ECEInteractiveSwitchCurve::Pitch];
    
    SwitchBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Switch base"));
    SwitchBase->SetStaticMesh(StaticMeshes[ECEInteractiveSwitchStaticMesh::Switch]);
    SwitchBase->GetBodyInstance()->bAutoWeld = false;
    RootComponent = SwitchBase;

    SwitchHandle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Switch handle"));
    SwitchHandle->SetStaticMesh(StaticMeshes[ECEInteractiveSwitchStaticMesh::Handle]);
    SwitchHandle->SetRelativeLocation(FVector(2.5f, 0.0f, 0.0f));
    SwitchHandle->SetRelativeRotation(FRotator(15.0f, 0.0f, 0.0f));
    SwitchHandle->GetBodyInstance()->bAutoWeld = false;
    SwitchHandle->SetupAttachment(RootComponent);

    SwitchHandleHighlighted = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Switch handle highlighted"));
    SwitchHandleHighlighted->SetStaticMesh(StaticMeshes[ECEInteractiveSwitchStaticMesh::Handle]);
    SwitchHandleHighlighted->SetMaterial(0, Materials[ECEInteractiveSwitchMaterial::OrangeShiny]);
    SwitchHandleHighlighted->GetBodyInstance()->bAutoWeld = false;
    SwitchHandleHighlighted->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    SwitchHandleHighlighted->SetVisibleFlag(false);
    SwitchHandleHighlighted->bRenderCustomDepth = true;
    SwitchHandleHighlighted->SetupAttachment(SwitchHandle);

    InteractTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Interact trigger"));
    InteractTrigger->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));
    InteractTrigger->InitBoxExtent(FVector(30.0f, 30.0f, 30.0f));
    InteractTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    InteractTrigger->SetupAttachment(RootComponent);

    TextRender1 = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRender1"));
    TextRender1->SetRelativeLocation(FVector(10.0f, 0.0f, -30.0f));
    TextRender1->Text = FText::FromString(TEXT("Click and drag to interact"));
    TextRender1->SetMaterial(0, Materials[ECEInteractiveSwitchMaterial::OpaqueEmissive]);
    TextRender1->HorizontalAlignment = EHTA_Center;
    TextRender1->XScale = 0.25f;
    TextRender1->YScale = 0.25f;
    TextRender1->SetVisibleFlag(false);
    TextRender1->SetupAttachment(RootComponent);

    RotateSwitchTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Rotate switch"));
}

// Called when the game starts or when spawneFStreamingManagerd
void ACEInteractiveSwitch::BeginPlay()
{
    Super::BeginPlay();
    check(TargetLED);
    
    InteractTrigger->OnComponentBeginOverlap.AddDynamic(this, &ACEInteractiveSwitch::OnTriggerBeginOverlap);
    InteractTrigger->OnComponentEndOverlap.AddDynamic(this, &ACEInteractiveSwitch::OnTriggerEndOverlap);

    FOnTimelineFloat UpdateTimelineDelegate;
    UpdateTimelineDelegate.BindUFunction(this, FName(TEXT("UpdateTimelineCallback")));
    RotateSwitchTimeline->AddInterpFloat(PitchCurve, UpdateTimelineDelegate);
    
    FOnTimelineEvent FinishedTimelineDelegate;
    FinishedTimelineDelegate.BindUFunction(this, FName(TEXT("FinishedTimelineCallback")));
    RotateSwitchTimeline->SetTimelineFinishedFunc(FinishedTimelineDelegate);
}

/*
 * Animate the Switch
 * Custom event called inside the "Grab interact" Interface function
 */
void ACEInteractiveSwitch::FlipSwitch()
{
    if (bSwitchPosition)
    {
        RotateSwitchTimeline->Reverse();
        TimelineDirection = ETimelineDirection::Backward;
    }
    else
    {
        RotateSwitchTimeline->Play();
        TimelineDirection = ETimelineDirection::Forward;
    }
}

/*
 * Set Visibilty of Components
 * Overlap event fires when a pawn touches the trigger
 */
void ACEInteractiveSwitch::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this && OtherComp)
    {
        // Swap visibility to show/hide a highlighted version of the mesh
        SwitchHandle->ToggleVisibility();
        SwitchHandleHighlighted->ToggleVisibility();
        TextRender1->ToggleVisibility();
    }
}

/*
 * Set Visibilty of Components
 * Overlap event fires when a pawn exits the trigger
 */
void ACEInteractiveSwitch::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor != this && OtherComp)
    {
        // Swap visibility to show/hide a highlighted version of the mesh
        SwitchHandle->ToggleVisibility();
        SwitchHandleHighlighted->ToggleVisibility();
        TextRender1->ToggleVisibility();
    }
}

void ACEInteractiveSwitch::UpdateTimelineCallback()
{
    const float PlaybackPosition = RotateSwitchTimeline->GetPlaybackPosition();
    const float Pitch = PitchCurve->GetFloatValue(PlaybackPosition);
    FRotator NewRotation = FRotator(Pitch, 0.0f, 0.0f);
    SwitchHandle->SetRelativeRotation(NewRotation); // Move the switch handle into the up/down position
}

/*
 * When animation is finished it toggles the Light on or off depending of the direction of the animation
 */
void ACEInteractiveSwitch::FinishedTimelineCallback()
{
    bSwitchPosition = TimelineDirection == ETimelineDirection::Forward; // Is the timeline playing or reversing?
    TargetLED->ToggleLight(bSwitchPosition); // Call a custom event in the target C++ to turn its light on/off
}

FVector ACEInteractiveSwitch::GrabInteract(FVector2D PlayerAimMovement, bool bIsInteracting)
{
    if ((PlayerAimMovement.Y >= 0.5f && !bSwitchPosition) || (PlayerAimMovement.Y <= -0.5f && bSwitchPosition))
    {
        FlipSwitch();
    }
    return SwitchHandle->GetSocketLocation(FName(TEXT("Focus target")));
}

void ACEInteractiveSwitch::InitializeAssets()
{
    InitializeStaticMeshes();
    InitializeCurves();
    InitializeMaterials();
}

void ACEInteractiveSwitch::InitializeStaticMeshes()
{
    // SM_Interactive_Switch_Handle
    static ConstructorHelpers::FObjectFinder<UStaticMesh> HandleAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Meshes/SM_Interactive_Switch_Handle.SM_Interactive_Switch_Handle"));
    check(HandleAsset.Object);
    StaticMeshes.Emplace(ECEInteractiveSwitchStaticMesh::Handle, HandleAsset.Object);

    // SM_Interactive_Switch
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SwitchHandleAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Meshes/SM_Interactive_Switch.SM_Interactive_Switch"));
    check(SwitchHandleAsset.Object);
    StaticMeshes.Emplace(ECEInteractiveSwitchStaticMesh::Switch, SwitchHandleAsset.Object);
}

void ACEInteractiveSwitch::InitializeCurves()
{
    // Curve_Pitch_InteractiveSwitch_Float
    static ConstructorHelpers::FObjectFinder<UCurveFloat> PitchAsset(TEXT("/Game/ContentExamplesCPP/Core/Curve_Pitch_InteractiveSwitch_Float.Curve_Pitch_InteractiveSwitch_Float"));
    check(PitchAsset.Object);
    Curves.Emplace(ECEInteractiveSwitchCurve::Pitch, PitchAsset.Object);
}

void ACEInteractiveSwitch::InitializeMaterials()
{
    // M_TextMaterial_Opaque_Emissive
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> OpaqueEmissiveAsset(TEXT("/Game/Assets/Materials/M_TextMaterial_Opaque_Emissive.M_TextMaterial_Opaque_Emissive"));
    check(OpaqueEmissiveAsset.Object);
    Materials.Emplace(ECEInteractiveSwitchMaterial::OpaqueEmissive, OpaqueEmissiveAsset.Object);

    // M_Orange_Shiny_Highlighted
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> OrangeShinyAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Materials/M_Orange_Shiny_Highlighted.M_Orange_Shiny_Highlighted"));
    check(OrangeShinyAsset.Object);
    Materials.Emplace(ECEInteractiveSwitchMaterial::OrangeShiny, OrangeShinyAsset.Object);
}
