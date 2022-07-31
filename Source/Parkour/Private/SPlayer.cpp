// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayer.h"
#include "MoveComponent.h"
#include "GrappleComonent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ASPlayer::ASPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(FName("RootComp"));
	SetRootComponent(CapsuleComponent);

	SKMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("MeshComponent"));
	SKMesh->SetupAttachment(CapsuleComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArm"));
	SpringArm->SetupAttachment(CapsuleComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Camera->SetupAttachment(SpringArm);

	CheckComponent = CreateDefaultSubobject<UBoxComponent>(FName("CheckComp"));
	CheckComponent->SetupAttachment(CapsuleComponent);

	MovementComponent = CreateDefaultSubobject<UMoveComponent>(FName("MoveComponent"));
	GrappleComponent = CreateDefaultSubobject<UGrappleComonent>(FName("GrappleComponent"));

}

// Called when the game starts or when spawned
void ASPlayer::BeginPlay()
{
	Super::BeginPlay();
	CheckComponent->OnComponentBeginOverlap.AddDynamic(this, &ASPlayer::BeginOverlap);
	CheckComponent->OnComponentEndOverlap.AddDynamic(this, &ASPlayer::EndOverlap);
}

// Called every frame
void ASPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementComponent->GetPlayerState() == WallRunning)
		jumpCount = 0;

	//UE_LOG(LogTemp, Warning, TEXT("jumpCount: %d"), jumpCount);
}

// Called to bind functionality to input
void ASPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ASPlayer::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ASPlayer::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &ASPlayer::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ASPlayer::LookUp);

	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ASPlayer::StartJump);
	PlayerInputComponent->BindAction(FName("StartSprint"), IE_Pressed, this, &ASPlayer::BeginSprint);
	PlayerInputComponent->BindAction(FName("StartSprint"), IE_Released, this, &ASPlayer::EndSprint);
	PlayerInputComponent->BindAction(FName("Climb"), IE_Pressed, this, &ASPlayer::BeginClimb);
	PlayerInputComponent->BindAction(FName("Grapple"), IE_Pressed, this, &ASPlayer::BeginGrapple);
}

void ASPlayer::BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && MovementComponent)
	{	
		if (GrappleComponent->GetGrappleValue())
			GrappleComponent->SetGrapple(false);
		MovementComponent->SetPlayerState(OnGround);
		MovementComponent->ResetWallNormal();
		MovementComponent->SetJump(false);
		MovementComponent->SetGravity(false);
		jumpCount = 0;
	}
}

void ASPlayer::EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{	
	if (MovementComponent)
	{	
		MovementComponent->SetPlayerState(InAir);
		if(!GrappleComponent->GetGrappleValue())
		MovementComponent->SetGravity(true);
	}
}

void ASPlayer::MoveForward(float Value)
{
	if (MovementComponent)
	MovementComponent->ForwardVector = GetActorForwardVector() * Value;
}

void ASPlayer::MoveRight(float Value)
{
	if (MovementComponent)
	MovementComponent->RightVector = GetActorRightVector() * Value;
}

void ASPlayer::Turn(float Value)
{
	zRot += Value;
	SetActorRotation(FRotator(0.f, zRot, 0.f));
}

void ASPlayer::LookUp(float Value)
{
	yRot += Value;
	if (yRot >= 85.f)
		yRot = 85.f;
	else if (yRot <= -85.f)
		yRot = -85.f;

	SpringArm->SetWorldRotation(FRotator(yRot, zRot, 0.f));
}

void ASPlayer::StartJump()
{	
	if (jumpCount < 2 && MovementComponent)
	{	
		if (MovementComponent->GetPlayerState() == WallRunning)
		{
			MovementComponent->SetPrevVelocity(GetActorForwardVector()* 1000.f + FVector(0.f, 0.f, 1000.f));
			//jumpCount = 0;
			MovementComponent->ResetWallNormal();
		}
		else
		{
			MovementComponent->ResetPrevVelocity();
		}
		MovementComponent->SetPlayerState(InAir);
		MovementComponent->SetJump(true);
		MovementComponent->SetGravity(true);
		jumpCount++;
	} 
}

void ASPlayer::BeginSprint()
{
	if (MovementComponent)
	MovementComponent->StartSprint();
}

void ASPlayer::EndSprint()
{	
	if (MovementComponent)
	MovementComponent->StopSprint();
}

void ASPlayer::BeginClimb()
{
	if (MovementComponent)
		MovementComponent->StartClimb(true);
}

void ASPlayer::BeginGrapple()
{	
	if (GrappleComponent)
	{
		GrappleComponent->GetGrapplePoint();
		//MovementComponent->SetPlayerState(Grappling);
		MovementComponent->SetGravity(false);
		MovementComponent->SetJump(false);
	}
}
