#include "CEInteractiveChain.h"
#include "CELEDBasic.h"
#include "ClothingSimulationFactory.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavAreas/NavArea_Obstacle.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

// Sets default values
ACEInteractiveChain::ACEInteractiveChain()
{
	InitializeAssets();
	ChainHeightCurve = Curves[ECEInteractiveChainCurve::ChainHeight];
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ChainBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chain base"));
	ChainBase->SetStaticMesh(StaticMeshes[ECEInteractiveChainStaticMesh::Box]);
	ChainBase->GetBodyInstance()->bAutoWeld = false;
	RootComponent = ChainBase;

	Chain = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Chain"));
	Chain->SetRelativeLocation(FVector(0.0f, 0.0f, -5.0f));
	Chain->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.6f));
	Chain->SetSkeletalMesh(SkeletalMeshes[ECEInteractiveChainSkeletalMesh::Chain]);
	Chain->CanCharacterStepUpOn = ECB_Yes;
	Chain->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	Chain->ClothingSimulationFactory = UClothingSimulationFactory::GetDefaultClothingSimulationFactoryClass();
	Chain->SetupAttachment(RootComponent);

	Handle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Handle"));
	Handle->SetStaticMesh(StaticMeshes[ECEInteractiveChainStaticMesh::Handle]);
	Handle->SetRelativeLocation(FVector(30.0f, 0.0f, 0.0f));
	Handle->SetRelativeRotation(FRotator(0.0f, 90.0f, 90.0f));
	Handle->SetUsingAbsoluteScale(true);
	Handle->GetBodyInstance()->bAutoWeld = false;
	Handle->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	Handle->SetupAttachment(Chain, FName(TEXT("Object012")));

	HandleHighlighted = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Handle highlighted"));
	HandleHighlighted->SetStaticMesh(StaticMeshes[ECEInteractiveChainStaticMesh::Handle]);
	HandleHighlighted->SetUsingAbsoluteScale(true);
	HandleHighlighted->GetBodyInstance()->bAutoWeld = false;
	HandleHighlighted->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	HandleHighlighted->SetVisibleFlag(false);
	HandleHighlighted->SetMaterial(0, Materials[ECEInteractiveChainMaterial::OrangeShiny]);
	HandleHighlighted->bRenderCustomDepth = true;
	HandleHighlighted->SetupAttachment(Handle);

	InteractTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Interact trigger"));
	InteractTrigger->SetRelativeLocation(FVector(30.0f, 0.0f, 0.0f));
	InteractTrigger->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	InteractTrigger->InitCapsuleSize(60.0f, 100.0f);
	InteractTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractTrigger->AreaClass = UNavArea_Obstacle::StaticClass();
	InteractTrigger->SetupAttachment(Chain, FName(TEXT("Object012")));

	TextRender1 = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRender1"));
	TextRender1->SetRelativeLocation(FVector(0.0f, 0.0f, -125.0f));
	TextRender1->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	TextRender1->SetMaterial(0, Materials[ECEInteractiveChainMaterial::OpaqueEmissive]);
	TextRender1->Text = FText::FromString(TEXT("Click and drag to interact"));
	TextRender1->HorizontalAlignment = EHTA_Center;
	TextRender1->XScale = 0.25f;
	TextRender1->YScale = 0.25f;
	TextRender1->SetVisibleFlag(false);
	TextRender1->SetupAttachment(Chain);
	
	ChainPullTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Chain pull"));

	// PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("Physics handle"));
	// PhysicsHandle->CurrentTransform = Handle->GetRelativeTransform();

	bIsOn = true;
	bA = true;
	bB = true;
}

