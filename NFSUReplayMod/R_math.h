#pragma once
#include <intrin.h>
#include <math.h>

#define IN
#define OUT


struct vector3 {
	float x, y, z;
	inline operator __m128() { return _mm_loadu_ps((float*)this); }
};
struct vector3a:vector3 {
	DWORD align;
	vector3a(__m128 t) { *this = *(vector3a*)&t; }
	vector3a(){ }
};


#if _M_IX86_FP >= 1 // SSE

inline float _sqrt(float F) {
	return *(float*)&(_mm_sqrt_ss(*(__m128*)&F));
}


inline __m128 operator|(__m128 a, __m128 b) {
	return _mm_or_ps(a, b);
}

inline __m128 operator&(__m128 a, __m128 b) {
	return _mm_and_ps(a, b);
}
inline __m128 operator&(__m128 a, DWORD b) {
	return _mm_and_ps(a, _mm_load_ps1((float*)&b));
}
inline __m128 operator&(DWORD a, __m128 b) {
	return b & a;
}

inline __m128 operator^(__m128 a, __m128 b) {
	return _mm_xor_ps(a, b);
}
inline __m128 operator^(__m128 a, DWORD b) {
	return _mm_xor_ps(a, _mm_load_ps1((float*)&b));
}

inline __m128 operator~(__m128 a) {
	return a^0xFFFF'FFFF;
}


inline __m128 operator+(__m128 a, __m128 b) {
	return _mm_add_ps(a, b);
}
inline __m128 operator-(__m128 a, __m128 b) {
	return _mm_sub_ps(a, b);
}
inline __m128 operator*(__m128 a, __m128 b) {
	return _mm_mul_ps(a, b);
}
inline __m128 operator/(__m128 a, __m128 b) {
	return _mm_div_ps(a, b);
}

inline __m128 operator*(__m128 a, float b) {
	return a * _mm_load_ps1(&b);
}
inline __m128 operator*(float a, __m128 b) {
	return b * a;
}


#if _M_IX86_FP >= 2 // SSE 2

inline __m128 operator>>(__m128 a, int imm8) {
	return _mm_castsi128_ps(_mm_srli_epi32(_mm_castps_si128(a), imm8));
}
inline __m128 operator<<(__m128 a, int imm8) {
	return _mm_castsi128_ps(_mm_slli_epi32(_mm_castps_si128(a), imm8));
}
inline __m128 abs(__m128 a) {
	return (a << 1) >> 1; // it's should be faster than load ss, shuffle and AND then
}

#else

inline __m128 abs(__m128 a) {
	return a & 0x7FFF'FFFF;
}

#endif // SSE 2

#else // no SSE

#define _sqrt sqrt

#endif



union int_float {
	DWORD i;
	float f;
};
#define ONLY_SIGN32(N) ( *(unsigned int*)&(N) >> 31 )
const float one = 1.0f;

inline float GetSinCosNormal(float cossin) {
	if (fabs(cossin) >= one)
		return 0;
	return _sqrt(one - (cossin * cossin));
}
inline __m128 GetSinCosNormalSSE(__m128 cossin) {
	__m128 ones = _mm_load_ps1(&one);
	__m128 ucossin = abs(cossin);
	__m128 res = _mm_cmple_ps(ucossin, ones); // 0xFFFF'FFFF if abs of cossin's element less or less-than-or-equal 1, else 0
	__m128 sincos = _mm_sqrt_ps(ones - (cossin * cossin));
	return sincos & res; // if cossin's element was >1, return 0
}

inline float GetSinCos(float cossin) {
	return _sqrt(one - (cossin * cossin));
}
inline __m128 GetSinCosSSE(__m128 cossin) {
	__m128 ones = _mm_load_ps1(&one);
	return _mm_sqrt_ps(ones - (cossin * cossin));
}


/*
 *	if val > 1
 *		return 1
 *	else
 *		if val < -1
 *			return -1
 *		else
 *			return val
 */
inline float FitIntoOne(float val) {
	if (fabs(val) <= 1) {
		return val;
	}
	else {
		int_float sv, _one, res;
		sv.f = val;
		_one.f = one;
		res.i = _one.i | (sv.i & 0x8000'0000);
		return res.f;
	}
}
inline __m128 FitIntoOneSSE(__m128 vals) {
	__m128 ones = _mm_load_ps1(&one);
	__m128 signs = vals >> 31;
	__m128 uvals = abs(vals);
	__m128 res = _mm_cmple_ps(vals, ones); // 0xFFFF'FFFF if abs of val less or equal 1, else 0
	return (ones & (~res))	// set 1.0 where values was >1 or <-1 else 0
		| (vals & res)		// zeroing all values that >1 or <-1, then bitwise OR from previous step
		| (signs << 31);	// and restore signs
}


inline float CosSigned(float Sin) {
	int_float sin, cos;
	sin.f = Sin;
	cos.f = GetSinCosNormal(Sin);
	cos.i |= sin.i << 31; // set sign from sin's lowest bit of mantissa
	return cos.f;
}

inline vector3a SetCosSignFromSrcSSE(vector3 src, vector3a ucoss) {
	return ucoss | (src << 31); // set sign from sin's lowest bit of mantissa
}

inline void CosSignedSSE(IN OUT vector3* Sins, OUT vector3a* Coss) {
	__m128 coss = GetSinCosSSE(*Sins);
	*Coss = SetCosSignFromSrcSSE(*Sins, coss);

	DWORD b = *(DWORD*)((float*)Sins + 3);
	*(vector3a*)Sins = (*Sins >> 1) << 1; // zeroing lowest bit of mantissa
	*(DWORD*)((float*)Sins + 3) = b;
}


inline float SetCosSignTo(float sin, float cossingned) {
	int_float s, c;
	s.f = sin;
	c.f = cossingned;
	s.i &= 0xFFFF'FFFE; // zeroing lowest bit of mantissa
	s.i |= c.i >> 31;	// set sign in lowest bit of mantissa from cos
	return s.f;
}

void AlignedRotationMatrixToRotation(float Matrix[3][4], vector3* sins);
void RotationToAlignedRotationMatrix(float Matrix[3][4], vector3* sins);