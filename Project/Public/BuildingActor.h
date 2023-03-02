// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UDynamicMesh.h"
#include "Components/SplineComponent.h"
#include "GeometryActors/GeneratedDynamicMeshActor.h"
#include "GeometryScript/GeometryScriptTypes.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "BuildingActor.generated.h"


UCLASS()
class MYPROJECT_API ABuildingActor : public AGeneratedDynamicMeshActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABuildingActor();
	static FVector SplinePointRotation(int Count, const USplineComponent* Spline_0, int Size);

private:
	int Number;

	UFUNCTION(BlueprintCallable)
	void SpawnFunction(TArray<UStaticMesh*> Windows, UStaticMesh* Corner, TArray<UStaticMesh*> BalconyWindow,
	                   TArray<UStaticMesh*> Balcony, TArray<UStaticMesh*> Entrance,
	                   TArray<UStaticMesh*> BalconyAccessories, UStaticMesh* Roof,
	                   TArray<UStaticMesh*> WindowsAccessories, USplineComponent* Spline,
	                   USplineComponent* Spline_Segmented, const int Seed, FVector HeightVector,
	                   int BalconyAccessoriesPercentage, int WallAccessoriesPercentage);
	void SpawnArrayMesh(TArray<UStaticMesh*> SelectedMesh, FTransform Transform, int N);
	void SpawnMesh(UStaticMesh* SelectedMesh, FTransform Transform);
	UFUNCTION(BlueprintCallable)
	UDynamicMesh* SpawnCap(UDynamicMesh* DynamicMesh, FGeometryScriptPrimitiveOptions PrimitiveOptions,
	                       UGeometryScriptDebug* Debug, USplineComponent* Spline, float Height, bool bCapped,
	                       FTransform Transform);
	UFUNCTION(BlueprintCallable)
	void SetDefaultValues(FVector HeightVector, UStaticMesh* Wall, UStaticMesh* Roof, FTransform& Transform);
};