// Called when the game starts or when spawned
void ACEInteractiveChain::BeginPlay()
{
	Super::BeginPlay();
	check(TargetLED);

	InteractTrigger->OnComponentBeginOverlap.AddDynamic(this, &ACEInteractiveChain::OnTriggerBeginOverlap);
	InteractTrigger->OnComponentEndOverlap.AddDynamic(this, &ACEInteractiveChain::OnTriggerEndOverlap);

	FOnTimelineFloat UpdateTimelineDelegate;
	UpdateTimelineDelegate.BindUFunction(this, FName(TEXT("UpdateTimelineCallback")));
	ChainPullTimeline->AddInterpFloat(ChainHeightCurve, UpdateTimelineDelegate);

	FOnTimelineEvent FinishedTimelineDelegate;
	FinishedTimelineDelegate.BindUFunction(this, FName(TEXT("FinishedTimelineCallback")));
	ChainPullTimeline->SetTimelineFinishedFunc(FinishedTimelineDelegate);
}

// Called every frame
void ACEInteractiveChain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DeltaSeconds = DeltaTime; // Set Delta Time
}

/*
 * Check for interaction
 * Interface function to be called by other C++s
 */
FVector ACEInteractiveChain::GrabInteract(FVector2D PlayerAimMovement, bool bIsInteracting)
{
	if (bIsInteracting)
	{
		const FRotator ControlRotation = UGameplayStatics::GetPlayerPawn(this, 0)->GetControlRotation();
		FVector NewHandleTarget = FVector(0.0f, PlayerAimMovement.X, 0.0f); // Turn the player's horizontal drag direction into a vector
		NewHandleTarget = ControlRotation.RotateVector(NewHandleTarget); // Offset the vector according to the direction the player is facing
		NewHandleTarget *= 15.0f; // Multiply how far the handle can be dragged
		if (PlayerAimMovement.Y > 0.5f) // Is the player dragging downwards?
		{
			MoveChain(true); // Call a custom event to move the chain down
			NewHandleTarget = FVector(0.0f, 0.0f, -100.0f); // If dragging down, override any sideways movement
		}
		if (!PhysicsHandle) // Has a physics handle component been set?
		{
			PhysicsHandle = NewObject<UPhysicsHandleComponent>(this, FName(TEXT("Physics handle"))); // Add a physics handle to the end of the chain so it can be pulled around
			PhysicsHandle->CurrentTransform = Handle->GetRelativeTransform();
		}
		else if (PhysicsHandle && !bIsGrabbed)
		{
			PhysicsHandle->GrabComponentAtLocation(Chain, NAME_None, Handle->GetComponentLocation());
			bIsGrabbed = true;	
		}
		HandleTarget = FMath::VInterpTo(NewHandleTarget, NewHandleTarget, DeltaSeconds, 3.0f); // Smoothly update the handle target location
	}
	else // Not Interacting: Chain moves back up
	{
		MoveChain(false); // Call a custom event to move the chain back up
		PhysicsHandle->ReleaseComponent(); // Let go of the end of the chain
		bIsGrabbed = false;
		HandleTarget = FVector::ZeroVector;
	}
	const FVector SocketLocation = Handle->GetSocketLocation(FName(TEXT("Focus target")));
	PhysicsHandle->SetTargetLocation(HandleTarget + SocketLocation); // Set a location the chain will be dragged towards
	return SocketLocation; // Return the location of the switch handle
}


void ACEInteractiveChain::InitializeAssets()
{
	InitializeMaterials();
	InitializeStaticMeshes();
	InitializeCurves();
	InitializeSkeletalMeshes();
}

void ACEInteractiveChain::InitializeStaticMeshes()
{
	// SM_Interactive_Box
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Meshes/SM_Interactive_Box.SM_Interactive_Box"));
	check(BoxAsset.Object);
	StaticMeshes.Emplace(ECEInteractiveChainStaticMesh::Box, BoxAsset.Object);

	// SM_Interactive_Chain_Handle
	static ConstructorHelpers::FObjectFinder<UStaticMesh> HandleAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Meshes/SM_Interactive_Chain_Handle.SM_Interactive_Chain_Handle"));
	check(HandleAsset.Object);
	StaticMeshes.Emplace(ECEInteractiveChainStaticMesh::Handle, HandleAsset.Object);
}

