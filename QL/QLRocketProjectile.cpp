//------------------------------------------------------------
// Quarter Life
//
// GNU General Public License v3.0
//
//  (\-/)
// (='.'=)
// (")-(")o
//------------------------------------------------------------


#include "QLRocketProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "QLUtility.h"
#include "QLCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

//------------------------------------------------------------
// Sets default values
//------------------------------------------------------------
AQLRocketProjectile::AQLRocketProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RocketLifeSpan = 5.0f;
    RocketSpeed = 2000.0f;

    RootSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootSphereComponent"));
    RootSphereComponent->InitSphereRadius(20.0f);
    RootSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    RootSphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    RootSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AQLRocketProjectile::OnBeginOverlapForComponent);
    RootSphereComponent->SetEnableGravity(false);
    RootComponent = RootSphereComponent;

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
    ProjectileMovementComponent->UpdatedComponent = RootSphereComponent;
    ProjectileMovementComponent->InitialSpeed = RocketSpeed;
    ProjectileMovementComponent->MaxSpeed = RocketSpeed;
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
    ProjectileMovementComponent->SetVelocityInLocalSpace(FVector(.0f, 0.0f, 0.0f));

    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    StaticMeshComponent->SetupAttachment(RootComponent);
    StaticMeshComponent->SetSimulatePhysics(false);
    StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    StaticMeshComponent->SetEnableGravity(false);

    ExplosionParticleSystem = nullptr;
    ExplosionSound = nullptr;
    BlastRadius = 400.0f;
    BlastSpeedChange = 1500.0f;
}

//------------------------------------------------------------
// Called when the game starts or when spawned
//------------------------------------------------------------
void AQLRocketProjectile::BeginPlay()
{
	Super::BeginPlay();

    ProjectileMovementComponent->InitialSpeed = RocketSpeed;
    ProjectileMovementComponent->MaxSpeed = RocketSpeed;

    SetLifeSpan(RocketLifeSpan);
}

//------------------------------------------------------------
// Called every frame
//------------------------------------------------------------
void AQLRocketProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLRocketProjectile::OnBeginOverlapForComponent(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Only add impulse and destroy projectile if we hit a physics
    if (OtherActor)
    {
        // query victims within the blast radius
        FVector Epicenter = GetActorLocation();
        TArray<FOverlapResult> OutOverlaps;
        FCollisionObjectQueryParams CollisionObjectQueryParams(ECollisionChannel::ECC_Pawn);
        FCollisionQueryParams CollisionQueryParams;

        GetWorld()->OverlapMultiByObjectType(OutOverlaps,
            Epicenter,
            FQuat(GetActorRotation()),
            CollisionObjectQueryParams,
            FCollisionShape::MakeSphere(BlastRadius),
            CollisionQueryParams);

        for (auto&& Result : OutOverlaps)
        {
            TWeakObjectPtr<UPrimitiveComponent> Comp = Result.Component;
            AActor* Actor = Comp->GetOwner();
            if (Actor)
            {
                AQLCharacter* Character = Cast<AQLCharacter>(Actor);
                if (Character)
                {
                    UCharacterMovementComponent*  CharacterMovementComponent = Character->GetCharacterMovement();
                    if (CharacterMovementComponent)
                    {
                        CharacterMovementComponent->AddRadialImpulse(
                            GetActorLocation(),
                            BlastRadius,
                            BlastSpeedChange,
                            ERadialImpulseFalloff::RIF_Linear,
                            true); // velocity change (true) or impulse (false)
                    }
                }
            }
        }


        //AQLCharacter* Character = Cast<AQLCharacter>(OtherActor);
        //if (Character)
        //{
        //    //Character->LaunchCharacter(FVector(0.0f, 0.0f, 100.0f), true, true);

        //    Character->GetCharacterMovement()->AddRadialImpulse(
        //        GetActorLocation(),
        //        100.0f, // Radius
        //        100.0f, // Strength
        //        ERadialImpulseFalloff::RIF_Linear,
        //        true); // velocity change (true) or impulse (false)
        //}

        Destroy();
    }
}

//------------------------------------------------------------
//------------------------------------------------------------
UProjectileMovementComponent* AQLRocketProjectile::GetProjectileMovementComponent()
{
    return ProjectileMovementComponent;
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLRocketProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (EndPlayReason == EEndPlayReason::Destroyed)
    {
        // play explosion particle system
        if (ExplosionParticleSystem)
        {
            FTransform Transform(FRotator::ZeroRotator,
                GetActorLocation(),
                FVector(4.0f)); // scale

            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
                ExplosionParticleSystem,
                Transform,
                true, // auto destroy
                EPSCPoolMethod::AutoRelease);
        }

        // play explosion sound
        if (ExplosionSound)
        {
            // fire and forget
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
        }
    }
}