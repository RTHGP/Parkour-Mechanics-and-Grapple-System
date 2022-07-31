// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveComponent.h"
#include "SPlayer.h"
#include "GrappleComonent.h"
#include "DrawDebugHelpers.h"
#include <time.h>

// Sets default values for this component's properties
UMoveComponent::UMoveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}



// Called when the game starts
void UMoveComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentSpeed = Speed;
	WallNormal = FVector(0.f);
	pState = InAir;
	MainPlayer = Cast<ASPlayer>(GetOwner());
	if(MainPlayer)
	PlayerGrappleComponent = Cast<UGrappleComonent>(MainPlayer->GetGrappleComponent());
	// ...
	
}


// Called every frame
void UMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UE_LOG(LogTemp, Warning, TEXT("%d"), bApplyGravity);

	if (bApplyGravity)
	{
		ApplyGravity(DeltaTime);
	}

	FVector DeltaVector = ForwardVector + RightVector;
	DeltaVector.Normalize();
	if (!DeltaVector.IsZero() && WallNormal.Size() <= 0.f) //disabled right movement while wallrunning!!
		StartMovement(DeltaVector, DeltaTime);

	if (bJump)
	{	
		if (WallNormal.Size() && GetPlayerState() == WallRunning) 
		SetJump(false);
		else
		{
			SetGravity(false);
			Jump(DeltaTime);
		}
	}

	if(pState == InAir || pState == WallRunning || pState == Grappling)
	CheckForWall();
	if (WallNormal.Size())
	{
		SetPlayerState(WallRunning);
		WallRunGravityMultiplier = 0.2f;
		SetGravity(false); //set true for real wallrun!
		StartSlideMovement(ForwardVector, WallNormal, DeltaTime);
	}
	else
		WallRunGravityMultiplier = 1.f;

	if (bBeginClimb)
		ClimbWall(DeltaTime);

	if (pState == OnGround)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnGround!"));
	}
	else if(pState == InAir)
	{
		UE_LOG(LogTemp, Warning, TEXT("InAir!"));
	}
	else if(pState == WallRunning)
	{
		UE_LOG(LogTemp, Warning, TEXT("WallRunning!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Grappling!"));
	}
}

FVector UMoveComponent::GetOwnerForwardVector()
{
	if (GetOwner())
		return GetOwner()->GetActorForwardVector();
	return FVector(0.f);
}

FVector UMoveComponent::GetOwnerPosition()
{
	return GetOwner()->GetActorLocation();
}

void UMoveComponent::StartMovement(FVector DeltaVector, float DeltaTime)
{
	FHitResult MovementHit;

	GetOwner()->AddActorWorldOffset(DeltaVector * CurrentSpeed * DeltaTime, true, &MovementHit);
	if (MovementHit.bBlockingHit)
		StartSlideMovement(DeltaVector, MovementHit.Normal, DeltaTime);
	
}

void UMoveComponent::StartSlideMovement(FVector DeltaVector, FVector Normal, float DeltaTime)
{
	FHitResult SlideHit;
	FVector SlideVector = FVector::VectorPlaneProject(DeltaVector, Normal);
	if (pState == WallRunning)
		SlideVector.Normalize();
	GetOwner()->AddActorWorldOffset(SlideVector * CurrentSpeed * DeltaTime, true, &SlideHit);
}

void UMoveComponent::ApplyGravity(float DeltaTime)
{	
	FVector GravityVector = FVector(0.f, 0.f, Gravity);
	GetOwner()->AddActorWorldOffset(DefaultGravityMultiplier * WallRunGravityMultiplier * GravityVector * DeltaTime * DeltaTime, true); // 1 more deltatime*
}

void UMoveComponent::SetPlayerState(PlayerState state)
{
	pState = state;
}

PlayerState UMoveComponent::GetPlayerState()
{
	return pState;
}

void UMoveComponent::SetGravity(bool Value)
{
	bApplyGravity = Value;
}

void UMoveComponent::SetJump(bool Value)
{
	bJump = Value;
}

void UMoveComponent::ResetPrevVelocity()
{
	PrevVelocity = FVector(0.f, 0.f, JumpVelocity);
}

