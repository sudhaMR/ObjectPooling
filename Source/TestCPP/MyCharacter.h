#include "BulletActor.h"
#include "MyActor.h"
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class TESTCPP_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMyActor> ToSpawn;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABulletActor> BulletActor;

	TArray<AMyActor*> SpawnedActorArray;
	//TLinkedList<AMyActor*> SpawnedActorArray;
	TArray<AMyActor*> ObjectPool;
	//TLinkedList<AMyActor*> ObjectPool;
	int spawnedActorArraySize;
	int objectPoolSize;
	UFUNCTION(BlueprintCallable)
	void Spawn();

	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);

	UPROPERTY(EditAnywhere)
		USceneComponent* OurVisibleComponent;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(EditAnywhere, Category = "ObjectPooling", meta = (EditCondition = true))
		bool toReuse;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	FVector CurrentVelocity;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	void LaunchBullet();
};
