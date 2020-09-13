#pragma once

#include "CoreMinimal.h"
#include "CEElementalProjectiles.h"
#include "GameFramework/Actor.h"
#include "CEWoodSphere.generated.h"

UENUM()
enum class ECEWoodSphereStaticMesh
{
	Sphere
};

UENUM()
enum class ECEWoodSphereParticleSystem
{
	Flames,
	Smoke,
    SteamPuff,
    WaterDrips
};

UENUM()
enum class ECEWoodSphereMaterial
{
	WoodInteractive
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACEWoodSphere : public AActor, public ICEElementalProjectiles
{
	GENERATED_BODY()

	UPROPERTY()
	USceneComponent* Base = nullptr;

	UPROPERTY()
	UStaticMeshComponent* WoodSphere = nullptr;

	UPROPERTY()
	UParticleSystemComponent* Fire = nullptr;

	UPROPERTY()
	UParticleSystemComponent* Smoke = nullptr;

	UPROPERTY()
	UParticleSystemComponent* Drips = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* WoodMaterial = nullptr;

	float Burntness = 0.0f;

	float Wetness = 0.0f;

	float Heat = 0.0f;

	UPROPERTY()
	TMap<ECEWoodSphereMaterial, UMaterialInterface*> Materials;

	UPROPERTY()
	TMap<ECEWoodSphereParticleSystem, UParticleSystem*> ParticleSystems;

	UPROPERTY()
	TMap<ECEWoodSphereStaticMesh, UStaticMesh*> StaticMeshes;

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
	ACEWoodSphere();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ElementalDamage(float HeatDamage, float WaterDamage) override;
};
