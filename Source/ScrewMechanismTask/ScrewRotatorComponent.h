#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScrewRotatorComponent.generated.h"

UENUM(BlueprintType)
enum class EScrewAxis : uint8
{
    X UMETA(DisplayName = "X"),
    Y UMETA(DisplayName = "Y"),
    Z UMETA(DisplayName = "Z")
};

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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screw", meta = (ClampMin = "0.0"))
    float RotationSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screw", meta = (ClampMin = "0.0"))
    float MaxTurnCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screw")
    float ThreadPitch;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screw")
    EScrewAxis RotationAxis;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screw|Bonus")
    bool bUseSmoothStop;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screw|Bonus", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float SmoothStopPercentage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screw|Bonus", meta = (ClampMin = "0.01", ClampMax = "1.0"))
    float MinimumSmoothSpeedMultiplier;

    UPROPERTY(BlueprintAssignable, Category = "Screw|Events")
    FScrewLimitReachedSignature OnScrewFullyTightened;

    UPROPERTY(BlueprintAssignable, Category = "Screw|Events")
    FScrewLimitReachedSignature OnScrewFullyLoosened;

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
