// Fill out your copyright notice in the Description page of Project Settings.


#include "SecurityCamera.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"

// Sets default values
ASecurityCamera::ASecurityCamera()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Camera Handler
	CylinderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CylinderMesh"));
	CylinderMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder"));

	if (CylinderVisualAsset.Succeeded())
	{
		CylinderMesh->SetStaticMesh(CylinderVisualAsset.Object);
		CylinderMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		CylinderMesh->SetWorldScale3D(FVector(0.05f, 0.05f, 0.25f));
		//CylinderMesh->SetWorldRotation(FRotator(-45.0f, 0.0f, 0.0f));
	}

	//Camera Body
	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	CubeMesh->SetupAttachment(CylinderMesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));

	if (CubeVisualAsset.Succeeded())
	{
		CubeMesh->SetStaticMesh(CubeVisualAsset.Object);
		CubeMesh->SetRelativeLocation(FVector(150.0f, 0.0f, 0.0f));
		//CubeMesh->SetWorldScale3D(FVector(0.25f));
		CubeMesh->SetRelativeScale3D(FVector(3.f, 3.f, 0.75f));
		CubeMesh->SetWorldRotation(FRotator(-45.0f, 0.0f, 0.0f));
	}

	//Camera Eye
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball"));
	BallMesh->SetupAttachment(CubeMesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));

	if (SphereVisualAsset.Succeeded())
	{
		BallMesh->SetStaticMesh(SphereVisualAsset.Object);
		BallMesh->SetRelativeLocation(FVector(50.0f, 0.0f, 20.0f));
		BallMesh->SetWorldScale3D(FVector(0.5f));
	}

	//Spot Light
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(BallMesh);
	SpotLight->SetRelativeLocation(FVector(50.0f, 0.0f, 50.0f));
	SpotLight->SetOuterConeAngle(25.f);
	SpotLight->SetLightColor(FLinearColor(255, 0, 0), true);
	SpotLight->SetIntensity(1000000.f);

	/*
	//Trigger
	ConeTrigger = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Trigger"));
	ConeTrigger->SetupAttachment(BallMesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cone.Shape_Cone"));

	if (ConeVisualAsset.Succeeded())
	{
		ConeTrigger->SetStaticMesh(ConeVisualAsset.Object);
		ConeTrigger->SetRelativeLocation(FVector((-500.f, 0.f, -500.f)));
		ConeTrigger->SetRelativeRotation(FRotator(447.f, 89.f, 446.f));
		ConeTrigger->SetWorldScale3D(FVector(80.f, 80.f, 80.f));

	}
	*/

	CollisionCube = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionCube->InitBoxExtent(FVector(3.0));
	CollisionCube->SetCollisionProfileName("Trigger");
	CollisionCube->SetupAttachment(CubeMesh);

	CollisionCube->OnComponentBeginOverlap.AddDynamic(this, &ASecurityCamera::OnOverlapBegin);
}

void ASecurityCamera::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		if (OtherActor->ActorHasTag(FName("Bullet")))
		{
			Destroy();
		}
	}
}

// Called when the game starts or when spawned
void ASecurityCamera::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASecurityCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*
FQuat Rotation = FQuat(FRotator(0.0, (FMath::ClampAngle(2, -45, 45)), 0.0));
AddActorLocalRotation(Rotation, false, 0, ETeleportType::None);
FMath::Sin(DeltaTime * 10);
*/

	if (!bisPlayerInVision)
	{
		auto cameraRotation = CylinderMesh->GetRelativeRotation();

		if (!bIsTimeToMoveBack)
		{
			CylinderMesh->SetRelativeRotation(FMath::Lerp(FQuat(cameraRotation), FQuat(FRotator(0.0f, -45.0f, 0.0f)), 0.01f));


		}

		if (bIsTimeToMoveBack)
		{
			CylinderMesh->SetRelativeRotation(FMath::Lerp(FQuat(cameraRotation), FQuat(FRotator(0.0f, 45.0f, 0.0f)), 0.01f));

		}

	}
	else
	{
		//Attack Mode
	}

}

