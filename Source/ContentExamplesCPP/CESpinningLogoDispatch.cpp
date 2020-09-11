#include "CESpinningLogoDispatch.h"
#include "CEButtonDispatch.h"
#include "Components/TimelineComponent.h"

// Sets default values
ACESpinningLogoDispatch::ACESpinningLogoDispatch()
{
	InitializeAssets();
	YawCurve = Curves[ECESpinningLogoDispatchCurve::Yaw];
	
    Base = CreateDefaultSubobject<USceneComponent>(TEXT("Base"));
	RootComponent = Base;

    Logo = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Logo"));
	Logo->SetStaticMesh(StaticMeshes[ECESpinningLogoDispatchStaticMesh::DoorLock]);
	Logo->SetRelativeScale3D(FVector(0.9f, 0.85f, 0.35f));
	Logo->GetBodyInstance()->bAutoWeld = false;
	Logo->SetMaterial(1, Materials[ECESpinningLogoDispatchMaterial::Chrome]);
	Logo->SetupAttachment(RootComponent);

	RotationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Rotation"));
}

// Called when the game starts or when spawned
void ACESpinningLogoDispatch::BeginPlay()
{
	Super::BeginPlay();
	check(TargetButton);

	TargetButton->OnButtonToggled().AddUFunction(this, FName(TEXT("ToggleSpinning"))); // Bind a custom event to the "Button toggled" Event Dispatcher

	FOnTimelineFloat UpdateTimelineDelegate;
	UpdateTimelineDelegate.BindUFunction(this, FName("UpdateTimelineCallback"));
	RotationTimeline->AddInterpFloat(YawCurve, UpdateTimelineDelegate);
	RotationTimeline->SetLooping(true);
}

/*
 * Toggle the spinning animation
 * Custom event fires when the Event Dispatcher function bound to it is called
 */
void ACESpinningLogoDispatch::ToggleSpinning(bool bIsOn)
{
	if (bIsOn)
	{
		Logo->SetMaterial(0, Materials[ECESpinningLogoDispatchMaterial::Lit]); // Switch logo material to a lit version
		RotationTimeline->Play();
	}
	else
	{
		Logo->SetMaterial(0, Materials[ECESpinningLogoDispatchMaterial::Unlit]); // Switch back to unlit logo material
		RotationTimeline->Stop();
	}
}

void ACESpinningLogoDispatch::UpdateTimelineCallback()
{
	const float PlaybackPosition = RotationTimeline->GetPlaybackPosition();
	const float Yaw = YawCurve->GetFloatValue(PlaybackPosition);
	const FRotator NewRotation = FRotator(0.0f, Yaw, 0.0f);
	Logo->SetRelativeRotation(NewRotation);
}

void ACESpinningLogoDispatch::InitializeAssets()
{
	InitializeStaticMeshes();
	InitializeCurves();
	InitializeMaterials();
}

void ACESpinningLogoDispatch::InitializeStaticMeshes()
{
	// SM_DoorLock
	ConstructorHelpers::FObjectFinder<UStaticMesh> DoorLockAsset(TEXT("/Game/DemoRoom/Meshes/SM_DoorLock.SM_DoorLock"));
	check(DoorLockAsset.Object);
	StaticMeshes.Emplace(ECESpinningLogoDispatchStaticMesh::DoorLock, DoorLockAsset.Object);
}

void ACESpinningLogoDispatch::InitializeCurves()
{
	//  Curve_Yaw_SpinningLogoDispatch_Float
	static ConstructorHelpers::FObjectFinder<UCurveFloat> YawAsset(TEXT("/Game/ContentExamplesCPP/Core/Curve_Yaw_SpinningLogoDispatch_Float.Curve_Yaw_SpinningLogoDispatch_Float"));
	check(YawAsset.Object);
	Curves.Emplace(ECESpinningLogoDispatchCurve::Yaw, YawAsset.Object);
}

void ACESpinningLogoDispatch::InitializeMaterials()
{
	//  M_Button_Chrome
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ChromeAsset(TEXT("/Game/DemoRoom/Materials/M_Button_Chrome.M_Button_Chrome"));
	check(ChromeAsset.Object);
	Materials.Emplace(ECESpinningLogoDispatchMaterial::Chrome, ChromeAsset.Object);
	
	//  M_UE4_Logo_Doors
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> LitAsset(TEXT("/Game/DemoRoom/Materials/M_UE4_Logo_Doors.M_UE4_Logo_Doors"));
	check(LitAsset.Object);
	Materials.Emplace(ECESpinningLogoDispatchMaterial::Lit, LitAsset.Object);

	//  M_UE4_Logo_Door
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> UnlitAsset(TEXT("/Game/DemoRoom/Materials/M_UE4_Logo_Door.M_UE4_Logo_Door"));
	check(UnlitAsset.Object);
	Materials.Emplace(ECESpinningLogoDispatchMaterial::Unlit, UnlitAsset.Object);
}
