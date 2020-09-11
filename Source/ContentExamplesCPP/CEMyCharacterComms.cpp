#include "CEMyCharacterComms.h"
#include "CEPlayerInteractions.h"
#include "CEProjectileFireball.h"
#include "CEProjectileWater.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavAreas/NavArea_Obstacle.h"

// Sets default values
ACEMyCharacterComms::ACEMyCharacterComms()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->InitCapsuleSize(40.0f, 100.0f);
    GetCapsuleComponent()->AreaClass = UNavArea_Obstacle::StaticClass();

    GetMesh()->SetRelativeLocation(FVector(0.0f, -1.41291f, -40.578861f));

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 70.0f));
    FirstPersonCamera->bUsePawnControlRotation = true;
    FirstPersonCamera->SetupAttachment(RootComponent);

    RightHand = CreateDefaultSubobject<USceneComponent>(TEXT("Right hand"));
    RightHand->SetRelativeLocation(FVector(0.0f, 90.0f, -50.0f));
    RightHand->SetupAttachment(FirstPersonCamera);

    LeftHand = CreateDefaultSubobject<USceneComponent>(TEXT("Left hand"));
    LeftHand->SetRelativeLocation(FVector(0.0f, -90.0f, -50.0f));
    LeftHand->SetupAttachment(FirstPersonCamera);

    bDoOnceFired = true;
}

// Called every frame
void ACEMyCharacterComms::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    DeltaSeconds = DeltaTime;
    if (bInteracting)
    {
        ICEPlayerInteractions* ITarget = Cast<ICEPlayerInteractions>(ActorToInteractWith);
        if (ITarget)
        {
            FVector2D AimMovement = FVector2D(GetInputAxisValue(FName(TEXT("Turn"))), GetInputAxisValue(FName(TEXT("LookUp")))); // Make a vector from the mouse/gamepad aim movement
            FVector PlayerFocusTarget = ITarget->GrabInteract(AimMovement, true); // Call an interface function inside the target C++
            FVector Start = FirstPersonCamera->GetComponentLocation();
            FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Start, PlayerFocusTarget);
            APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
            FRotator ControlRotation =  PlayerController->GetControlRotation();
            FRotator NewRotation = FMath::RInterpTo(ControlRotation, LookAtRotation, DeltaSeconds, 15.0f);
            PlayerController->SetControlRotation(NewRotation); // Rotate the player's view to focus on the interactive C++
            ResetDoOnce();
        }
    }
    else
    {
        if (!bDoOnceFired)
        {
            ICEPlayerInteractions* ITarget = Cast<ICEPlayerInteractions>(ActorToInteractWith);
            if (ITarget)
            {
                ITarget->GrabInteract(); // Call the interface function again with the "interacting" bool set to false
            }
            bDoOnceFired = true; // Fire once when the player releases the interact key/button
        }
    }
}

// Called to bind functionality to input
void ACEMyCharacterComms::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(FName(TEXT("MoveForward")), this, &ACEMyCharacterComms::MoveForward);
    PlayerInputComponent->BindAxis(FName(TEXT("MoveRight")), this, &ACEMyCharacterComms::MoveRight);
    PlayerInputComponent->BindAxis(FName(TEXT("Turn")), this, &ACEMyCharacterComms::Turn);
    PlayerInputComponent->BindAxis(FName(TEXT("LookUp")), this, &ACEMyCharacterComms::LookUp);

    PlayerInputComponent->BindAction(FName(TEXT("Run")), IE_Pressed, this, &ACEMyCharacterComms::RunPressed);
    PlayerInputComponent->BindAction(FName(TEXT("Run")), IE_Released, this, &ACEMyCharacterComms::RunReleased);
    PlayerInputComponent->BindAction(FName(TEXT("ShootFire")), IE_Pressed, this, &ACEMyCharacterComms::ShootFirePressed);
    PlayerInputComponent->BindAction(FName(TEXT("ShootFire")), IE_Released, this, &ACEMyCharacterComms::ShootFireReleased);
    PlayerInputComponent->BindAction(FName(TEXT("ShootWater")), IE_Pressed, this, &ACEMyCharacterComms::ShootWaterPressed);
    PlayerInputComponent->BindAction(FName(TEXT("ShootWater")), IE_Released, this, &ACEMyCharacterComms::ShootWaterReleased);
}

/*
 * // Custom function checks the overlapping actors and assigns the closest to a variable
 */
bool ACEMyCharacterComms::GetClosestInteractiveActor(TArray<AActor*> OtherActors)
{
    ActorToInteractWith = nullptr;
    ClosestDistance = 1000.0f;
    for (AActor* OtherActor : OtherActors)
    {
        ICEPlayerInteractions* TActor = Cast<ICEPlayerInteractions>(OtherActor);
        if (TActor) // Check that actors implement the specified C++ Interface
        {
            const float HorizontalDistance = GetHorizontalDistanceTo(OtherActor);
            if (HorizontalDistance < ClosestDistance) // If the distance is less than the previous closest actor...
            {
                ClosestDistance = HorizontalDistance; // Update closest distance
                ActorToInteractWith = OtherActor; // Update closest actor
            }
        }
    }
    return ActorToInteractWith != nullptr; // Was any valid actor set?
}

