#include "ScrewRotatorComponent.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"

/*
 * AI Assistance Disclosure:
 * AI assistance was used while designing the tick-based screw update flow,
 * turn/depth calculations, smooth stop interpolation, Blueprint event broadcasting,
 * and debug message organization.
 *
 * The code was tested and adjusted by the student in Unreal Engine.
 */

UScrewRotatorComponent::UScrewRotatorComponent()
{
    // Initializes default editor values and enables ticking for runtime screw updates.
    // AI-assisted area: default value organization and component tick setup.
    
    // TODO (Future Improvement): Move default screw presets into a Data Asset if multiple screw types are needed.
    PrimaryComponentTick.bCanEverTick = true;

    RotationSpeed = 180.0f;
    MaxTurnCount = 5.0f;
    ThreadPitch = 2.0f;
    RotationAxis = EScrewAxis::Z;

    bUseSmoothStop = true;
    SmoothStopPercentage = 0.10f;
    MinimumSmoothSpeedMultiplier = 0.15f;

    CurrentTurn = 0.0f;

    bHasInitialTransform = false;
    bTightenedEventSent = false;
    bLoosenedEventSent = false;

    CurrentDirectionText = TEXT("Idle");
}

void UScrewRotatorComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Stores the owner's initial transform so screw movement can be calculated relative to the starting position.
    AActor* Owner = GetOwner();

    if (Owner)
    {
        InitialTransform = Owner->GetActorTransform();
        bHasInitialTransform = true;

        CurrentTurn = FMath::Clamp(CurrentTurn, 0.0f, MaxTurnCount);
        ApplyScrewTransform();
    }
}

void UScrewRotatorComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction
)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Main runtime update loop.
    // AI-assisted area: frame-rate independent turn update using DeltaTime.
    if (!bHasInitialTransform || MaxTurnCount <= 0.0f)
    {
        DrawDebugInfo();
        return;
    }

    const int32 Direction = GetDirectionInput();

    if (Direction == 0)
    {
        CurrentDirectionText = TEXT("Idle");
        DrawDebugInfo();
        return;
    }

    CurrentDirectionText = Direction > 0 ? TEXT("Tightening") : TEXT("Loosening");

    const float OldTurn = CurrentTurn;

    const float EffectiveSpeed = GetEffectiveRotationSpeed(Direction);
    const float DeltaAngle = EffectiveSpeed * DeltaTime * static_cast<float>(Direction);
    const float DeltaTurn = DeltaAngle / 360.0f;

    // Clamp keeps the screw inside the valid range: 0 <= CurrentTurn <= MaxTurnCount.
    const float NewTurn = FMath::Clamp(CurrentTurn + DeltaTurn, 0.0f, MaxTurnCount);

    if (!FMath::IsNearlyEqual(NewTurn, CurrentTurn))
    {
        CurrentTurn = NewTurn;
        ApplyScrewTransform();
        UpdateLimitEvents(OldTurn, CurrentTurn);
    }

    DrawDebugInfo();
}

int32 UScrewRotatorComponent::GetDirectionInput() const
{
    // Reads E and Q directly from the first player controller.
    // TODO (Future Improvement): Replace direct key polling with Enhanced Input actions for larger projects.
    const UWorld* World = GetWorld();

    if (!World)
    {
        return 0;
    }

    const APlayerController* PlayerController = World->GetFirstPlayerController();

    if (!PlayerController)
    {
        return 0;
    }

    const bool bTighteningPressed = PlayerController->IsInputKeyDown(EKeys::E);
    const bool bLooseningPressed = PlayerController->IsInputKeyDown(EKeys::Q);

    if (bTighteningPressed == bLooseningPressed)
    {
        return 0;
    }

    return bTighteningPressed ? 1 : -1;
}

FVector UScrewRotatorComponent::GetLocalAxisVector() const
{
    switch (RotationAxis)
    {
    case EScrewAxis::X:
        return FVector::ForwardVector;

    case EScrewAxis::Y:
        return FVector::RightVector;

    case EScrewAxis::Z:
    default:
        return FVector::UpVector;
    }
}

void UScrewRotatorComponent::ApplyScrewTransform()
{
    // Applies both rotation and translation to the owner Actor.
    // AI-assisted area: transform calculation based on initial transform to avoid cumulative drift.
    AActor* Owner = GetOwner();

    if (!Owner || !bHasInitialTransform)
    {
        return;
    }

    const FVector LocalAxis = GetLocalAxisVector();

    const float AngleRadians = FMath::DegreesToRadians(GetCurrentAngle());

    const FQuat DeltaRotation(LocalAxis, AngleRadians);

    FQuat NewRotation = InitialTransform.GetRotation() * DeltaRotation;
    NewRotation.Normalize();

    const FVector WorldAxis = InitialTransform.TransformVectorNoScale(LocalAxis).GetSafeNormal();

    // Screw depth is calculated from the current turn count and thread pitch.
    const FVector NewLocation = InitialTransform.GetLocation() + WorldAxis * GetCurrentDepth();

    Owner->SetActorLocationAndRotation(
        NewLocation,
        NewRotation,
        false,
        nullptr,
        ETeleportType::TeleportPhysics
    );
}

