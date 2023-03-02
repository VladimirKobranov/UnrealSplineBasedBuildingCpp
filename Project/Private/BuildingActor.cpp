#include "BuildingActor.h"
#include "SplineBuilding.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "UDynamicMesh.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "DynamicMeshEditor.h"
#include "Generators/SweepGenerator.h"
#include "ConstrainedDelaunay2.h"

using namespace UE::Geometry;
#define LOCTEXT_NAMESPACE "UGeometryScriptLibrary_MeshPrimitiveFunctions"

// Sets default values
ABuildingActor::ABuildingActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

FVector ABuildingActor::SplinePointRotation(const int Count, const USplineComponent* Spline_0, const int Size)
{
	return Spline_0->GetLocationAtDistanceAlongSpline(Count * Size, ESplineCoordinateSpace::Local);
}

//Spawn Array Mesh
void ABuildingActor::SpawnArrayMesh(TArray<UStaticMesh*> SelectedMesh, const FTransform Transform, const int N)
{
	UStaticMeshComponent* NewStaticMeshComp = Cast<UStaticMeshComponent>(
		AddComponentByClass(UStaticMeshComponent::StaticClass(), false, Transform, false));
	NewStaticMeshComp->SetStaticMesh(SelectedMesh[N]);
}

//Spawn single Mesh
void ABuildingActor::SpawnMesh(UStaticMesh* SelectedMesh, const FTransform Transform)
{
	UStaticMeshComponent* NewStaticMeshComp = Cast<UStaticMeshComponent>(
		AddComponentByClass(UStaticMeshComponent::StaticClass(), false, Transform, false));
	NewStaticMeshComp->SetStaticMesh(SelectedMesh);
}

//Main function
void ABuildingActor::SpawnFunction(TArray<UStaticMesh*> Windows, UStaticMesh* Corner,
                                   TArray<UStaticMesh*> BalconyWindow, TArray<UStaticMesh*> Balcony,
                                   TArray<UStaticMesh*> Entrance, TArray<UStaticMesh*> BalconyAccessories,
                                   UStaticMesh* Roof, TArray<UStaticMesh*> WindowsAccessories, USplineComponent* Spline,
                                   USplineComponent* Spline_Segmented, const int Seed, FVector HeightVector,
                                   int BalconyAccessoriesPercentage, int WallAccessoriesPercentage)
{
	const FBox ModuleBBox = Windows[0]->GetBoundingBox();
	const int ModuleXSize = (ModuleBBox.Max - ModuleBBox.Min).X;
	const int ModuleZSize = (ModuleBBox.Max - ModuleBBox.Min).Z;
	TArray<FVector> OutPoints;
	const int SplineSegments = Spline->GetNumberOfSplineSegments();

	//Stream Initialization
	const FRandomStream RandStream = FRandomStream(Seed);

	for (int SplineSegmentsIndex = 0; SplineSegmentsIndex < SplineSegments; SplineSegmentsIndex++)
	{
		Spline->ConvertSplineSegmentToPolyLine(SplineSegmentsIndex, ESplineCoordinateSpace::Local, 0, OutPoints);
		Spline_Segmented->SetSplinePoints(OutPoints, ESplineCoordinateSpace::Local, true);

		FTransform Trans;
		const float SplineLength = Spline_Segmented->GetSplineLength();
		const float Count = UKismetMathLibrary::FTrunc(SplineLength / ModuleXSize);
		const int ZCount = HeightVector.Z / ModuleZSize;

		for (int XIndex = 0; XIndex <= Count; XIndex++)
		{
			for (int ZIndex = 0; ZIndex <= ZCount; ZIndex++)
			{
				//Random values
				const int RandomNumber = RandStream.FRandRange(0, 100);
				const int MainWallMeshRandom = RandStream.FRandRange(0, Windows.Num());
				const int MainBalconyMeshRandom = RandStream.FRandRange(0, BalconyWindow.Num());
				const int BalconyMeshRandom = RandStream.FRandRange(0, Balcony.Num());
				const int EntranceMeshRandom = RandStream.FRandRange(0, Entrance.Num());
				const int BalconyAccessoriesMeshRandom = RandStream.FRandRange(0, BalconyAccessories.Num());
				const int WindowsAccessoriesMeshRandom = RandStream.FRandRange(0, WindowsAccessories.Num());

				float XRestValue = (SplineLength - Count * ModuleXSize);
				const float XScale = (SplineLength - Count * ModuleXSize - XRestValue / 2) / ModuleXSize;
				Trans.SetRotation(FQuat4d(FRotator3d(UKismetMathLibrary::MakeRotFromX(
					SplinePointRotation(XIndex + 1, Spline_Segmented, ModuleXSize) - SplinePointRotation(
						XIndex, Spline_Segmented, ModuleXSize)))));
				//Main walls
				if (XIndex != Count)
				{
					Trans.SetScale3D(FVector3d(1.0, 1.0, 1.0));
					Trans.SetLocation(FVector3d(
						(Spline_Segmented->GetLocationAtDistanceAlongSpline(XIndex * ModuleXSize + XRestValue / 2,
						                                                    ESplineCoordinateSpace::Local)).X,
						(Spline_Segmented->GetLocationAtDistanceAlongSpline(XIndex * ModuleXSize + XRestValue / 2,
						                                                    ESplineCoordinateSpace::Local)).Y,
						ZIndex * ModuleZSize));
					//First floor
					if (ZIndex == 0)
					{
						if (XIndex == 2 && SplineSegmentsIndex == 1)
						{
							SpawnArrayMesh(Entrance, Trans, EntranceMeshRandom);
						}
						else
						{
							SpawnArrayMesh(Windows, Trans, MainWallMeshRandom);
						}
					}
					//Roof
					else if (ZIndex == ZCount)
					{
						SpawnMesh(Roof, Trans);
					}
					//Main floors
					else
					{
						if (XIndex % 2 == 0)
						{
							SpawnArrayMesh(Windows, Trans, MainWallMeshRandom);
							if (RandomNumber < WallAccessoriesPercentage)
							{
								SpawnArrayMesh(WindowsAccessories, Trans, WindowsAccessoriesMeshRandom);
							}
						}
						else
						{
							SpawnArrayMesh(BalconyWindow, Trans, MainBalconyMeshRandom);
							SpawnArrayMesh(Balcony, Trans, BalconyMeshRandom);
							if (RandomNumber < BalconyAccessoriesPercentage)
							{
								SpawnArrayMesh(BalconyAccessories, Trans, BalconyAccessoriesMeshRandom);
							}
						}
					}
				}
				//Corners 
				if (XIndex == 0 || XIndex == Count)
				{
					if (XIndex == Count)
					{
						XRestValue = XRestValue / 2 * -1;
					}
					Trans.SetScale3D(FVector3d(XScale, 1.0, 1.0));
					Trans.SetLocation(FVector3d(
						(Spline_Segmented->GetLocationAtDistanceAlongSpline(XIndex * ModuleXSize - XRestValue,
						                                                    ESplineCoordinateSpace::Local)).X,
						(Spline_Segmented->GetLocationAtDistanceAlongSpline(
							XIndex * ModuleXSize - XRestValue,
							ESplineCoordinateSpace::Local)).Y, ZIndex * ModuleZSize));
					//Roof 
					if (ZIndex == ZCount)
					{
						SpawnMesh(Roof, Trans);
					}
					else
					//Main walls
					{
						SpawnMesh(Corner, Trans);
					}
				}
			}
		}
	}
}

