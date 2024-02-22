// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerCarPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HealthComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "InkRush/Player/PlayerPawnMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
// Sets default values
ABasePlayerCarPawn::ABasePlayerCarPawn()

{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoxComponent = CreateDefaultSubobject <UBoxComponent>(TEXT("Box Component"));
	BoxComponent->InitBoxExtent(FVector(50.f, 50.f, 50.f));
	SetRootComponent(BoxComponent);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	HealthComponent = CreateDefaultSubobject <UHealthComponent>(TEXT("Health Component"));
	HealthComponent->OnHealthBecomeZero.AddDynamic(this, &ABasePlayerCarPawn::HealthBecomeZero);

	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("Pawn Movement"));
	MovementComponent->UpdatedComponent = BoxComponent;

	CarForwardRight = CreateDefaultSubobject<USceneComponent>(TEXT("ForwardRight"));
	CarForwardRight->SetupAttachment(Mesh);
	CarForwardLeft = CreateDefaultSubobject<USceneComponent>(TEXT("ForwardLeft"));
	CarForwardLeft->SetupAttachment(Mesh);
	CarBackRight = CreateDefaultSubobject<USceneComponent>(TEXT("BackRight"));
	CarBackRight->SetupAttachment(Mesh);
	CarBackLeft = CreateDefaultSubobject<USceneComponent>(TEXT("BackLeft"));
	CarBackLeft->SetupAttachment(Mesh);
	
}

// Called when the game starts or when spawned
void ABasePlayerCarPawn::BeginPlay()
{
	Super::BeginPlay();
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(Default_KBM_MappingContext, 0);
		}
	}

	HalfXBox = BoxComponent->GetScaledBoxExtent().X;
	HalfYBox = BoxComponent->GetScaledBoxExtent().Y;

	//APlayerBaseController* PBC = GetController<APlayerBaseController>();
	//PBC->SetMaxHealth(HealthComponent->GetMaxHealth());
}

