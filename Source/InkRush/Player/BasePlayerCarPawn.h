// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BasePlayerCarPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UHealthComponent;
class UBoxComponent;
class UInputMappingContext;
class UInputAction;
class USoundBase;
UCLASS()
class INKRUSH_API ABasePlayerCarPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABasePlayerCarPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Player|Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Player|Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Car|Placement")
	TObjectPtr<USceneComponent> CarForwardRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Car|Placement")
	TObjectPtr<USceneComponent> CarForwardLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Car|Placement")
	TObjectPtr<USceneComponent> CarBackRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Car|Placement")
	TObjectPtr<USceneComponent> CarBackLeft;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Car|Movement")
	float SuspensionLength = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Car|Movement")
	float SuspensionPower = 100000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Car|Movement")
	float TorquePower = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Player|Sound Cue")
	TObjectPtr<USoundBase> ImpactSoundCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Car|Movement")
	float MovementForcePower = 10000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Car|Movement")
	float MovementRightForcePower = 10000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Car|Movement")
	float MaxMovementSpeed = 20.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Player|Movement")
	TObjectPtr<UPawnMovementComponent> MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Player|Input")
	TObjectPtr<UInputMappingContext> Default_KBM_MappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Player|Input")
	TObjectPtr<UInputAction> MoveInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Player|Input")
	TObjectPtr<UInputAction> RandomImpulseInputAction;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Health")
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Visual|Particle")
	TObjectPtr<UParticleSystem> ExplosionParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Visual|Animation")
	float MeshRotationRate = 3.f;

	UPROPERTY()
	FVector PrevLocation;

	UPROPERTY()
	float InitMeshYaw;

	UFUNCTION()
	void Move(const FInputActionInstance& Instance);

	UFUNCTION(Server, Reliable)
	void ServerMove(const FVector2D& MovementVector);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastMove(const FVector2D& MovementVector);

	UFUNCTION()
	void RandomImpulse(const FInputActionInstance& Instance);

	UFUNCTION()
	virtual void TakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION()
	virtual void HealthBecomeZero(AActor* OwnerActor);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCarTickLogic();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastCarTickLogic();
private:
	UPROPERTY()
	double HalfXBox;
	double HalfYBox;

};
