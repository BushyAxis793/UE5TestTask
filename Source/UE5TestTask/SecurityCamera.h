// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SpotLightComponent.h"
#include "Components/BoxComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "UE5TestTaskCharacter.h"
#include "SecurityCamera.generated.h"

UCLASS()
class UE5TESTTASK_API ASecurityCamera : public AActor
{
	GENERATED_BODY()

		UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* CubeMesh;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* BallMesh;

	UPROPERTY(VisibleAnywhere)
		USpotLightComponent* SpotLight;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* CylinderMesh;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* ConeTrigger;

	UPROPERTY(VisibleAnywhere)
		UBoxComponent* CollisionCube;
	
	UPROPERTY(VisibleAnywhere)
	AUE5TestTaskCharacter* player;


	bool bisPlayerInVision = false;
	bool bIsTimeToMoveBack = false;
	FHitResult hit;

public:
	// Sets default values for this actor's properties
	ASecurityCamera();

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
