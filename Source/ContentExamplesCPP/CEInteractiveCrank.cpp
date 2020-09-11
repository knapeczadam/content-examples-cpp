#include "CEInteractiveCrank.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACEInteractiveCrank::ACEInteractiveCrank()
{
	InitializeAssets();
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WheelBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wheel base"));
	WheelBase->SetStaticMesh(StaticMeshes[ECEInteractiveCrankStaticMesh::Crank]);
	WheelBase->GetBodyInstance()->bAutoWeld = false;
	RootComponent = WheelBase;

	Wheel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wheel"));
	Wheel->SetStaticMesh(StaticMeshes[ECEInteractiveCrankStaticMesh::Wheel]);
	Wheel->SetRelativeLocation(FVector(12.5f, 0.0f, 0.0f));
	Wheel->GetBodyInstance()->bAutoWeld = false;
	Wheel->SetupAttachment(RootComponent);

	WheelHighlighted = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wheel highlighted"));
	WheelHighlighted->SetStaticMesh(StaticMeshes[ECEInteractiveCrankStaticMesh::Wheel]);
	WheelHighlighted->SetMaterial(0, Materials[ECEInteractiveCrankMaterial::OrangeShiny]);
	WheelHighlighted->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	WheelHighlighted->SetVisibleFlag(false);
	WheelHighlighted->bRenderCustomDepth = true;
	WheelHighlighted->SetupAttachment(Wheel);

	InteractTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Interact trigger"));
	InteractTrigger->SetRelativeLocation(FVector(60.0f, 0.0f, 0.0f));
	InteractTrigger->InitBoxExtent(FVector(30.0f, 30.0f, 30.0f));
	InteractTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractTrigger->SetupAttachment(RootComponent);

	TextRender1 = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRender1"));
	TextRender1->SetRelativeLocation(FVector(30.0f, 0.0f, -30.0f));
	TextRender1->SetMaterial(0, Materials[ECEInteractiveCrankMaterial::OpaqueEmissive]);
	TextRender1->Text = FText::FromString(TEXT("Click and drag to interact"));
	TextRender1->HorizontalAlignment = EHTA_Center;
	TextRender1->XScale = 0.25f;
	TextRender1->YScale = 0.25f;
	TextRender1->SetVisibleFlag(false);
	TextRender1->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACEInteractiveCrank::BeginPlay()
{
	Super::BeginPlay();
	check(TargetLED);
	
	InteractTrigger->OnComponentBeginOverlap.AddDynamic(this, &ACEInteractiveCrank::OnTriggerBeginOverlap);
	InteractTrigger->OnComponentEndOverlap.AddDynamic(this, &ACEInteractiveCrank::OnTriggerEndOverlap);
}

// Called every frame
void ACEInteractiveCrank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Rotate Crank
	// Turn the wheel's relative rotation into a direction vector
	const FVector RightVector = UKismetMathLibrary::GetRightVector(Wheel->GetRelativeRotation());
	// Grab direction is updated inside the "Grab interact" Interface function
	// Resulting value increases if the two directions are aligned
	const float TargetRotation = FVector::DotProduct(PlayerDragInteraction, RightVector) * -10.0f;
	// Smoothly increase/decrease a float representing how much the wheel will turn
	Rotation = FMath::FInterpTo(Rotation, TargetRotation, DeltaTime, 4.0f);
	Wheel->AddLocalRotation(FRotator(0.0f, 0.0f, Rotation));
	
	// Set Light Brightness depending on Rotation Value
	// Call a custom event in the target to update its light's brightness
	TargetLED->ChangeLightBrightness(FMath::Abs(Rotation)); // Absolute value is effectively the speed at which the wheel is being turned, in any direction
}

/*
 * Check interaction
 * Interface function to be called by other C++s
 */
FVector ACEInteractiveCrank::GrabInteract(FVector2D PlayerAimMovement, bool bIsInteracting)
{
	if (bIsInteracting)
	{
		// Set Drag Direction for Crank
		PlayerDragInteraction = FVector(0.0f, PlayerAimMovement.X, PlayerAimMovement.Y);
		PlayerDragInteraction.Normalize(0.0001f); // Update a vector representing the direction the player is dragging in. This vector is used in the event graph
	}
	else
	{
		PlayerDragInteraction = FVector::ZeroVector; // Reset Drag Direction to zero	
	}
	// Return Focus target for Player
	return Wheel->GetSocketLocation(FName(TEXT("Focus target"))); // Return the location of the wheel handle
}

/*
 * Set Visibilty of Components
 * Overlap event fires when a pawn touches the trigger
 */
void ACEInteractiveCrank::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		// Swap visibility to show/hide a highlighted version of the mesh
		Wheel->ToggleVisibility();
		WheelHighlighted->ToggleVisibility();
		TextRender1->ToggleVisibility();
	}
}

/*
 * Set Visibilty of Components
 * Overlap event fires when a pawn exits the trigger
 */
void ACEInteractiveCrank::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		// Swap visibility to show/hide a highlighted version of the mesh
		Wheel->ToggleVisibility();
		WheelHighlighted->ToggleVisibility();
		TextRender1->ToggleVisibility();	
	}
}

void ACEInteractiveCrank::InitializeAssets()
{
	InitializeStaticMeshes();
	InitializeMaterials();
}

void ACEInteractiveCrank::InitializeStaticMeshes()
{
	// SM_Interactive_Crank
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CrankAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Meshes/SM_Interactive_Crank.SM_Interactive_Crank"));
	check(CrankAsset.Object);
	StaticMeshes.Emplace(ECEInteractiveCrankStaticMesh::Crank, CrankAsset.Object);
	
	// SM_Interactive_Crank_Wheel
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WheelAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Meshes/SM_Interactive_Crank_Wheel.SM_Interactive_Crank_Wheel"));
	check(WheelAsset.Object);
	StaticMeshes.Emplace(ECEInteractiveCrankStaticMesh::Wheel, WheelAsset.Object);
}

void ACEInteractiveCrank::InitializeMaterials()
{
	// M_TextMaterial_Opaque_Emissive
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> OpaqueEmissiveAsset(TEXT("/Game/Assets/Materials/M_TextMaterial_Opaque_Emissive.M_TextMaterial_Opaque_Emissive"));
	check(OpaqueEmissiveAsset.Object);
	Materials.Emplace(ECEInteractiveCrankMaterial::OpaqueEmissive, OpaqueEmissiveAsset.Object);

	// M_Orange_Shiny_Highlighted
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> OrangeShinyAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Materials/M_Orange_Shiny_Highlighted.M_Orange_Shiny_Highlighted"));
	check(OrangeShinyAsset.Object);
	Materials.Emplace(ECEInteractiveCrankMaterial::OrangeShiny, OrangeShinyAsset.Object);
}
