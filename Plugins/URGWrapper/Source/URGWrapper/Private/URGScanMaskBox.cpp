#include "URGScanMaskBox.h"
#include "DrawDebugHelpers.h"

AURGScanMaskBox::AURGScanMaskBox() : AURGScanMask()
{
	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	Box->SetupAttachment(Root);

	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


bool AURGScanMaskBox::IsInside(const FVector &In)
{
	// test rotation , test scale

	FVector LocalPoint = Box->GetComponentTransform().InverseTransformPosition(In);
	FVector Extent = Box->GetUnscaledBoxExtent();

	// since it's local position, we can just do it from origin
	if (LocalPoint.X > Extent.X || LocalPoint.X < -Extent.X)
		return false;

	if (LocalPoint.Y > Extent.Y || LocalPoint.Y < -Extent.Y)
		return false;

	if (LocalPoint.Z > Extent.Z || LocalPoint.Z < -Extent.Z)
		return false;

	return true;
}