#pragma once

#include "CoreMinimal.h"
#include "CEElementalProjectiles.h"
#include "GameFramework/Actor.h"
#include "CEMetalSphere.generated.h"

UENUM()
enum class ECEMetalSphereStaticMesh
{
	Sphere
};

UENUM()
enum class ECEMetalSphereParticleSystem
{
	Smoke,
	SteamPuff,
	WaterDrips
};

UENUM()
enum class ECEMetalSphereMaterial
{
	MetalInteractive
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACEMetalSphere : public AActor, public ICEElementalProjectiles
{
	GENERATED_BODY()

	UPROPERTY()
	USceneComponent* Base;

	UPROPERTY()
	UStaticMeshComponent* MetalSphere;

	UPROPERTY()
	UParticleSystemComponent* Smoke;

	UPROPERTY()
	UParticleSystemComponent* Drips;

	float Burntness;

	float Wetness;

	float Heat;

	float DeltaSeconds;

	UPROPERTY()
	UMaterialInstanceDynamic* MetalMaterial;

	UPROPERTY()
	TMap<ECEMetalSphereMaterial, UMaterialInterface*> Materials;

	UPROPERTY()
	TMap<ECEMetalSphereParticleSystem, UParticleSystem*> ParticleSystems;

	UPROPERTY()
	TMap<ECEMetalSphereStaticMesh, UStaticMesh*> StaticMeshes;

private:
	void InitializeAssets();

	void InitializeMaterials();

	void InitializeParticleSystems();

	void InitializeStaticMeshes();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	ACEMetalSphere();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ElementalDamage(float HeatDamage, float WaterDamage) override;
};
