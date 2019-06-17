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
#include "GameFramework/Actor.h"
#include "QLPickup.h"
#include "QLWeapon.generated.h"

class AQLCharacter;
class UAudioComponent;
class USphereComponent;

//------------------------------------------------------------
//------------------------------------------------------------
UCLASS()
class QL_API AQLWeapon : public AQLPickup
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AQLWeapon();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    virtual void OnFire();

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    virtual void OnFireRelease();

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    virtual void OnFireHold();

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    virtual void OnAltFire();

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    virtual void OnAltFireRelease();

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    virtual void OnAltFireHold();

    virtual void PlayAnimation(const FName& AnimationName) override;

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    USkeletalMeshComponent* GetGunSkeletalMeshComponent();

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    USphereComponent* GetRootSphereComponent();

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    FVector GetMuzzleLocation();

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    FName GetWeaponName();

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    void SetHitRange(float HitRange);

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    UTexture2D* GetCrossHairTexture(const FName& CrossHairTextureName = "Default");

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    void SetWeaponManager(UQLWeaponManager* WeaponManagerExt);

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    UQLWeaponManager* GetWeaponManager();

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    virtual void PrepareForImpendingWeaponSwitch();

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    virtual void SetDamageMultiplier(const float Value);

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    void StartGlow(const FColor& Color);

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    void StopGlow();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    virtual void PostInitializeComponents() override;

    virtual void EnableFireCallBack();

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    void OnComponentBeginOverlapImpl(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++Property")
    TMap<FName, UTexture2D*> CrosshairTextureList;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++Property")
    USphereComponent* RootSphereComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++Property")
    USkeletalMeshComponent* GunSkeletalMeshComponent;

    // Location on gun mesh where projectiles should spawn.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++Property")
    USceneComponent* MuzzleSceneComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++Property")
    UParticleSystemComponent* BeamComponent;

    UPROPERTY()
    TWeakObjectPtr<UMaterialInstanceDynamic> DynamicMaterialGun;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++Property")
    FName WeaponName;

    FTimerHandle HeldDownFireTimerHandle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++Property")
    float HitRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++Property")
    float RateOfFire;

    UPROPERTY()
    bool bIsFireHeld;

    UPROPERTY()
    TWeakObjectPtr<UQLWeaponManager> WeaponManager;

    FTimerHandle DisableFireTimerHandle;

    UPROPERTY()
    bool bFireEnabled;

    UPROPERTY()
    float DamageMultiplier;
};
