#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScrewRotatorComponent.generated.h"

/*
 * AI Assistance Disclosure:
 * This component was developed by the student with AI assistance for code organization,
 * Unreal Engine reflection macro usage, editor-exposed property structure,
 * Blueprint event structure, and bonus feature planning.
 *
 * The final implementation was reviewed, compiled, tested, and adjusted by the student
 * inside Unreal Engine.
 */

// Defines the local axis used for screw rotation and screw depth movement.
UENUM(BlueprintType)
enum class EScrewAxis : uint8
{
    X UMETA(DisplayName = "X"),
    Y UMETA(DisplayName = "Y"),
    Z UMETA(DisplayName = "Z")
};

// Blueprint-assignable delegate used for limit events.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FScrewLimitReachedSignature);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SCREWMECHANISMTASK_API UScrewRotatorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UScrewRotatorComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction
    ) override;

public:
    // Editor-exposed screw configuration parameters.
    // TODO (Future Improvement): Add editable input key settings instead of hardcoding E and Q in C++.
    // TODO (Future Improvement): Add an option for screw handedness, such as clockwise or counter-clockwise tightening.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screw", meta = (ClampMin = "0.0"))
    float RotationSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screw", meta = (ClampMin = "0.0"))
    float MaxTurnCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screw")
    float ThreadPitch;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screw")
    EScrewAxis RotationAxis;

    // Bonus feature settings for smooth stopping near the screw limits.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screw|Bonus")
    bool bUseSmoothStop;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screw|Bonus", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float SmoothStopPercentage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screw|Bonus", meta = (ClampMin = "0.01", ClampMax = "1.0"))
    float MinimumSmoothSpeedMultiplier;

    // Bonus feature: Blueprint events that notify when the screw reaches its limits.
    UPROPERTY(BlueprintAssignable, Category = "Screw|Events")
    FScrewLimitReachedSignature OnScrewFullyTightened;

    UPROPERTY(BlueprintAssignable, Category = "Screw|Events")
    FScrewLimitReachedSignature OnScrewFullyLoosened;

    // Bonus feature: Helper functions callable from Blueprint.
    UFUNCTION(BlueprintCallable, Category = "Screw")
    void ResetScrew();

    UFUNCTION(BlueprintCallable, Category = "Screw")
    void SetScrewTurn(float NewTurn);

    UFUNCTION(BlueprintPure, Category = "Screw")
    float GetCurrentTurn() const;

    UFUNCTION(BlueprintPure, Category = "Screw")
    float GetCurrentAngle() const;

    UFUNCTION(BlueprintPure, Category = "Screw")
    float GetCurrentDepth() const;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Screw|Runtime", meta = (AllowPrivateAccess = "true"))
    float CurrentTurn;

    FTransform InitialTransform;

    bool bHasInitialTransform;
    bool bTightenedEventSent;
    bool bLoosenedEventSent;

    FString CurrentDirectionText;

private:
    int32 GetDirectionInput() const;

    FVector GetLocalAxisVector() const;

    void ApplyScrewTransform();

    float GetEffectiveRotationSpeed(int32 Direction) const;

    void UpdateLimitEvents(float OldTurn, float NewTurn);

    void DrawDebugInfo() const;
};
