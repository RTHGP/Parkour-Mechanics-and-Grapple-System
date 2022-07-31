// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MoveComponent.generated.h"

enum PlayerState
{
	OnGround,
	InAir,
	WallRunning,
	Grappling,
};

class UGrappleComonent;
class ASPlayer;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PARKOUR_API UMoveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMoveComponent();

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
		float Gravity = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
		float DefaultGravityMultiplier = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
		float Speed = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
		float JumpVelocity = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
		float JumpGravityMultiplier = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
		float SpeedMultiplier = 1.5f;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	FVector ForwardVector;

	FVector RightVector;

	void SetPlayerState(PlayerState state);

	PlayerState GetPlayerState();

	void SetGravity(bool Value);

	void SetJump(bool Value);

	void ResetPrevVelocity();

	void SetPrevVelocity(FVector NewVector);

	void StartSprint();

	void StopSprint();

	void ResetWallNormal();

	void StartClimb(bool Value);

private:

	bool bApplyGravity = true;

	bool bJump = false;

	bool bBeginClimb = false;

	float CurrentSpeed = 0.f;

	float WallRunGravityMultiplier = 0.4f;

	PlayerState pState;

	ASPlayer* MainPlayer;

	UGrappleComonent* PlayerGrappleComponent;

	FVector PrevVelocity;

	FVector WallNormal;

	FVector PrevWallNormal;

	FVector GetOwnerForwardVector();

	FVector GetOwnerPosition();

	void StartMovement(FVector DeltaVector, float DeltaTime);
	
	void StartSlideMovement(FVector DeltaVector, FVector Normal, float DeltaTime);

	void ApplyGravity(float DeltaTime);

	void Jump(float DeltaTime);

	void CheckForWall();

	void ClimbWall(float DeltaTime);

};
