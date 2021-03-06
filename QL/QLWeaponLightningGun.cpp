//------------------------------------------------------------
// Quarter Life
//
// GNU General Public License v3.0
//
//  (\-/)
// (='.'=)
// (")-(")o
//------------------------------------------------------------

#include "QLWeaponLightningGun.h"
#include "QLCharacter.h"
#include "QLUtility.h"
#include "Engine/World.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/DamageType.h"
#include "Components/AudioComponent.h"
#include "QLWeaponManager.h"
#include "QLUmgFirstPerson.h"
#include "QLPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

//------------------------------------------------------------
//------------------------------------------------------------
AQLWeaponLightningGun::AQLWeaponLightningGun()
{
    QLName = FName(TEXT("LightningGun"));
    HitRange = 1200.0f;
    RateOfFire = 0.05f;
    bIsFireHeld = false;

    BasicDamage = 6.0f;
    KnockbackSpeedChange = 50.0f;
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLWeaponLightningGun::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    SetDamageMultiplier(1.0f);

    if (BeamComponent)
    {
        // BeamComponent->bAutoActivate = false does not work
        BeamComponent->Deactivate();
    }
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLWeaponLightningGun::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsFireHeld)
    {
        OnFireHold();
    }
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLWeaponLightningGun::OnFire()
{
    if (bIsFireHeld)
    {
        return;
    }

    PlaySound(FName(TEXT("Fire")));

    bIsFireHeld = true;

    // handle beam
    if (BeamComponent)
    {
        BeamComponent->Activate();
    }

    GetWorldTimerManager().SetTimer(HoldFireTimerHandle,
                                    this,
                                    &AQLWeaponLightningGun::SpawnLightning,
                                    RateOfFire, // time interval in second
                                    true, // loop
                                    0.0f); // delay in second
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLWeaponLightningGun::OnFireRelease()
{
    if (!bIsFireHeld)
    {
        return;
    }

    StopSound();

    bIsFireHeld = false;

    if (BeamComponent)
    {
        BeamComponent->Deactivate();
    }

    GetWorldTimerManager().ClearTimer(HoldFireTimerHandle);
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLWeaponLightningGun::OnFireHold()
{
    AQLCharacter* User = GetWeaponManager()->GetUser();

    // to do: in order to ensure correctness, for each tick, ray trace is performed twice, one in Tick(), the other in HasHitEnemy()
    // need to understand the tick order and simplify the calculation
    FHitResult HitResult = User->RayTraceFromCharacterPOV(HitRange);

    if (BeamComponent)
    {
        BeamComponent->SetBeamSourcePoint(0, GetMuzzleLocation(), 0);

        // if hit does not occur
        if (!HitResult.bBlockingHit)
        {
            FVector TargetLocation = GetMuzzleLocation() + User->GetFirstPersonCameraComponent()->GetForwardVector() * HitRange;
            BeamComponent->SetBeamTargetPoint(0, TargetLocation, 0);
        }
        else
        {
            // if hit occurs, handle beam
            BeamComponent->SetBeamTargetPoint(0, HitResult.ImpactPoint, 0);
        }

        // repeat fire sound
        if (!SoundComponent->IsPlaying())
        {
            SoundComponent->Play();
        }
    }
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLWeaponLightningGun::SpawnLightning()
{
    AQLCharacter* User = GetWeaponManager()->GetUser();

    if (User == nullptr)
    {
        return;
    }

    FHitResult HitResult = User->RayTraceFromCharacterPOV(HitRange);

    // if hit does not occur
    if (!HitResult.bBlockingHit)
    {
        // do sth
        QLUtility::Log("AQLWeaponLightningGun: no hit");
        return;
    }

    // if hit occurs, check the hit actor
    auto* hitActor = Cast<AQLCharacter>(HitResult.GetActor());
    if (!hitActor)
    {
        // do sth
        QLUtility::Log("AQLWeaponLightningGun: does not hit AQLCharacter");
        return;
    }

    // if self-hurt
    if (hitActor == User)
    {
        // do sth
        QLUtility::Log("AQLWeaponLightningGun: only hit player himself");
        return;
    }

    // create a damage event
    const FPointDamageEvent DamageEvent;

    float DamageAmount = BasicDamageAdjusted;
    DamageAmount = hitActor->TakeDamage(DamageAmount, DamageEvent, User->GetController(), this);

    // change victim velocity
    UCharacterMovementComponent* CharacterMovementComponent = hitActor->GetCharacterMovement();
    if (CharacterMovementComponent)
    {
        CharacterMovementComponent->AddImpulse(
            -HitResult.ImpactNormal * KnockbackSpeedChange, // impulse vector
            true); // velocity change (true) or impulse (false)
    }

    // display damage
    AQLPlayerController* QLPlayerController = User->GetQLPlayerController();
    if (DamageAmount > 0.0f && QLPlayerController)
    {
        PlaySoundFireAndForget("Hit");
        QLPlayerController->ShowDamageOnScreen(DamageAmount, HitResult.ImpactPoint);
    }
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLWeaponLightningGun::StopFire()
{
    // stop firing
    if (bIsFireHeld)
    {
        OnFireRelease();
    }
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLWeaponLightningGun::SetDamageMultiplier(const float Value)
{
    Super::SetDamageMultiplier(Value);

    BasicDamageAdjusted = Value * BasicDamage;
}