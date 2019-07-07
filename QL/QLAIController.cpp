//------------------------------------------------------------
// Quarter Life
//
// GNU General Public License v3.0
//
//  (\-/)
// (='.'=)
// (")-(")o
//------------------------------------------------------------


#include "QLAIController.h"
#include "Classes/BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "QLCharacter.h"
#include "Classes/Perception/AIPerceptionComponent.h"
#include "Classes/Perception/AISenseConfig.h"
#include "Classes/Perception/AISenseConfig_Sight.h"
#include "Classes/Perception/AISenseConfig_Hearing.h"
#include "Classes/Perception/AISenseConfig_Prediction.h"
#include "Classes/Perception/AISense.h"
#include "Classes/Perception/AISense_Sight.h"
#include "Classes/Perception/AISense_Hearing.h"
#include "Classes/Perception/AISense_Prediction.h"
#include "QLUtility.h"

//------------------------------------------------------------
//------------------------------------------------------------
AQLAIController::AQLAIController()
{
    // AI sense
    AISenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AISenseConfig_Sight"));
    AISenseConfig_Sight->PeripheralVisionAngleDegrees = 70.0f;

    AISenseConfig_Hearing = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("AISenseConfig_Hearing"));
    AISenseConfig_Prediction = CreateDefaultSubobject<UAISenseConfig_Prediction>(TEXT("AISenseConfig_Prediction"));

    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
    PerceptionComponent->ConfigureSense(*AISenseConfig_Sight);
    PerceptionComponent->ConfigureSense(*AISenseConfig_Hearing);
    PerceptionComponent->ConfigureSense(*AISenseConfig_Prediction);

    PerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());

    QLTeamId = FGenericTeamId(1);

    QLDetectEnemies = true;
    QLDetectFriendlies = true;
    QLDetectNeutrals = true;
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLAIController::BeginPlay()
{
    Super::BeginPlay();
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLAIController::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (PerceptionComponent)
    {
        PerceptionComponent->OnPerceptionUpdated.RemoveDynamic(this, &AQLAIController::OnPerceptionUpdatedImpl);
        PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AQLAIController::OnPerceptionUpdatedImpl);
    }

    if (AISenseConfig_Sight)
    {
        AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = QLDetectEnemies;
        AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = QLDetectFriendlies;
        AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = QLDetectNeutrals;
    }

    SetGenericTeamId(QLTeamId);
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BehaviorTreeBasic)
    {
        RunBehaviorTree(BehaviorTreeBasic);
    }

    auto* MyCharacter = Cast<AQLCharacter>(InPawn);
    if (MyCharacter)
    {
        // allow bots to aim up and down (change pitch)
        MyCharacter->InitializeBot();
    }
}


//------------------------------------------------------------
//------------------------------------------------------------
void AQLAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
    APawn* const MyPawn = GetPawn();
    if (MyPawn)
    {
        FRotator NewControlRotation = GetControlRotation();

        // Look toward focus
        const FVector FocalPoint = GetFocalPoint();
        if (FAISystem::IsValidLocation(FocalPoint))
        {
            NewControlRotation = (FocalPoint - MyPawn->GetPawnViewLocation()).Rotation();
        }
        else if (bSetControlRotationFromPawnOrientation)
        {
            NewControlRotation = MyPawn->GetActorRotation();
        }

        // This code snippet must be commented out in order for the bots to aim up and down
        // Don't pitch view unless looking at another pawn
        //if (NewControlRotation.Pitch != 0 && Cast<APawn>(GetFocusActor()) == nullptr)
        //{
        //    NewControlRotation.Pitch = 0.f;
        //}

        SetControlRotation(NewControlRotation);

        if (bUpdatePawn)
        {
            const FRotator CurrentPawnRotation = MyPawn->GetActorRotation();

            if (CurrentPawnRotation.Equals(NewControlRotation, 1e-3f) == false)
            {
                MyPawn->FaceRotation(NewControlRotation, DeltaTime);
            }
        }
    }
}

//------------------------------------------------------------
//------------------------------------------------------------
void AQLAIController::OnPerceptionUpdatedImpl(const TArray<AActor*>& UpdatedActors)
{
    for (auto&& Target : UpdatedActors)
    {
        FActorPerceptionBlueprintInfo Info;
        PerceptionComponent->GetActorsPerception(Target, Info);

        for (const auto& Stimulus : Info.LastSensedStimuli)
        {
            if (Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass()))
            {
                bool bSenseResult = Stimulus.WasSuccessfullySensed();
                if (bSenseResult)
                {
                    QLUtility::Log("ENTER");
                }
                else
                {
                    QLUtility::Log("LEAVE");
                }
            }
        }
    }
}