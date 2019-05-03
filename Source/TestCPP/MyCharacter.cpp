// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Create a dummy root component we can attach things to.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	// Create a camera and a visible object
	UCameraComponent* OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));
	OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
	// Attach our camera and visible object to our root component. Offset and rotate the camera.
	OurCamera->SetupAttachment(RootComponent);
	OurCamera->SetRelativeLocation(FVector(-250.0f, 0.0f, 250.0f));
	OurCamera->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	OurVisibleComponent->SetupAttachment(RootComponent);

	//Reserve space in a dynamic array to avoid frequent allocation and resizing while object pooling is on
	toReuse = false;
	if (toReuse)
	{
		SpawnedActorArray.Reserve(400);
		ObjectPool.Reserve(400);
	}	
}

void AMyCharacter::Spawn()
{

	//Add malloc counts here
	//Add float time calculation every time the function is called.

	AMyActor *tempActor;
	//bool toReuse = false;
	UE_LOG(LogTemp, Warning, TEXT("ToReuse: %d "), toReuse);


	//ToSpawn is the actor to be spawned
	if (ToSpawn)
	{
		/*The World is the top level object representing a map or a sandbox in which Actors and 
		Components will exist and be rendered. GetWorld() gets the pointer to this top level.
		*/
		UWorld* world = GetWorld();

		if (world)
		{
			//Struct of optional parameters passed to SpawnActor function
			FActorSpawnParameters spawnParameters;
			spawnParameters.Owner = this;

			//Location and rotation data to be assigned to the dynamically spawned actor
			FRotator rotator(0.0,0.0,0.0);
			FVector spawnLocation(0, 0, 100);

			int sign = FMath::FRandRange(1, 4);
			FVector randVector;

			randVector.X = FMath::FRandRange(-1*(spawnLocation.X + SpawnedActorArray.Num()/10), spawnLocation.X + SpawnedActorArray.Num()/10);
			randVector.Y = FMath::FRandRange(-1*(spawnLocation.Y + SpawnedActorArray.Num()/10), spawnLocation.Y + SpawnedActorArray.Num()/10);
			randVector.Z = FMath::FRandRange(spawnLocation.Z, spawnLocation.Z + SpawnedActorArray.Num()/10);

			//Set the position on spawn location wrt actor
			if (sign % 2 == 0)
				randVector *= -1;

			spawnLocation = randVector;
			UE_LOG(LogTemp, Warning, TEXT("Count: %d"), SpawnedActorArray.Num());

			if (!toReuse)
			{
				//Call the SpawnActor function and pass the parameters
				tempActor = world->SpawnActor<AMyActor>(ToSpawn, spawnLocation, rotator, spawnParameters);
				SpawnedActorArray.Add(tempActor);
				UE_LOG(LogTemp, Warning, TEXT("Spawn only from memory"));
			}
			else
			{
				//If object pool is turned on and objects are available in the pool get from pool
				if (ObjectPool.Num() > 0)
				{
					UE_LOG(LogTemp, Warning, TEXT("Generating from pool"));
				
					tempActor = ObjectPool.Pop(false);
					tempActor->SetActorEnableCollision(true);
					tempActor->SetActorHiddenInGame(false);

					tempActor->SetActorLocationAndRotation(spawnLocation, rotator);
					SpawnedActorArray.Add(tempActor);
				}
				else
				{
					//If object pool is turned on and objects are not in pool, then spawn from memory
					UE_LOG(LogTemp, Warning, TEXT("Spawning from memory"));
					tempActor = world->SpawnActor<AMyActor>(ToSpawn, spawnLocation, rotator, spawnParameters);
					SpawnedActorArray.Add(tempActor);					
				}
				
			}
			
			//Disable or destroy objects if the count goes beyond 300 objects
			if (world && SpawnedActorArray.Num() > 300)
			{				
				while (SpawnedActorArray.Num() >= 2)
				{	

				/*	ObjectPool.Push(SpawnedActorArray.Pop(false));
					ObjectPool[ObjectPool.Num() - 1]->SetActorHiddenInGame(true);
					ObjectPool[ObjectPool.Num() - 1]->SetActorEnableCollision(false);*/
										
					if (!toReuse)
					{
						UE_LOG(LogTemp, Warning, TEXT("Destroying"));
						SpawnedActorArray.Pop(true)->Destroy();
						GetWorld()->ForceGarbageCollection(false);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("pushing to pool"));
						ObjectPool.Push(SpawnedActorArray.Pop(false));
						ObjectPool[ObjectPool.Num() - 1]->SetActorHiddenInGame(true);
						ObjectPool[ObjectPool.Num() - 1]->SetActorEnableCollision(false);
					}
					
				}
			}

		}
	}
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();	

	FTimerHandle UnusedHandle;
	//GetWorldTimerManager().SetTimer(UnusedHandle, this, &AMyActor::TimerElapsed, TimerDelay, false);
	
	//GetWorldTimerManager().SetTimer(UnusedHandle, this, AMyCharacter::Spawn(), 1.0f, true, 2.0f);
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Spawn();	

	/*Handle movement based on our "MoveX" and "MoveY" axes
	{
		if (!CurrentVelocity.IsZero())
		{
			FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
			SetActorLocation(NewLocation);
		}
	}
	*/
}

void AMyCharacter::Move_XAxis(float AxisValue)
{
	// Move at 100 units per second forward or backward
	CurrentVelocity.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.0f;
}

void AMyCharacter::Move_YAxis(float AxisValue)
{
	// Move at 100 units per second right or left
	CurrentVelocity.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.0f;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

//	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Respond every frame to the values of our two movement axes, "MoveX" and "MoveY".
	InputComponent->BindAxis("MoveX", this, &AMyCharacter::Move_XAxis);
	InputComponent->BindAxis("MoveY", this, &AMyCharacter::Move_YAxis);

	InputComponent->BindAction("Jump", IE_Pressed, this, &AMyCharacter::LaunchBullet);
	//InputComponent->BindAction("Jump", IE_Released, this, &AMyCharacter::StopJumping);

	InputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyCharacter::LookUpAtRate);
}

void AMyCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMyCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMyCharacter::LaunchBullet()
{
	if (BulletActor)
	{
		UWorld* world = GetWorld();
		if (world)
		{
			FActorSpawnParameters spawnParameters;
			spawnParameters.Owner = this;
			FRotator rotator(0.0, 0.0, 0.0);
			FVector spawnLocation(0, 0, 100);
			world->SpawnActor<ABulletActor>(BulletActor, spawnLocation, rotator, spawnParameters);
		}
	}
}