void ABuildingActor::SetDefaultValues(FVector HeightVector, UStaticMesh* Wall, UStaticMesh* Roof, FTransform& Transform)
{
	const FBox ModuleBBox = Wall->GetBoundingBox();
	const int ModuleZSize = (ModuleBBox.Max - ModuleBBox.Min).Z;
	const int ZCount = HeightVector.Z / ModuleZSize;

	const FBox ModuleRoofBBox = Roof->GetBoundingBox();
	const int ModuleRoofZSize = (ModuleRoofBBox.Max - ModuleRoofBBox.Min).Z;

	const int OverallSize = ZCount * ModuleZSize + ModuleRoofZSize;
	Transform.SetLocation((FVector3d(0, 0, OverallSize)));
	Number = 3;
}

UDynamicMesh* ABuildingActor::SpawnCap(UDynamicMesh* DynamicMesh, FGeometryScriptPrimitiveOptions PrimitiveOptions,
                                       UGeometryScriptDebug* Debug, USplineComponent* Spline, float Height,
                                       bool bCapped, FTransform Transform)
{
	//const FTransform Trans;

	int SecondNumber = Number;
	TArray<FVector2d> PolygonVertices;
	const int SplinePointsNumber = Spline->GetNumberOfSplinePoints();

	for (int i = 0; i < SplinePointsNumber; i ++)
	{
		const FVector SplinePointLocation = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		PolygonVertices.AddUnique(TArray<TVector2<double>>::ElementType(SplinePointLocation.X, SplinePointLocation.Y));
	}

	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendSimpleExtrudePolygon(
		DynamicMesh, PrimitiveOptions, Transform, TArray<FVector2D>(PolygonVertices),
		Height, 0, bCapped, EGeometryScriptPrimitiveOriginMode::Base,
		Debug);
	{
		if (DynamicMesh == nullptr)
		{
			AppendError(Debug, EGeometryScriptErrorType::InvalidInputs,
			            LOCTEXT("AppendSimpleExtrudePolygon_NullMesh",
			                    "AppendSimpleExtrudePolygon: TargetMesh is Null"));
			return DynamicMesh;
		}
		if (PolygonVertices.Num() < 3)
		{
			AppendError(Debug, EGeometryScriptErrorType::InvalidInputs, LOCTEXT(
				            "AppendSimpleExtrudePolygon_InvalidPolygon",
				            "AppendSimpleExtrudePolygon: PolygonVertices array requires at least 3 positions"));
			return DynamicMesh;
		}

		FGeneralizedCylinderGenerator ExtrudeGen;
		for (const FVector2d& Point : PolygonVertices)
		{
			ExtrudeGen.CrossSection.AppendVertex(Point);
		}

		constexpr int32 NumDivisions = FMath::Max(1, 0 - 1);
		constexpr int32 NumPathSteps = NumDivisions + 1;
		const double StepSize = static_cast<double>(Height) / static_cast<double>(NumDivisions);

		for (int32 k = 0; k <= NumPathSteps; ++k)
		{
			const double StepHeight = (k == NumPathSteps) ? Height : (static_cast<double>(k) * StepSize);
			ExtrudeGen.Path.Add(FVector3d(0, 0, StepHeight));
		}

		ExtrudeGen.InitialFrame = FFrame3d();
		ExtrudeGen.bCapped = bCapped;
		ExtrudeGen.bPolygroupPerQuad = (PrimitiveOptions.PolygroupMode ==
			EGeometryScriptPrimitivePolygroupMode::PerQuad);
		ExtrudeGen.Generate();

		// FVector3d OriginShift = (Origin == EGeometryScriptPrimitiveOriginMode::Center) ? FVector3d(0, 0, -(Height/2)) : FVector3d::Zero();
		// AppendPrimitive(TargetMesh, &ExtrudeGen, Transform, PrimitiveOptions, OriginShift);
		return DynamicMesh;
	}
}