#include "CESpinningLogo.h"
#include "Components/TimelineComponent.h"

// Sets default values
ACESpinningLogo::ACESpinningLogo()
{
	InitializeAssets();
	YawCurve = Curves[ECESpinningLogoCurve::Yaw];
	
    Base = CreateDefaultSubobject<USceneComponent>(TEXT("Base"));
	RootComponent = Base;

    Logo = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Logo"));
	Logo->SetStaticMesh(StaticMeshes[ECESpinningLogoStaticMesh::DoorLock]);
	Logo->SetRelativeScale3D(FVector(0.4f, 0.64f, 0.64f));
	Logo->SetMaterial(1, Materials[ECESpinningLogoMaterial::Chrome]);
	Logo->GetBodyInstance()->bAutoWeld = false;
	Logo->SetupAttachment(RootComponent);

    RotationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Rotation"));
}

// Called when the game starts or when spawned
void ACESpinningLogo::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat UpdateTimelineDelegate;
	UpdateTimelineDelegate.BindUFunction(this, FName(TEXT("UpdateTimelineCallback")));
	RotationTimeline->AddInterpFloat(YawCurve, UpdateTimelineDelegate);
}

/*
 * Toggle the spinning animation
 */
void ACESpinningLogo::PushedButton()
{
	if (bIsA)
	{
		Logo->SetMaterial(0, Materials[ECESpinningLogoMaterial::Lit]); // Switch logo material to a lit version
		RotationTimeline->SetLooping(true); // Set whether the timeline is looping or not
		RotationTimeline->Play();
	}
	else
	{
		Logo->SetMaterial(0, Materials[ECESpinningLogoMaterial::Unlit]); // Switch back to unlit logo material
		RotationTimeline->SetLooping(false); // Turn off looping so the rotation stops
		RotationTimeline->Stop();
	}
	bIsA = !bIsA;
}

void ACESpinningLogo::UpdateTimelineCallback()
{
	const float PlaybackPosition = RotationTimeline->GetPlaybackPosition();
	const float Yaw = YawCurve->GetFloatValue(PlaybackPosition);
	FRotator NewRotation = FRotator(0.0f, Yaw, 0.0f);
	Logo->SetRelativeRotation(NewRotation);
}

void ACESpinningLogo::InitializeAssets()
{
	InitializeStaticMeshes();
	InitializeCurves();
	InitializeMaterials();
}

void ACESpinningLogo::InitializeStaticMeshes()
{
	// SM_DoorLock
	ConstructorHelpers::FObjectFinder<UStaticMesh> DoorLockAsset(TEXT("/Game/DemoRoom/Meshes/SM_DoorLock.SM_DoorLock"));
	check(DoorLockAsset.Object);
	StaticMeshes.Emplace(ECESpinningLogoStaticMesh::DoorLock, DoorLockAsset.Object);
}

void ACESpinningLogo::InitializeCurves()
{
	// Curve_Yaw_SpinningLogo_Float
	static ConstructorHelpers::FObjectFinder<UCurveFloat> YawAsset(TEXT("/Game/ContentExamplesCPP/Core/Curve_Yaw_SpinningLogo_Float.Curve_Yaw_SpinningLogo_Float"));
	check(YawAsset.Object);
	Curves.Emplace(ECESpinningLogoCurve::Yaw, YawAsset.Object);
}

void ACESpinningLogo::InitializeMaterials()
{
	// M_Button_Chrome
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ChromeAsset(TEXT("/Game/DemoRoom/Materials/M_Button_Chrome.M_Button_Chrome"));
	check(ChromeAsset.Object);
	Materials.Emplace(ECESpinningLogoMaterial::Chrome, ChromeAsset.Object);
	
	// M_UE4_Logo_Doors
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> LitAsset(TEXT("/Game/DemoRoom/Materials/M_UE4_Logo_Doors.M_UE4_Logo_Doors"));
	check(LitAsset.Object);
	Materials.Emplace(ECESpinningLogoMaterial::Lit, LitAsset.Object);

	// M_UE4_Logo_Door
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> UnlitAsset(TEXT("/Game/DemoRoom/Materials/M_UE4_Logo_Door.M_UE4_Logo_Door"));
	check(UnlitAsset.Object);
	Materials.Emplace(ECESpinningLogoMaterial::Unlit, UnlitAsset.Object);
}
