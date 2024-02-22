// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
}


void UHealthComponent::DecreaseHealth(float Damage)
{
	CurrentHealth -= Damage;

	if (CurrentHealth <= 0)
	{
		OnHealthBecomeZero.Broadcast(GetOwner());
	}
}

float UHealthComponent::GetMaxHealth()
{
	return this->MaxHealth;
}

float UHealthComponent::GetCurrentHealth()
{
	return CurrentHealth;
}

void UHealthComponent::ResetHealth()
{
	CurrentHealth = MaxHealth;
}