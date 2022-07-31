// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrappleComonent.generated.h"

class ASPlayer;
class UMoveComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PARKOUR_API UGrappleComonent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrappleComonent();

	UPROPERTY(EditDefaultsOnly, Category = "Grapple Settings")
		float Range = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Grapple Settings")
		float Speed = 500.f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	void GetGrapplePoint();

	void SetGrapple(bool Value);

	bool GetGrappleValue();

private:

	bool bStartGrapple;

	FVector GrapplePoint;

	FVector GrappleHitNormal;

	ASPlayer* MainPlayer;

	UMoveComponent* PlayerMoveComponent;

	void StartGrapple(float DeltaTime);
		
};
