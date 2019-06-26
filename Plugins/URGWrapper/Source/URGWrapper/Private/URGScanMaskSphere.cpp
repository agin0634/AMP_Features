#include "URGScanMaskSphere.h"
#include "DrawDebugHelpers.h"

AURGScanMaskSphere::AURGScanMaskSphere() : AURGScanMask()
{
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(Root);

	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


bool AURGScanMaskSphere::IsInside(const FVector &In)
{
	FVector Location = Sphere->GetComponentLocation();
	float R = Sphere->GetScaledSphereRadius();

	return FVector::DistSquared(Location, In) <= R * R;
}