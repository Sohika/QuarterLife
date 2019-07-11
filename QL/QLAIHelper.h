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
#include "QLAIHelper.generated.h"

class AQLCharacter;
class AQLCharacterHelper;

//------------------------------------------------------------
//------------------------------------------------------------
UCLASS()
class QL_API AQLAIHelper : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AQLAIHelper();

    UFUNCTION(BlueprintCallable, Category = "C++Function")
    void SetCharacterHelper(AQLCharacterHelper* CharacterHelperExt);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    virtual void PostInitializeComponents() override;

    //------------------------------------------------------------
    //------------------------------------------------------------
    UFUNCTION(BlueprintCallable, Category = "C++Function")
    void SpawnBots();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++Property")
    int NumBotsToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++Property")
    float SpawnRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++Property")
    TSubclassOf<AQLCharacter> CharacterClass;

    //------------------------------------------------------------
    //------------------------------------------------------------
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++Property")
    USphereComponent* RootSphereComponent;

    UPROPERTY()
    TWeakObjectPtr<AQLCharacterHelper> CharacterHelper;
};
