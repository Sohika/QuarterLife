//----------------------------------------
// Quarter Life
//
// GNU General Public License v3.0
//
//  (\-/)
// (='.'=)
// (")-(")o
//----------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "QLCharacter.generated.h"

class AQLWeapon;
class UQLWeaponManager;

//------------------------------------------------------------
//------------------------------------------------------------
UCLASS()
class QL_API AQLCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AQLCharacter();

    // Called every frame
    // virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    FHitResult RayTraceFromCharacterPOV(float rayTraceRange = 1e5f);

    // Returns FirstPersonMesh subobject
    UFUNCTION(BlueprintCallable, Category = "C++Function")
    class USkeletalMeshComponent* GetFirstPersonMesh();

    // Returns FirstPersonCameraComponent subobject
    UFUNCTION(BlueprintCallable, Category = "C++Function")
    class UCameraComponent* GetFirstPersonCameraComponent() const;

    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
protected:

    // Pawn mesh : 1st person view(arms; seen only by self)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++Property")
    class USkeletalMeshComponent* FirstPersonMesh;

    // Pawn mesh: 1st person view (arms; seen only by self)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++Property")
    class USkeletalMeshComponent* ThirdPersonMesh;

    // First person camera
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++Property", meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FirstPersonCameraComponent;

    // Base turn rate, in deg/sec. Other scaling may affect final turn rate.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++Property")
    float BaseTurnRate;

    // Base look up/down rate, in deg/sec. Other scaling may affect final rate.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++Property")
    float BaseLookUpRate;

    // Projectile class to spawn
    // UPROPERTY(EditDefaultsOnly, Category = Projectile)
    // TSubclassOf<class ATestFirstPersonProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++Property")
    float Health;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++Property")
    float MaxHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++Property")
    float Armor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++Property")
    float MaxArmor;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    float GetHealth() const;

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    float GetMaxHealth() const;

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    float GetArmor() const;

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    float GetMaxArmor() const;

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    void AddWeapon(AQLWeapon* Weapon);

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    void SetCurrentWeapon(const FName& WeaponName);

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Handles moving forward/backward
    void MoveForward(float Val);

    // Handles strafing movement, left and right
    void MoveRight(float Val);

    // Called via input to turn at a given rate.
    // This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
    void TurnAtRate(float Rate);

    // Called via input to turn look up/down at a given rate.
    // This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
    void LookUpAtRate(float Rate);

    // Fires a projectile.
    UFUNCTION(BlueprintCallable, Category = "C++Function")
    void OnFire();

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    void OnFireRelease();

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    void OnAltFire();

protected:

    UQLWeaponManager* WeaponManager;
};