void UMoveComponent::SetPrevVelocity(FVector NewVector)
{
	PrevVelocity = NewVector;
}

void UMoveComponent::StartSprint() //right sprint not fix while holding shift
{	
	if (!ForwardVector.IsNearlyZero() && ForwardVector.X > 0.f)
		CurrentSpeed = SpeedMultiplier * Speed;
}

void UMoveComponent::StopSprint()
{	
	CurrentSpeed = Speed;
}

void UMoveComponent::ResetWallNormal()
{
	WallNormal = FVector(0.f);
}

void UMoveComponent::StartClimb(bool Value)
{
	bBeginClimb = Value;
}

void UMoveComponent::Jump(float DeltaTime) 
{	
	FVector FinalVelocity = PrevVelocity + FVector(0.f, 0.f, JumpGravityMultiplier * Gravity) * DeltaTime;
	GetOwner()->AddActorWorldOffset(FinalVelocity * DeltaTime, true);
	PrevVelocity = FinalVelocity;
}

void UMoveComponent::CheckForWall()
{	
	FHitResult WallHit;
	FHitResult RightWallHit;
	FHitResult LeftWallHit;
	FVector Start{ GetOwner()->GetActorLocation() + FVector(0.f, 0.f, -50.f) };
	FVector RightEnd{ Start + GetOwner()->GetActorRightVector() * 60.f };
	FVector LeftEnd{ Start + GetOwner()->GetActorRightVector() * -60.f };

	GetWorld()->LineTraceSingleByChannel(RightWallHit, Start, RightEnd, ECollisionChannel::ECC_Camera);
	DrawDebugLine(GetWorld(), Start, RightEnd, FColor::Red, false, 2.f);

	GetWorld()->LineTraceSingleByChannel(LeftWallHit, Start, LeftEnd, ECollisionChannel::ECC_Camera);
	DrawDebugLine(GetWorld(), Start, LeftEnd, FColor::Blue, false, 2.f);

	if (WallNormal.Size() > 0.f)
	{
		WallNormal.Normalize();
		GetWorld()->LineTraceSingleByChannel(WallHit, Start, Start + WallNormal * -60.f, ECC_Camera);
		DrawDebugLine(GetWorld(), Start, Start + WallNormal * -60.f, FColor::Yellow, false, 2.f);

	}
	
	if (LeftWallHit.bBlockingHit && ForwardVector.Size() > 0.f)
	{	
		//UE_LOG(LogTemp, Warning, TEXT("Wall Normal!! : %s"), *WallNormal.ToString());
		WallNormal = LeftWallHit.Normal;
	}
	else if (RightWallHit.bBlockingHit && ForwardVector.Size() > 0.f)
	{
		WallNormal = RightWallHit.Normal;
	}
	else if (!WallHit.bBlockingHit || ForwardVector.Size() <= 0.f)
	{	
		SetPlayerState(InAir);
		ResetWallNormal();
		if(!PlayerGrappleComponent->GetGrappleValue())
		SetGravity(true);
	}
}

void UMoveComponent::ClimbWall(float DeltaTime)
{
	FHitResult CanClimb;
	FVector CCStart{ GetOwnerPosition() + FVector(0.f, 0.f, 150.f) };
	FVector CCEnd{ CCStart + GetOwnerForwardVector() * 100.f };

	FHitResult Climb;
	FVector Start{ GetOwnerPosition() + FVector(0.f, 0.f, -70.f) };
	FVector End{ Start + GetOwnerForwardVector() * 100.f };

	GetWorld()->LineTraceSingleByChannel(CanClimb, CCStart, CCEnd, ECC_Camera);
	DrawDebugLine(GetWorld(), CCStart, CCEnd, FColor::Red, false, 2.f);
	if (!CanClimb.bBlockingHit)
	{
		GetWorld()->LineTraceSingleByChannel(Climb, Start, End, ECC_Camera);
		DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 2.f);
		if (Climb.bBlockingHit)
		{	
			ResetPrevVelocity();
			GetOwner()->AddActorWorldOffset(FVector(0.f, 0.f, 1500.f)* DeltaTime, true);
		}
		else
		{
			bBeginClimb = false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can'tClimb!!"));
	}
}

