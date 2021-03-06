//------------------------------------------------------------
// Quarter Life
//
// GNU General Public License v3.0
//
//  (\-/)
// (='.'=)
// (")-(")o
//------------------------------------------------------------


#include "QLPowerup.h"
#include "QLPlayerController.h"
#include "QLCharacter.h"
#include "QLUmgFirstPerson.h"
#include "QLPowerupManager.h"
#include "Components/SphereComponent.h"

//------------------------------------------------------------
//------------------------------------------------------------
AQLPowerup::AQLPowerup()
{
    bCanBeRespawned = true;
    RespawnInterval = 120.0f;
    EffectDuration = 30.0f;
    ProgressUpdateTimeIncrement = 0.1f;
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLPowerup::PostInitializeComponents()
{
    Super::PostInitializeComponents();
}

//------------------------------------------------------------
//------------------------------------------------------------
float AQLPowerup::GetProgressPercent()
{
    return ProgressPercent;
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLPowerup::OnComponentBeginOverlapImpl(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor)
    {
        AQLCharacter* QLCharacter = Cast<AQLCharacter>(OtherActor);
        if (QLCharacter)
        {
            Beneficiary = QLCharacter;

            if (!Beneficiary.IsValid())
            {
                return;
            }

            bool bSuccessful = Beneficiary->AddPowerup(this);

            if (!bSuccessful)
            {
                return;
            }

            PlaySoundFireAndForget("PickUp");

            PowerUpPlayer();

            Deactivate();

            TimeElapsed = 0.0f;

            // until the next respawn
            GetWorldTimerManager().SetTimer(RespawnTimerHandle,
                this,
                &AQLPowerup::Reactivate,
                1.0f, // time interval in second
                false, // loop
                RespawnInterval); // delay in second

            // once the effect starts, periodically update
            GetWorldTimerManager().SetTimer(EffectStartTimerHandle,
                this,
                &AQLPowerup::UpdateProgressOnUMG,
                ProgressUpdateTimeIncrement, // time interval in second
                true, // loop
                0.0f); // delay in second

            // once the effect ends
            GetWorldTimerManager().SetTimer(EffectEndTimerHandle,
                this,
                &AQLPowerup::OnEffectEnd,
                1.0f, // time interval in second
                false, // loop
                EffectDuration); // delay in second
        }
    }
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLPowerup::PowerUpPlayer()
{
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLPowerup::Reactivate()
{
    RootSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AQLPowerup::OnComponentBeginOverlapImpl);

    if (DynamicMaterial.IsValid())
    {
        DynamicMaterial->SetScalarParameterValue("GlowIntensity", 5.0f);
    }

    GetWorldTimerManager().ClearTimer(RespawnTimerHandle);
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLPowerup::Deactivate()
{
    // temporarily remove delegate until next time quad shows up
    RootSphereComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AQLPowerup::OnComponentBeginOverlapImpl);

    if (DynamicMaterial.IsValid())
    {
        DynamicMaterial->SetScalarParameterValue("GlowIntensity", 0.1f);
    }
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLPowerup::OnEffectEnd()
{
    GetWorldTimerManager().ClearTimer(EffectEndTimerHandle);
    GetWorldTimerManager().ClearTimer(EffectStartTimerHandle);

    if (Beneficiary.IsValid())
    {
        Beneficiary->RemovePowerup(this);
        UpdateProgressOnUMGInternal(0.0f);
    }
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLPowerup::UpdateProgressOnUMGInternal(const float Value)
{

}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLPowerup::UpdateProgressOnUMG()
{
    TimeElapsed += ProgressUpdateTimeIncrement;
    ProgressPercent = 1.0f - TimeElapsed / EffectDuration;

    ProgressPercent = ProgressPercent < 0.0f ? 0.0f : ProgressPercent;
    ProgressPercent = ProgressPercent > 1.0f ? 1.0f : ProgressPercent;
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLPowerup::SetUMGVisibility(const bool bFlag)
{
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLPowerup::SetPowerupManager(UQLPowerupManager* PowerupManagerExt)
{
    PowerupManager = PowerupManagerExt;
}

//------------------------------------------------------------
//------------------------------------------------------------
FName AQLPowerup::GetPowerupName()
{
    return PowerupName;
}