FTransform ACEMyCharacterComms::ProjectileTrace(FVector ProjectileOrigin)
{
    FHitResult HitResult;
    const FVector StartPosition = FirstPersonCamera->GetComponentLocation();
    const FVector EndPosition = StartPosition + FirstPersonCamera->GetForwardVector() * 5000.0f;
    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
    ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
    bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, StartPosition, EndPosition, ObjectQueryParams); // Trace forwards 10000 units in the direction the player is aiming
    
    FVector NewLocation = EndPosition; // If the trace hit nothing, use the end point location instead
    if (bHit)
    {
        NewLocation = HitResult.Location;
    }
    FRotator LookAtRotation =  UKismetMathLibrary::FindLookAtRotation(ProjectileOrigin, NewLocation);
    FTransform ProjectileTransformToUse = FTransform(LookAtRotation, ProjectileOrigin); // Make a transform oriented towards the location the trace hit
    return ProjectileTransformToUse;
}

void ACEMyCharacterComms::ShootFireball()
{
    if (bPowersEnabled)
    {
        FVector ProjectileOrigin = LeftHand->GetComponentLocation();
        FTransform ProjectileTransform = ProjectileTrace(ProjectileOrigin); // Custom function traces ahead of the player to determine a point to aim at
        GetWorld()->SpawnActor<ACEProjectileFireball>(ACEProjectileFireball::StaticClass(), ProjectileTransform);
    }
}

void ACEMyCharacterComms::ShootWater()
{
    if (bPowersEnabled)
    {
        FVector ProjectileOrigin = RightHand->GetComponentLocation();
        FTransform ProjectileTransform = ProjectileTrace(ProjectileOrigin);
        GetWorld()->SpawnActor<ACEProjectileWater>(ACEProjectileWater::StaticClass(), ProjectileTransform); 
    }
}

void ACEMyCharacterComms::Turn(float AxisValue)
{
    if (!bInteracting)
    {
        AddControllerYawInput(AxisValue);
    }
}

void ACEMyCharacterComms::LookUp(float AxisValue)
{
    if (!bInteracting)
    {
        AddControllerPitchInput(AxisValue);
    }
}

void ACEMyCharacterComms::MoveForward(float AxisValue)
{
    if (!bInteracting)
    {
        const FVector WorldDirection = GetControlRotation().Vector();
        AddMovementInput(WorldDirection, AxisValue);
    }
}

void ACEMyCharacterComms::MoveRight(float AxisValue)
{
    if (!bInteracting)
    {
        const FVector WorldDireciton = FRotationMatrix(GetControlRotation()).GetScaledAxis(EAxis::Y);
        AddMovementInput(WorldDireciton, AxisValue);
    }
}

void ACEMyCharacterComms::RunPressed()
{
    GetCharacterMovement()->MaxWalkSpeed = 2200.0f;;
}

void ACEMyCharacterComms::RunReleased()
{
    GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

void ACEMyCharacterComms::ShootWaterPressed()
{
    if (bPowersEnabled) // Is the ability to shoot enabled?
    {
        ShootWater(); // Call custom event to shoot water
        GetWorldTimerManager().SetTimer(ShootWaterTimer, this, &ACEMyCharacterComms::ShootWater, 0.2f, true); // Start a timer that repeatedly calls the custom event until stopped
    }
}

void ACEMyCharacterComms::ShootWaterReleased()
{
    GetWorldTimerManager().ClearTimer(ShootWaterTimer); // Stop shooting water
}

void ACEMyCharacterComms::ShootFirePressed()
{
    TArray<AActor*> OverlappingActors;
    GetOverlappingActors(OverlappingActors);
    bool bOverlappingValidActor = GetClosestInteractiveActor(OverlappingActors); 
    if (bOverlappingValidActor)
    {
        bInteracting = true;
    }
    else
    {
        if (bPowersEnabled) // Is the ability to shoot enabled?
        {
            ShootFireball(); // Call custom event to shoot a fireball
            GetWorldTimerManager().SetTimer(ShootFireTimer,this, &ACEMyCharacterComms::ShootFireball, 0.2f, true); // Start a timer that repeatedly calls the custom event until stopped
        }
    }
}

void ACEMyCharacterComms::ShootFireReleased()
{
    bInteracting = false;
    GetWorldTimerManager().ClearTimer(ShootFireTimer); // Stop shooting fireballs
}

void ACEMyCharacterComms::ResetDoOnce()
{
    bDoOnceFired = false;
}    
