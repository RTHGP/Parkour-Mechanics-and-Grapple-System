// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SPlayer.generated.h"

class UBoxComponent;
class UMoveComponent;
class UGrappleComonent;
class UCameraComponent;
class UCapsuleComponent;
class USpringArmComponent;
class USkeletalMeshComponent;

UCLASS()
class PARKOUR_API ASPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASPlayer();

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
		UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
		USkeletalMeshComponent* SKMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
		USpringArmComponent* SpringArm;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
		UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
		UBoxComponent* CheckComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
		UMoveComponent* MovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
		UGrappleComonent* GrappleComponent;

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor
			, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UMoveComponent* GetMoveComponent() const { return MovementComponent; }

	UGrappleComonent* GetGrappleComponent() const { return GrappleComponent; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	int jumpCount = 0;

	float zRot = 0.f;

	float yRot = 0.f;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void Turn(float Value);

	void LookUp(float Value);

	void StartJump();

	void BeginSprint();

	void EndSprint();

	void BeginClimb();

	void BeginGrapple();
};
