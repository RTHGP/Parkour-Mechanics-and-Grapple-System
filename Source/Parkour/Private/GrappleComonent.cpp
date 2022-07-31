// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleComonent.h"
#include "SPlayer.h"
#include "MoveComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGrappleComonent::UGrappleComonent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrappleComonent::BeginPlay()
{
	Super::BeginPlay();
	MainPlayer = Cast<ASPlayer>(GetOwner());
	if(MainPlayer)
	PlayerMoveComponent = Cast<UMoveComponent>(MainPlayer->GetMoveComponent());
}


// Called every frame
void UGrappleComonent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bStartGrapple && MainPlayer)
		StartGrapple(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("%d"), bStartGrapple);
}

void UGrappleComonent::GetGrapplePoint()
{
	if (GetOwner())
	{
		FHitResult GrappleHit;
		FVector Start = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
		FVector End = Start + GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetActorForwardVector() * Range;
		GetWorld()->LineTraceSingleByChannel(GrappleHit, Start, End, ECollisionChannel::ECC_Camera);
		if (GrappleHit.bBlockingHit)
		{
			GrapplePoint = GrappleHit.Location;
			GrappleHitNormal = GrappleHit.Normal;
			bStartGrapple = true;
		}
		DrawDebugLine(GetWorld(), GrappleHit.TraceStart, GrappleHit.TraceEnd, FColor::Red, false, 2.f);
	}
}

void UGrappleComonent::SetGrapple(bool Value)
{
	bStartGrapple = Value;
}

bool UGrappleComonent::GetGrappleValue()
{
	return bStartGrapple;
}

void UGrappleComonent::StartGrapple(float DeltaTime) //set state to grappling
{	
	FHitResult MovementHit;
	FVector CurrentLocation = GetOwner()->GetActorLocation();
	//FVector CameraDirection = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetActorForwardVector();
	FVector DeltaVector = (GrapplePoint - CurrentLocation);
	DeltaVector.Normalize();
	GetOwner()->SetActorLocation(CurrentLocation + (DeltaVector /*+ CameraDirection */ + FVector(0.f, 0.f, 0.5f)) * Speed * DeltaTime, true, &MovementHit);

	float GrappleAngle = FVector::DotProduct(GrappleHitNormal, -DeltaVector);
	//UE_LOG(LogTemp, Warning, TEXT("%f"), GrappleAngle);
	FVector Distance = GrapplePoint - CurrentLocation;
	if (Distance.Size() < 10.f || MovementHit.bBlockingHit || GrappleAngle < 0.f)
	{
		//GetOwner()->SetActorLocation(CurrentLocation + FVector(100.f, 0.f, 100.f)); //optional
		PlayerMoveComponent->SetGravity(true);
		bStartGrapple = false;
	}
}