// Called every frame
void ABasePlayerCarPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FVector ActorLocation = GetActorLocation();

	const FVector ForwardRight = ActorLocation + GetActorForwardVector() * HalfXBox + GetActorRightVector() * HalfYBox;
	const FVector ForwardLeft =  ActorLocation + GetActorForwardVector() * HalfXBox - GetActorRightVector() * HalfYBox;
	const FVector BackRight =    ActorLocation - GetActorForwardVector() * HalfXBox + GetActorRightVector() * HalfYBox;
	const FVector BackLeft =     ActorLocation - GetActorForwardVector() * HalfXBox - GetActorRightVector() * HalfYBox;

	//Linetracebychannel

	FVector drawPos(0.0f);
	FColor drawColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f).ToFColor(true);
	float drawDuration = 0.0f;
	bool drawShadow = true;
	

	FVector DownVectorSuspension = -1.0 * GetActorUpVector() * SuspensionLength;

	const TArray<AActor*> ActorsToIgnore;
	
	FHitResult FRHit;
	float FRSuspentionRatio = 0.f;
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), ForwardRight, ForwardRight + DownVectorSuspension, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, FRHit, true))
	{
		FRSuspentionRatio = 1.0f - (FRHit.Distance / SuspensionLength);
	}	
	DrawDebugString(GetWorld(), ForwardRight, *FString::Printf(TEXT("[%f]"),  FRSuspentionRatio), NULL, drawColor, drawDuration, drawShadow);
	

	FHitResult FLHit;
	float FLSuspentionRatio = 0.f;
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), ForwardLeft, ForwardLeft + DownVectorSuspension, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, FLHit, true))
	{
		FLSuspentionRatio = 1.0f - (FLHit.Distance / SuspensionLength);
	}
	DrawDebugString(GetWorld(), ForwardLeft, *FString::Printf(TEXT("[%f]"),  FLSuspentionRatio), NULL, drawColor, drawDuration, drawShadow);


	FHitResult BRHit;
	float BRSuspentionRatio = 0.f;
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), BackRight, BackRight + DownVectorSuspension, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, BRHit, true))
	{
		BRSuspentionRatio = 1.0f - (BRHit.Distance / SuspensionLength);
	}
	DrawDebugString(GetWorld(), BackRight, *FString::Printf(TEXT("[%f]"), BRSuspentionRatio), NULL, drawColor, drawDuration, drawShadow);


	FHitResult BLHit;
	float BLSuspentionRatio = 0.f;
	
	if(UKismetSystemLibrary::LineTraceSingle(GetWorld(), BackLeft, BackLeft + DownVectorSuspension, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, BLHit, true))
	{ 
		BLSuspentionRatio = 1.0f - (BLHit.Distance / SuspensionLength);
	}
	DrawDebugString(GetWorld(), BackLeft, *FString::Printf(TEXT("[%f]"), BLSuspentionRatio), NULL, drawColor, drawDuration, drawShadow);

	const FVector SuspentionForce = GetActorUpVector() * SuspensionPower;

	//Apply forces
	BoxComponent->AddForceAtLocation(SuspentionForce * FRSuspentionRatio, ForwardRight + DownVectorSuspension);
	BoxComponent->AddForceAtLocation(SuspentionForce * FLSuspentionRatio, ForwardLeft + DownVectorSuspension);
	BoxComponent->AddForceAtLocation(SuspentionForce * BRSuspentionRatio, BackRight + DownVectorSuspension);
	BoxComponent->AddForceAtLocation(SuspentionForce * BLSuspentionRatio, BackLeft+ DownVectorSuspension);


	//const float length = BoxComponent->GetScaledBoxExtent().X;
	//const FVector ActorRight = ActorLocation + GetActorRightVector() * length;
	//const FVector ActorLeft = ActorLocation - GetActorRightVector() * length;
	//const FVector ActorForward = ActorLocation + GetActorForwardVector() * length;
	//const FVector ActorBack = ActorLocation - GetActorForwardVector() * length;
	DrawDebugSphere(GetWorld(), ForwardRight, 10, 10, FColor::Red, false, 2, 1, 1);
	DrawDebugSphere(GetWorld(), ForwardLeft, 10, 10, FColor::Blue, false, 2, 1, 1);
	DrawDebugSphere(GetWorld(), BackRight, 10, 10, FColor::Green, false, 2, 1, 1);
	DrawDebugSphere(GetWorld(), BackLeft, 10, 10, FColor::Magenta, false, 2, 1, 1);

	DrawDebugLine(GetWorld(),GetActorLocation(), GetActorLocation() + GetActorUpVector() * 100, FColor::Black, false, 2, 1, 1);
}

// Called to bind functionality to input
void ABasePlayerCarPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	Input->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ABasePlayerCarPawn::Move);
	Input->BindAction(RandomImpulseInputAction, ETriggerEvent::Triggered, this, &ABasePlayerCarPawn::RandomImpulse);
	//Input->BindAction(ShootInputAction, ETriggerEvent::Triggered, this, &ABasePlayerCarPawn::Shoot);
}

void ABasePlayerCarPawn::Move(const FInputActionInstance& Instance)
{

	// input is a Vector2D
	FVector2D MovementVector = Instance.GetValue().Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		UE_LOG(LogTemp, Warning, TEXT("[%s]"), *(MovementVector.X * GetActorRightVector() * TorquePower).ToString())
		BoxComponent->AddTorqueInRadians(FVector(0.f, 0.f, MovementVector.X * TorquePower));
		//AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABasePlayerCarPawn::RandomImpulse(const FInputActionInstance& Instance)
{
	UE_LOG(LogTemp,Warning, TEXT("Random Impulse"))
	BoxComponent->AddImpulse(GetActorUpVector() * 10000);
}



void ABasePlayerCarPawn::TakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy,
                                         FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
                                         const UDamageType* DamageType, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Player took damage"));

	HealthComponent->DecreaseHealth(Damage);

	/* Notify the player state of the updated health. */
	/*TObjectPtr<APlayerBaseController> PBC = GetController<APlayerBaseController>();
	if (PBC)
	{
		PBC->SetHealth(HealthComponent->GetCurrentHealth());
	}

	if (ImpactSoundCue)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ImpactSoundCue);
	}*/
}

void ABasePlayerCarPawn::HealthBecomeZero(AActor* OwnerActor)
{
	/* Controller handles player's death*/
	/*APlayerBaseController* PBC = GetController<APlayerBaseController>();
	if (PBC)
	{
		PBC->RecieveOnDeath();
	}*/
}

