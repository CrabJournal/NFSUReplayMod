#include "stdafx.h"
#include "R_math.h"

// "Compress" matrix
void AlignedRotationMatrixToRotation(float Matrix[3][4], vector3* rotation) {
	rotation->x = SetCosSignTo(Matrix[1][0], Matrix[0][0]);		//  sin x * cos y and sign bit from cos x * cos y
	rotation->y = Matrix[2][0];									// -sin y
	rotation->z = SetCosSignTo(Matrix[2][1], Matrix[2][2]);		//  sin z * cos y and sign bit from cos z * cos y
}
void RotationToAlignedRotationMatrix(float Matrix[3][4], vector3* rotation) {
	float ucosY = GetSinCos(rotation->y);
	vector3a rbXZ  = (*rotation >> 1) << 1; // zeroing last bit of mantissa for x and z, do not use y
	vector3  sins = { FitIntoOne(rbXZ.x / ucosY), -rotation->y, FitIntoOne(rbXZ.y / ucosY) };
	vector3a coss = SetCosSignFromSrcSSE(*rotation, GetSinCosSSE(sins));

	float cosXsinY = coss.x * sins.y;
	float sinXsinZ = sins.x * sins.z;
	float sinXcosZ = sins.x * coss.z;

	Matrix[0][0] = coss.x * coss.y;
	Matrix[0][1] = cosXsinY * sins.z - sinXcosZ;
	Matrix[0][2] = cosXsinY * coss.z + sinXsinZ;
	Matrix[1][0] = rbXZ.x;
	Matrix[1][1] = sinXsinZ * sins.y + coss.x * coss.z;
	Matrix[1][2] = sinXcosZ * sins.y - coss.x * sins.z;
	Matrix[2][0] = rotation->y;
	Matrix[2][1] = rbXZ.z;
	Matrix[2][2] = coss.y * coss.z;
}