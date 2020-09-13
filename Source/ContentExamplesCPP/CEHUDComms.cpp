#include "CEHUDComms.h"
#include "Engine/Canvas.h"

ACEHUDComms::ACEHUDComms()
{
    InitializeAssets();

    TextPopTimeline = CreateDefaultSubobject<UTimelineComponent>(FName(TEXT("Text pop")));
    HUDFont = Fonts[ECEHUDCommsFont::RobotoDistanceField];
    ResourceMaterial = Materials[ECEHUDCommsMaterial::CrossHair];
    VerticalOffsetCurve = Curves[ECEHUDCommsCurve::VerticalOffset];
}

void ACEHUDComms::BeginPlay()
{
    Super::BeginPlay();
    FOnTimelineFloat UpdateTimelineDelegate;
    UpdateTimelineDelegate.BindUFunction(this, FName(TEXT("UpdateTimelineCallback")));
    TextPopTimeline->AddInterpFloat(VerticalOffsetCurve, UpdateTimelineDelegate);
}


void ACEHUDComms::DrawHUD()
{
    Super::DrawHUD();
    SizeX = Canvas->SizeX;
    SizeY = Canvas->SizeY;
    float ScreenX = Canvas->SizeX / 2 - 20;
    float ScreenY = Canvas->SizeY / 2 - 20;
    DrawMaterial(ResourceMaterial, ScreenX, ScreenY, 40.0f, 40.0f, 0.0f, 0.0f, 1.0f, 1.0f); // Draw Crosshair in the center of the screen
    if (bPowersEnabled) // This bool draws text on the screen when the player pawn gains the ability to shoot projectiles
    {
        // Hud Elements for Shooting Projectiles (fade in and fade out)
        float OutWidth = 0;
        float OutHeight = 0;
        GetTextSize(FString(TEXT("Left click: Shoot fire")), OutWidth, OutHeight, HUDFont, 1.25f);
        const float ScreenX2 = SizeX - 150.0f - OutWidth;
        const float ScreenY2 = SizeY - 150.0f - VerticalOffset;
        const FLinearColor LeftTextColor = FLinearColor(1.0f, 0.8f, 0.7f, 1.0f);
        const FLinearColor RightTextColor = FLinearColor(0.7f, 0.8f, 1.0f, 1.0f);
        DrawText(FString(TEXT("Left click: Shoot fire")), LeftTextColor, 100.0f, ScreenY2, HUDFont, 1.25f);
        DrawText(FString(TEXT("Right click: Shoot water")), RightTextColor, ScreenX2, ScreenY2, HUDFont, 1.25f);
        if (!bDoOnceFired)
        {
            TextPopTimeline->PlayFromStart(); // Timeline is used to animate the text popping up so it draws attention
            bDoOnceFired = true;
        }
    }
    else
    {
        ResetDoOnce();
    }
}

void ACEHUDComms::UpdateTimelineCallback()
{
    const float PlaybackPosition = TextPopTimeline->GetPlaybackPosition();
    VerticalOffset = VerticalOffsetCurve->GetFloatValue(PlaybackPosition);
}

void ACEHUDComms::InitializeMaterials()
{
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> CrossHair(TEXT("/Game/ExampleContent/Blueprint_Communication/Materials/M_Crosshair.M_Crosshair"));
    check(CrossHair.Object);
    Materials.Emplace(ECEHUDCommsMaterial::CrossHair, CrossHair.Object);
}

void ACEHUDComms::InitializeFonts()
{
    static ConstructorHelpers::FObjectFinder<UFont> RobotoDistanceFieldAsset(TEXT("/Engine/EngineFonts/RobotoDistanceField.RobotoDistanceField"));
    check(RobotoDistanceFieldAsset.Object);
    Fonts.Emplace(ECEHUDCommsFont::RobotoDistanceField, RobotoDistanceFieldAsset.Object);
}

void ACEHUDComms::InitializeCurves()
{
    static ConstructorHelpers::FObjectFinder<UCurveFloat> VerticalOffsetAsset(TEXT("/Game/ContentExamplesCPP/Core/Curve_VerticalOffset_HUDComms_Float.Curve_VerticalOffset_HUDComms_Float"));
    check(VerticalOffsetAsset.Object);
    Curves.Emplace(ECEHUDCommsCurve::VerticalOffset, VerticalOffsetAsset.Object);
}

void ACEHUDComms::InitializeAssets()
{
    InitializeMaterials();
    InitializeFonts();
    InitializeCurves();
}
