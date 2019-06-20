//------------------------------------------------------------
// Quarter Life
//
// GNU General Public License v3.0
//
//  (\-/)
// (='.'=)
// (")-(")o
//------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "QLPlayerController.generated.h"

class UQLUmgFirstPerson;
class UQLUmgAbility;

//------------------------------------------------------------
//------------------------------------------------------------
UCLASS()
class QL_API AQLPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    AQLPlayerController();

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    float GetFrameRate() const;

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    float GetControlledPawnSpeed() const;

    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    void AddUMG();

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    UQLUmgFirstPerson* GetUMG();

    UPROPERTY(EditDefaultsOnly, Category = "C++Property")
    TSubclassOf<UQLUmgFirstPerson> UmgFirstPersonClass;

    UPROPERTY(EditDefaultsOnly, Category = "C++Property")
    TSubclassOf<UQLUmgAbility> UmgAbilityClass;

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    void ShowDamageOnScreen(float DamageAmount, const FVector& WorldTextLocation);

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    void ShowAbilityMenu();
protected:
    //------------------------------------------------------------
    //------------------------------------------------------------
    virtual void PostInitializeComponents() override;

    //------------------------------------------------------------
    //------------------------------------------------------------
    virtual void BeginPlay() override;

    //------------------------------------------------------------
    //------------------------------------------------------------
    virtual void OnPossess(APawn* ControlledPawn) override;

    //------------------------------------------------------------
    //------------------------------------------------------------
    virtual void SetupInputComponent() override;

    UPROPERTY()
    UQLUmgFirstPerson* UmgFirstPerson;

    UPROPERTY()
    UQLUmgAbility* UmgAbility;

    float FPS;
};
