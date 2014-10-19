
#include <math.h>

void VECTOR3D::Normalize()
{
	float length=GetLength();

	if(length==1 || length==0)			//return if length is 1 or 0
		return;

	float scalefactor = 1.0f/length;
	x *= scalefactor;
	y *= scalefactor;
	z *= scalefactor;
}

VECTOR3D VECTOR3D::GetNormalized() const
{
	VECTOR3D result(*this);

	result.Normalize();

	return result;
}


VECTOR3D operator*(float scaleFactor, const VECTOR3D & rhs)
{
	return rhs*scaleFactor;
}

bool VECTOR3D::operator==(const VECTOR3D & rhs) const
{
	if(x==rhs.x && y==rhs.y && z==rhs.z)
		return true;

	return false;
}

