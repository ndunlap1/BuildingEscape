// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Engine/Public/DrawDebugHelpers.h"

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

	UE_LOG(LogTemp, Warning, TEXT("Grabber reporting for duty"));
	// ...

	/// Look for attached Physics handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Physics Handle is found"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: No Physics Handle found on %s"), *GetOwner()->GetName());
	}
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Input Component is found"));
		
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



void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab is pressed"));
}

void UGrabber::Released()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab is released"));
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Get Player view point this tick
	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewLocation, OUT PlayerViewRotation);

	//UE_LOG(LogTemp, Warning, TEXT("Player location: %s"), *PlayerViewLocation.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("Player view rotation: %s"), *PlayerViewRotation.ToString());
	
	FVector LineTraceEnd = PlayerViewLocation + (PlayerViewRotation.Vector() * Reach);

	/// Draw a red trace in the world to visualize
	DrawDebugLine(
		GetWorld(),
		PlayerViewLocation,
		LineTraceEnd,
		FColor(255, 0, 0),
		false,
		0.f,
		0.f,
		10
	);

	/// Setup query paramaters
	FHitResult Hit;
	FCollisionQueryParams TraceParamaters(FName(TEXT("")), false, GetOwner());

	/// Line-trace (AKA Ray-cast) out to reach distance
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParamaters
	);

	AActor* HitActor = Hit.GetActor();

	if (HitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Line Trace is hitting: %s "), *(HitActor->GetName()));
	}

	


	// See what we hit

	// ...
}

