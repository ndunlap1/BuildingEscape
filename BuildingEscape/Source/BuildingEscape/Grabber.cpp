// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Engine/Public/DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	/// Look for attached Physics handle
	FindPhysicsComponent();
	SetupInputComponent();
	
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		// Set binding action for input to grab an object
		InputComponent->BindAction(
			"Grab",
			IE_Pressed,
			this,
			&UGrabber::Grab
		);
		// Set a binding action for input to release the object
		InputComponent->BindAction(
			"Grab",
			IE_Released,
			this,
			&UGrabber::Released
		);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: No InputComponent found on %s"), *GetOwner()->GetName());
	}
}

void UGrabber::FindPhysicsComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: No Physics Handle found on %s"), *GetOwner()->GetName());
	}
}


void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab is pressed"));

	/// LINE TRACE Try and reach any actors with physics body collison channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent(); // We are moving mesh with this.
	auto ActorHit = HitResult.GetActor();
	/// If we hit something then attach a Physics Handle
		
	if (ActorHit)
	{
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true //Allow Rotation
		);
	}
}

void UGrabber::Released()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab is released"));
	PhysicsHandle->ReleaseComponent(); 
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If physics handle is attached
	if (PhysicsHandle->GrabbedComponent)
	{
		// move the object that we are holding
		PhysicsHandle->SetTargetLocation(GetLineTraceEnd());
	}

	// ...
}

FVector UGrabber::GetLineTraceStart()
{
	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;
	// Get Player view point this tick
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewLocation, OUT PlayerViewRotation);

	// Calculate Line Trace End
	return PlayerViewLocation;
}


FVector UGrabber::GetLineTraceEnd()
{
	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;
	// Get Player view point this tick
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewLocation, OUT PlayerViewRotation);

	// Calculate Line Trace End
	return PlayerViewLocation + (PlayerViewRotation.Vector() * Reach);
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	
	/// Setup query paramaters
	FHitResult HitResult;
	FCollisionQueryParams TraceParamaters(FName(TEXT("")), false, GetOwner());
	
	/// Line-trace (AKA Ray-cast) out to reach distance
	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		GetLineTraceStart(),
		GetLineTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParamaters
	);

	return HitResult;
}