float UScrewRotatorComponent::GetEffectiveRotationSpeed(int32 Direction) const
{
    // Calculates reduced rotation speed when the screw approaches a limit.
    // AI-assisted area: smooth stop interpolation using FMath::Lerp.
    // TODO (Future Improvement): Expose the slow-down curve as a CurveFloat for more customizable behavior.
    if (!bUseSmoothStop || SmoothStopPercentage <= 0.0f)
    {
        return RotationSpeed;
    }

    const float SlowRange = MaxTurnCount * SmoothStopPercentage;

    if (SlowRange <= KINDA_SMALL_NUMBER)
    {
        return RotationSpeed;
    }

    const float DistanceToLimit = Direction > 0
        ? MaxTurnCount - CurrentTurn
        : CurrentTurn;

    const float Alpha = FMath::Clamp(DistanceToLimit / SlowRange, 0.0f, 1.0f);

    const float SpeedMultiplier = FMath::Lerp(
        MinimumSmoothSpeedMultiplier,
        1.0f,
        Alpha
    );

    return RotationSpeed * SpeedMultiplier;
}

void UScrewRotatorComponent::UpdateLimitEvents(float OldTurn, float NewTurn)
{
    // Broadcasts Blueprint events when the screw reaches its min or max limit.
    // AI-assisted area: event guard flags prevent repeated event broadcasting every frame.
    const bool bFullyTightened = FMath::IsNearlyEqual(NewTurn, MaxTurnCount, 0.001f);
    const bool bFullyLoosened = FMath::IsNearlyZero(NewTurn, 0.001f);

    if (bFullyTightened && OldTurn < MaxTurnCount && !bTightenedEventSent)
    {
        OnScrewFullyTightened.Broadcast();
        bTightenedEventSent = true;
    }

    if (!bFullyTightened)
    {
        bTightenedEventSent = false;
    }

    if (bFullyLoosened && OldTurn > 0.0f && !bLoosenedEventSent)
    {
        OnScrewFullyLoosened.Broadcast();
        bLoosenedEventSent = true;
    }

    if (!bFullyLoosened)
    {
        bLoosenedEventSent = false;
    }
}

void UScrewRotatorComponent::DrawDebugInfo() const
{
    // Displays runtime debug information requested in the assignment.
    // TODO (Future Improvement): Add a bShowDebugInfo editor setting to enable or disable on-screen debug messages.
    if (!GEngine)
    {
        return;
    }

    GEngine->AddOnScreenDebugMessage(
        1001,
        0.05f,
        FColor::Cyan,
        FString::Printf(TEXT("Current Angle : %.0f"), GetCurrentAngle())
    );

    GEngine->AddOnScreenDebugMessage(
        1002,
        0.05f,
        FColor::Cyan,
        FString::Printf(TEXT("Current Turn : %.2f / %.2f"), CurrentTurn, MaxTurnCount)
    );

    GEngine->AddOnScreenDebugMessage(
        1003,
        0.05f,
        FColor::Cyan,
        FString::Printf(TEXT("Current Depth : %.2f"), GetCurrentDepth())
    );

    GEngine->AddOnScreenDebugMessage(
        1004,
        0.05f,
        FColor::Cyan,
        FString::Printf(TEXT("Direction : %s"), *CurrentDirectionText)
    );

    if (FMath::IsNearlyEqual(CurrentTurn, MaxTurnCount, 0.001f))
    {
        GEngine->AddOnScreenDebugMessage(
            1005,
            0.05f,
            FColor::Green,
            TEXT("Fully Tightened")
        );
    }
    else if (FMath::IsNearlyZero(CurrentTurn, 0.001f))
    {
        GEngine->AddOnScreenDebugMessage(
            1005,
            0.05f,
            FColor::Yellow,
            TEXT("Fully Loosened")
        );
    }
}

// Resets the screw to the fully loosened position.
void UScrewRotatorComponent::ResetScrew()
{
    SetScrewTurn(0.0f);
}

// Sets the screw turn manually while keeping the value inside valid limits.
void UScrewRotatorComponent::SetScrewTurn(float NewTurn)
{
    CurrentTurn = FMath::Clamp(NewTurn, 0.0f, MaxTurnCount);

    bTightenedEventSent = FMath::IsNearlyEqual(CurrentTurn, MaxTurnCount, 0.001f);
    bLoosenedEventSent = FMath::IsNearlyZero(CurrentTurn, 0.001f);

    ApplyScrewTransform();
}

// Returns the current screw turn value.
float UScrewRotatorComponent::GetCurrentTurn() const
{
    return CurrentTurn;
}

// Returns the current screw angle in degrees.
float UScrewRotatorComponent::GetCurrentAngle() const
{
    return CurrentTurn * 360.0f;
}

// Returns the current screw depth based on thread pitch.
float UScrewRotatorComponent::GetCurrentDepth() const
{
    return CurrentTurn * ThreadPitch;
}
