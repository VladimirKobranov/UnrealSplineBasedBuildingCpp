//Copyright Vk
//https://github.com/VladimirKobranov
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
	TArray<FVector> OutPoints;
	USplineComponent* SplineComp;

private:
	double ModuleHeight;
	TArray<int> SetBalconyLocations;

	UFUNCTION(BlueprintCallable)
	void SpawnFunction(TArray<UStaticMesh*> Windows, UStaticMesh* Corner, TArray<UStaticMesh*> BalconyWindow,
	                   TArray<UStaticMesh*> Balcony, TArray<UStaticMesh*> Entrance,
	                   TArray<UStaticMesh*> BalconyAccessories, UStaticMesh* Roof,
	                   TArray<UStaticMesh*> WindowsAccessories, TArray<UStaticMesh*> Pipe, USplineComponent* Spline,
	                   USplineComponent* Spline_Segmented, const int Seed, FVector HeightVector,
	                   int BalconyAccessoriesPercentage, int WallAccessoriesPercentage, TArray<int> BalconyLocations,
	                   TArray<int> BalconyLocationsSides, TArray<int> EntraceLocations,
	                   TArray<int> EntraceLocationsSides);
	void SpawnArrayMesh(TArray<UStaticMesh*> SelectedMesh, FTransform Transform, int N);
	void SpawnMesh(UStaticMesh* SelectedMesh, FTransform Transform);
	UFUNCTION(BlueprintCallable)
	UDynamicMesh* SpawnCap(UDynamicMesh* DynamicMesh, FGeometryScriptPrimitiveOptions PrimitiveOptions,
	                       UGeometryScriptDebug* Debug, USplineComponent* Spline,
	                       FTransform Transform);
	UFUNCTION(BlueprintCallable)
	void SetDefaultValues(FVector HeightVector, UStaticMesh* Wall, UStaticMesh* Roof, FTransform& Transform);
};