void ACEInteractiveChain::InitializeMaterials()
{
	// M_TextMaterial_Opaque_Emissive
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> OpaqueEmissiveAsset(TEXT("/Game/Assets/Materials/M_TextMaterial_Opaque_Emissive.M_TextMaterial_Opaque_Emissive"));
	check(OpaqueEmissiveAsset.Object);
	Materials.Emplace(ECEInteractiveChainMaterial::OpaqueEmissive, OpaqueEmissiveAsset.Object);

	// M_Orange_Shiny_Highlighted
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> OrangeShinyAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Materials/M_Orange_Shiny_Highlighted.M_Orange_Shiny_Highlighted"));
	check(OrangeShinyAsset.Object);
	Materials.Emplace(ECEInteractiveChainMaterial::OrangeShiny, OrangeShinyAsset.Object);
}

void ACEInteractiveChain::InitializeCurves()
{
	// Curve_ChainHeight_InteractiveChain_Float 
	ConstructorHelpers::FObjectFinder<UCurveFloat> ChainHeightAsset(TEXT("/Game/ContentExamplesCPP/Core/Curve_ChainHeight_InteractiveChain_Float.Curve_ChainHeight_InteractiveChain_Float"));
	check(ChainHeightAsset.Object);
	Curves.Emplace(ECEInteractiveChainCurve::ChainHeight, ChainHeightAsset.Object); 
}

void ACEInteractiveChain::InitializeSkeletalMeshes()
{
	// S_Chain
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ChainAsset(TEXT("/Game/ExampleContent/Blueprint_Communication/Meshes/S_Chain"));
	check(ChainAsset.Object);
	SkeletalMeshes.Emplace(ECEInteractiveChainSkeletalMesh::Chain, ChainAsset.Object);
}

/*
 * Set Visibilty of Components
 * Overlap event fires when the pawn touches the trigger
 */
void ACEInteractiveChain::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		// Swap visibility to show/hide a highlighted version of the mesh
		Handle->ToggleVisibility();
		HandleHighlighted->ToggleVisibility();
		TextRender1->ToggleVisibility();
	}
}

/*
 * Set Visibilty of Components
 * Overlap event fires when a pawn exits the trigger
 */
void ACEInteractiveChain::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		// Swap visibility to show/hide a highlighted version of the mesh
		Handle->ToggleVisibility();
		HandleHighlighted->ToggleVisibility();
		TextRender1->ToggleVisibility();	
	}
}

void ACEInteractiveChain::UpdateTimelineCallback()
{
	const float PlaybackPosition = ChainPullTimeline->GetPlaybackPosition();
	const float ChainHeight = ChainHeightCurve->GetFloatValue(PlaybackPosition);
	Chain->SetRelativeLocation(FVector(0.0f, 0.0f, ChainHeight)); // Move the chain up/down
}

/*
 * If the animation is finished in the right direction it will toggle the light
 */
void ACEInteractiveChain::FinishedTimelineCallback()
{
	if (Direction == ETimelineDirection::Forward) // Did the timeline finish playing forwards?
	{
		TargetLED->ToggleLight(bIsOn); // Call a custom event in the target to turn its light on/off
		bIsOn = !bIsOn;
	}
}

/*
 * Custom event called inside the "Grab interact" Interface function
 */
void ACEInteractiveChain::MoveChain(bool bDirection)
{
	// Execute once until reset
	if (bDirection && bA)
	{
		bA = false;
		ChainPullTimeline->Play();
		Direction = ETimelineDirection::Forward;
		bB = true;
	}
	else if (bB)
	{
		bB = false;
		ChainPullTimeline->Reverse();
		Direction = ETimelineDirection::Backward;
		bA = true;
	}
}
