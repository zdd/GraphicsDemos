#ifndef UTILITY_H
#define UTILITY_H

#include <d3dx9.h>

__declspec(dllexport) float GetRandomFloat(float lowBound, float highBound) ;
__declspec(dllexport) void GetRandomVector(D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max) ;
__declspec(dllexport) DWORD FloatToDword(float f) ;

#endif // end UTILITY_H