// System headers
#include <time.h>

// Customer headers
#include "Camera.h"
#include "Cube.h"
#include "Math.h"

#pragma warning(disable: 4244)

// Globals
HWND				g_hHandle		= NULL ;	// Game window handle
D3DPRESENT_PARAMETERS d3dpp;					// D3D present parameters
LPDIRECT3D9			g_pD3D			= NULL ;	// Used to create the D3DDevice
LPDIRECT3DDEVICE9	g_pd3dDevice	= NULL ;	// Our rendering device
Camera				g_Camera ;					// Model view camera

int OldWindowWidth = 0 ;
int OldWindowHeight = 0 ;

int WindowWidth = 800 ;
int WindowHeight = 800 ;

WINDOWPLACEMENT wp ;			// window placement, used for full-screen -> window

bool OneRotateFinish = true;	// A rotate action is not allowed if the previous rotation was in process
bool Inactive = false ;			// window is inactive, if true, stop rendering and yield time 50ms to other app
bool HitCube = false ;			// true if ray intersection with Rubik cube when left button down
bool MouseDrag = false ;		// WM-MOUSEMOVE is not processed if mouse was not dragged
bool AlreadyGetLayer = false ;	// 

ArcBall WorldBall ;

float RectWidth = 30.5f ;		// The width of the RubikCube
float LayerWidth = 10.0f ;		// width of layer
int HitFace = -1 ;				// Current hit face 
int PreviousLayer = 0 ;			// The layer id for the previous rotation
int CurrentLayer = 0 ;			// current layer has been dragged
char CurrentDirection = 'c' ;	// rotation direction of current layer
float TotalAngle = 0.0f ;		// the total angle from left button down till left button up
D3DXVECTOR3 HitPoint ;			// Hit point when mouse is down
D3DXVECTOR3 PreviousPoint ;			// the point when mouse is down ;
D3DXVECTOR3 CurrentPoint ;		// current point during mouse moving

const float HalfWidth = RectWidth / 2 ;

// Eight corner of the Rubik cube
D3DXVECTOR3 A(-HalfWidth,  HalfWidth, -HalfWidth);  D3DXVECTOR3 B( HalfWidth,  HalfWidth, -HalfWidth);
D3DXVECTOR3 C( HalfWidth, -HalfWidth, -HalfWidth);  D3DXVECTOR3 D(-HalfWidth, -HalfWidth, -HalfWidth);
D3DXVECTOR3 E(-HalfWidth,  HalfWidth,  HalfWidth);  D3DXVECTOR3 F( HalfWidth,  HalfWidth,  HalfWidth);
D3DXVECTOR3 G( HalfWidth, -HalfWidth,  HalfWidth);  D3DXVECTOR3 H(-HalfWidth, -HalfWidth,  HalfWidth);

// Six plane of the Rubik cube
Rect  FrontFace(A, B, C, D) ; 
Rect   BackFace(E, F, G, H) ;
Rect   LeftFace(E, A, D, H) ;
Rect  RightFace(B, F, G, C) ;
Rect    TopFace(E, F, B, A) ;
Rect BottomFace(G, H, D, C) ;

const int FaceNum = 6 ;
Rect Faces[FaceNum] = {FrontFace, BackFace, LeftFace, RightFace, TopFace, BottomFace} ;

// Random distort Rubik cube
void Shuffle() ;

// forward declaration, WM_PAINT message use this function
void Render() ;

// this array hold the 27 unit cubes which will build up the final Rubik
Cube Cubes[27] ;

// Name of the .x file
// the model is made by 3Dmax and convert to .x file by DeepExploration
WCHAR* xFileName[27] = 
{
	L"0.x",  L"1.x",  L"2.x",  L"3.x",  L"4.x",  L"5.x",  L"6.x",  L"7.x",  L"8.x",
	L"9.x", L"10.x", L"11.x", L"12.x", L"13.x", L"14.x", L"15.x", L"16.x", L"17.x",
	L"18.x", L"19.x", L"20.x", L"21.x", L"22.x", L"23.x", L"24.x", L"25.x", L"26.x",
} ;

// this is the absolutely position info of the Rubik cube, this info is not changed during rotation
// you can think about this position as a 3D grid like a container of unit cube
// after rotation, a cube may change it's position, move from one grid to another, but the grid is still there.
// each time one cube occupy one grid
// we use the position information to get the corresponding cube and do the rotation
// for example, if we want to rotation the top layer around the z-axis, we get the cube in the 9 top grids
// 0,  1,  2,  3,  4,  5,  6,  7,  8(just for clarification)and rotation them, no mater which cube were in these position
int id[] = 
{ 
	0,  1,  2,  3,  4,  5,  6,  7,  8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26 
} ;

// Clean the resource
VOID Cleanup()
{
	if(g_pd3dDevice != NULL) 
		g_pd3dDevice->Release();

	if(g_pD3D != NULL)       
		g_pD3D->Release();
}

// Update the ids for a given layer, based on the number of D3DX_PI / 2
// that the layer has rotated
void UpdateLayerInfo(int layer, char direction, int numOfHalfPI)
{
	numOfHalfPI %= 4 ;

	if(numOfHalfPI == 0)
		return ;

	int temp = -1;

	if(numOfHalfPI == 1)
	{
		switch(layer)
		{
		case 0:
			if(direction == 'r')
			{
				temp = id[0]; id[0] = id[18]; id[18] = id[24]; id[24] =  id[6];  id[6] = temp ; // corner
				temp = id[3]; id[3] =  id[9];  id[9] = id[21]; id[21] = id[15]; id[15] = temp ; // cross
			}
			else
			{
				temp = id[0]; id[0] =  id[6];  id[6] = id[24]; id[24] = id[18]; id[18] = temp ;
				temp = id[3]; id[3] = id[15]; id[15] = id[21]; id[21] =  id[9];  id[9] = temp ;
			}
			break ;

		case 1:
			if (direction == 'r')
			{
				temp = id[1]; id[1] = id[19]; id[19] = id[25]; id[25] =  id[7];  id[7] = temp ;
				temp = id[4]; id[4] = id[10]; id[10] = id[22]; id[22] = id[16]; id[16] = temp ;
			}
			else
			{
				temp = id[1]; id[1] =  id[7];  id[7] = id[25]; id[25] = id[19]; id[19] = temp ;
				temp = id[4]; id[4] = id[16]; id[16] = id[22]; id[22] = id[10]; id[10] = temp ;
			}
			break ;

		case 2:
			if (direction == 'r')
			{
				temp = id[2]; id[2] = id[20]; id[20] = id[26]; id[26] =  id[8];  id[8] = temp ;
				temp = id[5]; id[5] = id[11]; id[11] = id[23]; id[23] = id[17]; id[17] = temp ;
			} 
			else
			{
				temp = id[2]; id[2] =  id[8];  id[8] = id[26]; id[26] = id[20]; id[20] = temp ;
				temp = id[5]; id[5] = id[17]; id[17] = id[23]; id[23] = id[11]; id[11] = temp ;
			}
			break ;

		case 3:
			if (direction == 'r')
			{
				temp = id[18]; id[18] = id[20]; id[20] = id[26]; id[26] = id[24]; id[24] = temp ;
				temp = id[19]; id[19] = id[23]; id[23] = id[25]; id[25] = id[21]; id[21] = temp ;
			} 
			else
			{
				temp = id[18]; id[18] = id[24]; id[24] = id[26]; id[26] = id[20]; id[20] = temp ;
				temp = id[19]; id[19] = id[21]; id[21] = id[25]; id[25] = id[23]; id[23] = temp ;
			}
			break ;

		case 4:
			if (direction == 'r')
			{
				temp =  id[9];  id[9] = id[11]; id[11] = id[17]; id[17] = id[15]; id[15] = temp ;
				temp = id[10]; id[10] = id[14]; id[14] = id[16]; id[16] = id[12]; id[12] = temp ;
			} 
			else
			{
				temp =  id[9];  id[9] = id[15]; id[15] = id[17]; id[17] = id[11]; id[11] = temp ;
				temp = id[10]; id[10] = id[12]; id[12] = id[16]; id[16] = id[14]; id[14] = temp ;
			}
			break ;

		case 5:
			if (direction == 'r')
			{
				temp = id[0]; id[0] = id[2]; id[2] = id[8]; id[8] = id[6]; id[6] = temp ;
				temp = id[1]; id[1] = id[5]; id[5] = id[7]; id[7] = id[3]; id[3] = temp ;
			} 
			else
			{
				temp = id[0]; id[0] = id[6]; id[6] = id[8]; id[8] = id[2]; id[2] = temp ;
				temp = id[1]; id[1] = id[3]; id[3] = id[7]; id[7] = id[5]; id[5] = temp ;
			}
			break ;

		case 6:
			if (direction == 'r')
			{
				temp = id[6]; id[6] = id[24]; id[24] = id[26]; id[26] =  id[8];  id[8] = temp ;
				temp = id[7]; id[7] = id[15]; id[15] = id[25]; id[25] = id[17]; id[17] = temp ;
			} 
			else
			{
				temp = id[6]; id[6] =  id[8];  id[8] = id[26]; id[26] = id[24]; id[24] = temp ;
				temp = id[7]; id[7] = id[17]; id[17] = id[25]; id[25] = id[15]; id[15] = temp ;
			}
			break ;

		case 7:
			if (direction == 'r')
			{
				temp = id[3]; id[3] = id[21]; id[21] = id[23]; id[23] =  id[5];  id[5] = temp ;
				temp = id[4]; id[4] = id[12]; id[12] = id[22]; id[22] = id[14]; id[14] = temp ;
			} 
			else
			{
				temp = id[3]; id[3] =  id[5];  id[5] = id[23]; id[23] = id[21]; id[21] = temp ;
				temp = id[4]; id[4] = id[14]; id[14] = id[22]; id[22] = id[12]; id[12] = temp ;
			}
			break ;

		case 8:
			if (direction == 'r')
			{
				temp = id[0]; id[0] = id[18]; id[18] = id[20]; id[20] =  id[2];  id[2] = temp ;
				temp = id[1]; id[1] =  id[9];  id[9] = id[19]; id[19] = id[11]; id[11] = temp ;
			} 
			else
			{
				temp = id[0]; id[0] =  id[2];  id[2] = id[20]; id[20] = id[18]; id[18] = temp ;
				temp = id[1]; id[1] = id[11]; id[11] = id[19]; id[19] =  id[9];  id[9] = temp ;
			}
			break ;

		default:
			throw "Unknown layer!" ;
			break ;
		}
	}

	else if(numOfHalfPI == 2)
	{
		switch(layer)
		{
		case 0: swap( id[0], id[24]); swap( id[6], id[18]); swap( id[3], id[21]); swap( id[9], id[15]); break ;
		case 1: swap( id[1], id[25]); swap( id[7], id[19]); swap( id[4], id[22]); swap(id[10], id[16]); break ;
		case 2: swap( id[2], id[26]); swap( id[8], id[20]); swap( id[5], id[23]); swap(id[11], id[17]); break ;
		case 3: swap(id[18], id[26]); swap(id[20], id[24]); swap(id[19], id[25]); swap(id[21], id[23]); break ;
		case 4: swap( id[9], id[17]); swap(id[11], id[15]); swap(id[10], id[16]); swap(id[12], id[14]); break ;
		case 5: swap( id[0],  id[8]); swap( id[2],  id[6]); swap( id[1],  id[7]); swap( id[3],  id[5]); break ;
		case 6: swap( id[6], id[26]); swap( id[8], id[24]); swap( id[7], id[25]); swap(id[15], id[17]); break ;
		case 7: swap( id[3], id[23]); swap( id[5], id[21]); swap( id[4], id[22]); swap(id[12], id[14]); break ;
		case 8: swap( id[0], id[20]); swap( id[2], id[18]); swap( id[1], id[19]); swap( id[9], id[11]); break ;

		default: throw "Unknown layer!" ; break ;
		}
	}

	else if(numOfHalfPI == 3)
	{
		switch(layer)
		{
		case 0:
			if(direction == 'c')
			{
				temp = id[0]; id[0] =  id[6];  id[6] = id[24]; id[24] = id[18]; id[18] = temp ;
				temp = id[3]; id[3] = id[15]; id[15] = id[21]; id[21] =  id[9];  id[9] = temp ;

			}
			else
			{
				temp = id[0]; id[0] = id[18]; id[18] = id[24]; id[24] =  id[6];  id[6] = temp ;
				temp = id[3]; id[3] =  id[9];  id[9] = id[21]; id[21] = id[15]; id[15] = temp ;
			}
			break ;

		case 1:
			if (direction == 'r')
			{
				temp = id[1]; id[1] =  id[7];  id[7] = id[25]; id[25] = id[19]; id[19] = temp ;
				temp = id[4]; id[4] = id[16]; id[16] = id[22]; id[22] = id[10]; id[10] = temp ;
			}
			else
			{
				temp = id[1]; id[1] = id[19]; id[19] = id[25]; id[25] =  id[7];  id[7] = temp ;
				temp = id[4]; id[4] = id[10]; id[10] = id[22]; id[22] = id[16]; id[16] = temp ;
			}
			break ;

		case 2:
			if (direction == 'r')
			{
				temp = id[2]; id[2] =  id[8];  id[8] = id[26]; id[26] = id[20]; id[20] = temp ;
				temp = id[5]; id[5] = id[17]; id[17] = id[23]; id[23] = id[11]; id[11] = temp ;
			} 
			else
			{
				temp = id[2]; id[2] = id[20]; id[20] = id[26]; id[26] =  id[8];  id[8] = temp ;
				temp = id[5]; id[5] = id[11]; id[11] = id[23]; id[23] = id[17]; id[17] = temp ;
			}
			break ;

		case 3:
			if (direction == 'r')
			{
				temp = id[18]; id[18] = id[24]; id[24] = id[26]; id[26] = id[20]; id[20] = temp ;
				temp = id[19]; id[19] = id[21]; id[21] = id[25]; id[25] = id[23]; id[23] = temp ;
			} 
			else
			{
				temp = id[18]; id[18] = id[20]; id[20] = id[26]; id[26] = id[24]; id[24] = temp ;
				temp = id[19]; id[19] = id[23]; id[23] = id[25]; id[25] = id[21]; id[21] = temp ;
			}
			break ;

		case 4:
			if (direction == 'r')
			{
				temp =  id[9];  id[9] = id[15]; id[15] = id[17]; id[17] = id[11]; id[11] = temp ;
				temp = id[10]; id[10] = id[12]; id[12] = id[16]; id[16] = id[14]; id[14] = temp ;
			} 
			else
			{
				temp =  id[9];  id[9] = id[11]; id[11] = id[17]; id[17] = id[15]; id[15] = temp ;
				temp = id[10]; id[10] = id[14]; id[14] = id[16]; id[16] = id[12]; id[12] = temp ;
			}
			break ;

		case 5:
			if (direction == 'r')
			{
				temp = id[0]; id[0] = id[6]; id[6] = id[8]; id[8] = id[2]; id[2] = temp ;
				temp = id[1]; id[1] = id[3]; id[3] = id[7]; id[7] = id[5]; id[5] = temp ;
			} 
			else
			{
				temp = id[0]; id[0] = id[2]; id[2] = id[8]; id[8] = id[6]; id[6] = temp ;
				temp = id[1]; id[1] = id[5]; id[5] = id[7]; id[7] = id[3]; id[3] = temp ;
			}
			break ;

		case 6:
			if (direction == 'r')
			{
				temp = id[6]; id[6] =  id[8];  id[8] = id[26]; id[26] = id[24]; id[24] = temp ;
				temp = id[7]; id[7] = id[17]; id[17] = id[25]; id[25] = id[15]; id[15] = temp ;
			} 
			else
			{
				temp = id[6]; id[6] = id[24]; id[24] = id[26]; id[26] =  id[8];  id[8] = temp ;
				temp = id[7]; id[7] = id[15]; id[15] = id[25]; id[25] = id[17]; id[17] = temp ;
			}
			break ;

		case 7:
			if (direction == 'r')
			{
				temp = id[3]; id[3] =  id[5];  id[5] = id[23]; id[23] = id[21]; id[21] = temp ;
				temp = id[4]; id[4] = id[14]; id[14] = id[22]; id[22] = id[12]; id[12] = temp ;
			} 
			else
			{
				temp = id[3]; id[3] = id[21]; id[21] = id[23]; id[23] =  id[5];  id[5] = temp ;
				temp = id[4]; id[4] = id[12]; id[12] = id[22]; id[22] = id[14]; id[14] = temp ;
			}
			break ;

		case 8:
			if (direction == 'r')
			{
				temp = id[0]; id[0] =  id[2];  id[2] = id[20]; id[20] = id[18]; id[18] = temp ;
				temp = id[1]; id[1] = id[11]; id[11] = id[19]; id[19] =  id[9];  id[9] = temp ;
			} 
			else
			{
				temp = id[0]; id[0] = id[18]; id[18] = id[20]; id[20] =  id[2];  id[2] = temp ;
				temp = id[1]; id[1] =  id[9];  id[9] = id[19]; id[19] = id[11]; id[11] = temp ;
			}
			break ;

		default:
			throw "Unknown layer!" ;
			break ;
		}
	}
	else
		throw "Number of half PI is incorrect!" ;
}

// Get the cubes in a given layer, we have 9 layers total
// 0, 1, 2 along x-axis
// 3, 4, 5 along y-axis
// 6, 7, 8 along z-axis
// the numbers of the layer are increasing form the origin to the positive part of the axis
void GetLayerCubes(int layer, int cubes[])
{
	switch(layer)
	{
	case 0:
		cubes[0] =  id[0]; cubes[1] =  id[3]; cubes[2] =  id[6]; 
		cubes[3] =  id[9]; cubes[4] = id[12]; cubes[5] = id[15]; 
		cubes[6] = id[18]; cubes[7] = id[21]; cubes[8] = id[24];
		break;

	case 1:
		cubes[0] =  id[1]; cubes[1] =  id[4]; cubes[2] =  id[7]; 
		cubes[3] = id[10]; cubes[4] = id[13]; cubes[5] = id[16]; 
		cubes[6] = id[19]; cubes[7] = id[22]; cubes[8] = id[25];
		break;

	case 2:
		cubes[0] =  id[2]; cubes[1] =  id[5]; cubes[2] =  id[8]; 
		cubes[3] = id[11]; cubes[4] = id[14]; cubes[5] = id[17]; 
		cubes[6] = id[20]; cubes[7] = id[23]; cubes[8] = id[26];
		break;

	case 3:
		cubes[0] = id[18]; cubes[1] = id[19]; cubes[2] = id[20]; 
		cubes[3] = id[21]; cubes[4] = id[22]; cubes[5] = id[23]; 
		cubes[6] = id[24]; cubes[7] = id[25]; cubes[8] = id[26];
		break;

	case 4:
		cubes[0] =  id[9]; cubes[1] = id[10]; cubes[2] = id[11]; 
		cubes[3] = id[12]; cubes[4] = id[13]; cubes[5] = id[14]; 
		cubes[6] = id[15]; cubes[7] = id[16]; cubes[8] = id[17];
		break;

	case 5:
		cubes[0] =  id[0]; cubes[1] =  id[1]; cubes[2] =  id[2]; 
		cubes[3] =  id[3]; cubes[4] =  id[4]; cubes[5] =  id[5]; 
		cubes[6] =  id[6]; cubes[7] =  id[7]; cubes[8] =  id[8];
		break;

	case 6:
		cubes[0] =  id[6]; cubes[1] =  id[7]; cubes[2] =  id[8]; 
		cubes[3] = id[15]; cubes[4] = id[16]; cubes[5] = id[17]; 
		cubes[6] = id[24]; cubes[7] = id[25]; cubes[8] = id[26];
		break;

	case 7:
		cubes[0] =  id[3]; cubes[1] =  id[4]; cubes[2] =  id[5]; 
		cubes[3] = id[12]; cubes[4] = id[13]; cubes[5] = id[14]; 
		cubes[6] = id[21]; cubes[7] = id[22]; cubes[8] = id[23];
		break;

	case 8:
		cubes[0] =  id[0]; cubes[1] =  id[1]; cubes[2] =  id[2]; 
		cubes[3] =  id[9]; cubes[4] = id[10]; cubes[5] = id[11]; 
		cubes[6] = id[18]; cubes[7] = id[19]; cubes[8] = id[20];
		break;

	default:
		throw L"Unknown layer!";
	}
}

// Calculate the hit layer, oldPoint is given when left button is down
// curPoint is given during mouse moving, transform both into model space, and subtract the two vectors to get a new vector
// compute the angle between the vector and the coordinate axis to get the rotation layer and direction
// Once a rotation layer is selected, it will keep to take effect until the left button is up
void SetLayer(int faceId, D3DXVECTOR3 *hitPoint, D3DXVECTOR3 *prePoint, D3DXVECTOR3 *curPoint)
{
	if(AlreadyGetLayer)
		return ;

	D3DXVECTOR3 xAxis(1.0f, 0.0f, 0.0f) ;
	D3DXVECTOR3 yAxis(0.0f, 1.0f, 0.0f) ;
	D3DXVECTOR3 zAxis(0.0f, 0.0f, 1.0f) ;

	D3DXVECTOR3 dragVector = *curPoint - *prePoint ;
	D3DXVec3Normalize(&dragVector, &dragVector) ;

	float cos45 = 0.7071067811f ;

	// Compute the cosine value 
	float angleWithXAxis = D3DXVec3Dot(&dragVector, &xAxis) ;
	float angleWithYAxis = D3DXVec3Dot(&dragVector, &yAxis) ;
	float angleWithZAxis = D3DXVec3Dot(&dragVector, &zAxis) ;

	float x = hitPoint->x + HalfWidth ;
	float y = hitPoint->y + HalfWidth ;
	float z = hitPoint->z + HalfWidth ;

	// Set layer
	if(abs(angleWithXAxis) >= cos45)
	{
		if(faceId == 0 || faceId == 1)	// rotation around the y-axis
		{
			CurrentLayer = (int)(y / 10.25) + 3;
		}

		if(faceId == 4 || faceId == 5) // rotation around the z-axis
		{
			CurrentLayer = (int)(z / 10.25) + 6;
		}
	}

	else if(abs(angleWithYAxis) >= cos45)
	{
		if(faceId == 0 || faceId == 1) // rotation around the x-axis
		{
			CurrentLayer = (int)(x / 10.25) ;
		}

		if(faceId == 2 || faceId == 3) // rotation around the z-axis
		{
			CurrentLayer = (int)(z / 10.25) + 6;
		}
	}

	else //angleWithZAxis >= cos45
	{
		if(faceId == 2 || faceId == 3) // rotation around the y-axis
		{
			CurrentLayer = (int)(y / 10.25) + 3;
		}

		if(faceId == 4 || faceId == 5) // rotation around the x-axis
		{
			CurrentLayer = (int)(x / 10.25) ;
		}
	}

	AlreadyGetLayer = true ;
}

void SetDirection(int faceId, D3DXVECTOR3 *hitPoint, D3DXVECTOR3 *prePoint, D3DXVECTOR3 *curPoint)
{
	D3DXVECTOR3 xAxis(1.0f, 0.0f, 0.0f) ;
	D3DXVECTOR3 yAxis(0.0f, 1.0f, 0.0f) ;
	D3DXVECTOR3 zAxis(0.0f, 0.0f, 1.0f) ;

	D3DXVECTOR3 dragVector = *curPoint - *prePoint ;
	D3DXVec3Normalize(&dragVector, &dragVector) ;

	float cos45 = 0.7071067811f ;

	// Compute the cosine value 
	float angleWithXAxis = D3DXVec3Dot(&dragVector, &xAxis) ;
	float angleWithYAxis = D3DXVec3Dot(&dragVector, &yAxis) ;
	float angleWithZAxis = D3DXVec3Dot(&dragVector, &zAxis) ;

	float x = hitPoint->x + HalfWidth ;
	float y = hitPoint->y + HalfWidth ;
	float z = hitPoint->z + HalfWidth ;

	// Set direction
	if(abs(angleWithXAxis) >= cos45)
	{
		if(faceId == 0 || faceId == 4)	// rotation around the y-axis
		{
			if(angleWithXAxis >= 0.0f)
				CurrentDirection = 'r' ;
			else
				CurrentDirection = 'c' ;
		}

		if(faceId == 1 || faceId == 5)	// rotation around the y-axis
		{
			if(angleWithXAxis >= 0.0f)
				CurrentDirection = 'c' ;
			else
				CurrentDirection = 'r' ;
		}
	}

	else if(abs(angleWithYAxis) >= cos45)
	{
		if(faceId == 0 || faceId == 3) // rotation around the x-axis
		{
			if(angleWithYAxis >= 0.0f)
				CurrentDirection = 'c' ;
			else
				CurrentDirection = 'r' ;
		}

		if(faceId == 1 ||faceId == 2) // rotation around the x-axis
		{
			if(angleWithYAxis >= 0.0f)
				CurrentDirection = 'r' ;
			else
				CurrentDirection = 'c' ;
		}
	}

	else //angleWithZAxis >= cos45
	{
		if(faceId == 2 || faceId == 4) // rotation around the y-axis
		{
			if(angleWithZAxis >= 0.0f)
				CurrentDirection = 'c' ;
			else
				CurrentDirection = 'r' ;
		}

		if(faceId == 3 || faceId == 5) // rotation around the y-axis
		{
			if(angleWithZAxis >= 0.0f)
				CurrentDirection = 'r' ;
			else
				CurrentDirection = 'c' ;
		}
	}
}

// layer: 0-8
// direction: 'c'-clockwise, 'r'-counterclockwise
// the angles are measured clockwise when looking along the rotation axis toward the origin
void RotateLayer(int layer, char direction, float angle)
{
	if (angle == 0.0f)
		return ;

	// Get cube ids on current selected layer
	int cubeIds[9] ;
	GetLayerCubes(layer, cubeIds) ;

	// Build up the rotation matrix based on direction and angle
	D3DXMATRIX matRotation ;

	if(layer >= 0 && layer <= 2)		// rotation around x-axis
	{
		if(direction == 'c')
			D3DXMatrixRotationX(&matRotation, angle) ;
		else					
			D3DXMatrixRotationX(&matRotation, -angle) ;
	}
	else if(layer >= 3 && layer <= 5)	// rotation around y-axis
	{
		if(direction == 'c')
			D3DXMatrixRotationY(&matRotation, angle) ;
		else					
			D3DXMatrixRotationY(&matRotation, -angle) ;
	}
	else if(layer >= 6 && layer <= 8)	// rotation around z-axis
	{
		if(direction == 'c')
			D3DXMatrixRotationZ(&matRotation, angle) ;
		else					
			D3DXMatrixRotationZ(&matRotation, -angle) ;
	}

	else
		throw "Layer id is incorrect!" ;

	// Apply the rotation matrix for cubes on current layer
	for(int i = 0; i < 9; i++)
	{
		Cubes[cubeIds[i]].Rotate(&matRotation);
	}
}

// Transform the screen point to vector in model space
D3DXVECTOR3 ScreenToVector3(int x, int y)
{
	D3DXVECTOR3 vector3 ;

	// Get view port
	D3DVIEWPORT9 vp;
	g_pd3dDevice->GetViewport(&vp);

	// Get Projection matrix
	D3DXMATRIX proj;
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &proj);

	vector3.x = ((( 2.0f*x) / vp.Width)  - 1.0f) / proj(0, 0);
	vector3.y = (((-2.0f*y) / vp.Height) + 1.0f) / proj(1, 1);
	vector3.z = 1.0f ;

	// Get view matrix
	D3DXMATRIX view;
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &view);

	// Get world matrix
	D3DXMATRIX world;
	g_pd3dDevice->GetTransform(D3DTS_WORLD, &world);

	// Concatenate them in to single matrix
	D3DXMATRIX WorldView = world * view;

	// inverse it
	D3DXMATRIX worldviewInverse;
	D3DXMatrixInverse(&worldviewInverse, 0, &WorldView);

	D3DXVec3TransformCoord(&vector3, &vector3, &worldviewInverse) ;

	D3DXVec3Normalize(&vector3, &vector3) ;

	return vector3 ;
}

// Calculate the picking ray and transform it to model space 
// x and y are the screen coordinates when left button down
Ray CalcPickingRay(int x, int y)
{
	float px = 0.0f;
	float py = 0.0f;

	// Get viewport
	D3DVIEWPORT9 vp;
	g_pd3dDevice->GetViewport(&vp);

	// Get Projection matrix
	D3DXMATRIX proj;
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &proj);

	px = ((( 2.0f*x) / vp.Width)  - 1.0f) / proj(0, 0);
	py = (((-2.0f*y) / vp.Height) + 1.0f) / proj(1, 1);

	Ray ray;
	ray._origin    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	ray._direction = D3DXVECTOR3(px, py, 1.0f); // 方向，注意投影平面的Z坐标是1 

	// Get view matrix
	D3DXMATRIX view;
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &view);

	// Get world matrix
	D3DXMATRIX world;
	g_pd3dDevice->GetTransform(D3DTS_WORLD, &world);

	// Concatinate them in to single matrix
	D3DXMATRIX WorldView = world * view;

	// inverse it
	D3DXMATRIX worldviewInverse;
	D3DXMatrixInverse(&worldviewInverse, 0, &WorldView);


	// Transform the ray to model space
	D3DXVec3TransformCoord(&ray._origin, &ray._origin, &worldviewInverse) ;
	D3DXVec3TransformNormal(&ray._direction, &ray._direction, &worldviewInverse) ;

	// Normalize the direction
	D3DXVec3Normalize(&ray._direction, &ray._direction) ;

	return ray;
}

void OnLeftButtonDown(int x, int y)
{
	if(!OneRotateFinish) // another rotate is in progress, return directly
		return ;
	OneRotateFinish = false ; // Prevent the other rotation during this one

	// Calculate the picking ray
	Ray ray = CalcPickingRay(x, y) ;

	D3DXVECTOR3 currentHitPoint;	// hit point on the face
	float distance ;		// distance from the origin of the ray and to the hit point
	float maxDist = 100000.0f ;

	// Loop the six faces of the cube and select the one nearest to the camera
	for(int i = 0; i < FaceNum; i++)
	{
		if(ray.Intersection(&Faces[i], &currentHitPoint, &distance))
		{
			HitCube = true ;

			if(distance < maxDist)
			{
				maxDist = distance ;
				HitPoint = currentHitPoint ;
				HitFace = i ;
			}
		}
	}

	// no action if there is no intersection
	if(!HitCube)
		return ;

	PreviousPoint = ScreenToVector3(x, y) ;

	// if the ray intersect with a face, start up the arc ball
	WorldBall.OnBegin(x, y) ;
}

void OnMouseMove(int x, int y)
{
	WorldBall.OnMove(x, y) ;

	// Get the rotation increment
	D3DXQUATERNION quat = WorldBall.GetRotationQuatIncreament();

	//extract rotation angle from quaternion
	float angle = 2.0f * acosf(quat.w) ;

	// Get current point and transform it into model space
	CurrentPoint = ScreenToVector3(x, y) ;

	// Get rotation layer 
	SetLayer(HitFace, &HitPoint, &PreviousPoint, &CurrentPoint) ;
	SetDirection(HitFace, &HitPoint, &PreviousPoint, &CurrentPoint) ;
	
	// accumulate the total angle, total angle is used for last update
	if(CurrentDirection == 'c')
		TotalAngle += angle ;
	else
		TotalAngle -= angle ;

	// Rotate
	RotateLayer(CurrentLayer, CurrentDirection, angle);

	// Update previous point
	PreviousPoint = CurrentPoint ;
}

// When Left button up, complete the rotation of the left angle to align the cube and update the layer info
void OnLeftButtonUp()
{
	HitCube = false ;
	AlreadyGetLayer = false ;

	WorldBall.OnEnd();

	float leftAngle = 0.0f ;	// the angle need to rotate when mouse is up
	int numOfHalfPI = 0 ;

	char totalDirection ;
	if(TotalAngle >= 0.0f)
		totalDirection = 'c' ;
	else
		totalDirection = 'r' ;

	// The left direction
	char leftDirection ;
	if(TotalAngle >= 0.0f)
		leftDirection = 'c' ;
	else
		leftDirection = 'r' ;

	// Count the absolute value of LayerAngle
	TotalAngle = abs(TotalAngle) ;

	// Count how many 90 degrees the cube has rotate
	while (TotalAngle > D3DX_PI / 2)
	{
		TotalAngle -= D3DX_PI / 2;
		numOfHalfPI++ ;
	}

	// more than D3DX_PI / 4 and less than D3DX_PI / 2 it treated as D3DX_PI / 2
	if(TotalAngle > D3DX_PI / 4)
	{
		numOfHalfPI++ ;
		leftAngle = D3DX_PI / 2 - TotalAngle ;
	}
	else
	{
		leftAngle = TotalAngle ; 

		// reverse the rotation direction since the left angle is less than D3DX_PI / 4
		if(leftDirection == 'c')
			leftDirection = 'r' ;
		else
			leftDirection = 'c' ;
	}

	// Rotate the left angle for current layer
	RotateLayer(CurrentLayer, leftDirection, leftAngle) ;

	// Update Unit cube position after rotation
	UpdateLayerInfo(CurrentLayer, totalDirection, numOfHalfPI) ;

	// clear LayerAngle for next accumulation
	TotalAngle = 0.0f ;

	OneRotateFinish = true ; // Make the next rotate enable
}


HRESULT ResetDevice(int width, int height) ;
void ToggleFullScreen() ;

// Message process
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)   
{
	switch (message)    
	{
	case WM_ACTIVATE:
		if(wParam == WA_ACTIVE)
			Inactive = false ;
		if(wParam == WA_INACTIVE)
			Inactive = true ;
		break ;

	case WM_LBUTTONDOWN:
		{	
			// Set current window to capture mouse event, so even if the mouse was release outside the window
			// the message still can be correctly processed.
			SetCapture(hwnd) ;
			int iMouseX = ( short )LOWORD( lParam );
			int iMouseY = ( short )HIWORD( lParam );
			OnLeftButtonDown(iMouseX, iMouseY);
		}
		break ;

	case WM_CAPTURECHANGED:
		OnLeftButtonUp() ;
		ReleaseCapture();

	case WM_LBUTTONUP:
		{
			OnLeftButtonUp();
			ReleaseCapture();
		}
		break ;

	case WM_MOUSEMOVE:
		{
			// Only response when the picking ray hit the cube
			if(HitCube)
			{
				int iMouseX = ( short )LOWORD( lParam );
				int iMouseY = ( short )HIWORD( lParam );
				OnMouseMove(iMouseX, iMouseY) ;
			}
		}
		break ;

	case WM_PAINT: 
		Render();
		break ;

	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case 'F':
				ToggleFullScreen() ;
				break;
			case 'R':
				Shuffle() ;
				break ;
			case VK_ESCAPE:
				SendMessage( g_hHandle, WM_CLOSE, 0, 0 );
				break ;
			default:
				break ;
			}
		}
		break ;

	case WM_SIZE:
		{
			// inactive the app when window is minimized
			if(SIZE_MINIMIZED == wParam)
				Inactive = true ;
			else
			{
				Inactive = false;

				// Get window width and height after resize
				int currentWindowWidth = ( short )LOWORD( lParam );
				int currentWindowHeight = ( short )HIWORD( lParam );

				// Reset device only when user resize the window
				if(currentWindowWidth != WindowWidth || currentWindowHeight != WindowHeight)
				{
					WindowWidth = currentWindowWidth ;
					WindowHeight = currentWindowHeight ;
					ResetDevice(WindowWidth, WindowHeight) ;
				}
			}
		}
		break ;

	case WM_CLOSE:
	case WM_DESTROY:
		Cleanup() ;
		PostQuitMessage (0) ;
		return 0 ;    
	}

	// handle mouse message
	g_Camera.HandleMessages(hwnd, message, wParam, lParam) ;

	return DefWindowProc (hwnd, message, wParam, lParam) ;
}


// Random mix the Rubik cube
void Shuffle()
{
	if(!OneRotateFinish)
		return ;
	OneRotateFinish = false ;

	// Set the random seed
	srand((unsigned int)time(0));

	// Generate direction
	char dir = ' ' ;
	if(rand() % 2 == 0)
		dir = 'c' ;
	else
		dir = 'r' ;

	// Rotate 20 times
	for(int i = 0; i < 20; i++)
	{
		// Generate the layer
		int layer = rand() % 9 ;
		RotateLayer(layer, dir, D3DX_PI / 2) ;
		UpdateLayerInfo(layer, dir, 1) ;
	}

	OneRotateFinish = true ;
}

// Setup view and projection matrix
void SetupMatrix()
{
	// View matrix
	D3DXMATRIX matView = *g_Camera.GetViewMatrix() ;
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView) ;

	// Projection matrix
	D3DXMATRIX matProj = *g_Camera.GetProjMatrix() ;
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj) ;
}

// Setup a point light
void SetupLight()
{
	// Create light
	D3DLIGHT9 pointLight ;

	// Light color
	D3DXCOLOR color = D3DCOLOR_XRGB(255, 255, 255) ;

	// Light position
	D3DXVECTOR3 position = *g_Camera.GetEyePoint() ;
	pointLight.Position		= position;

	// Light type, we use point light here
	pointLight.Type			= D3DLIGHT_POINT ;

	// Light attributes
	pointLight.Ambient		= color * 0.6f;
	pointLight.Diffuse		= color;
	pointLight.Specular		= color * 0.6f;

	pointLight.Range		= 320.0f;
	pointLight.Falloff		= 1.0f;
	pointLight.Attenuation0	= 1.0f;
	pointLight.Attenuation1	= 0.0f;
	pointLight.Attenuation2	= 0.0f;

	// Enable light
	g_pd3dDevice->SetLight(0, &pointLight) ;
	g_pd3dDevice->LightEnable(0, true) ;
}

// Reset the scene by rebuild the viewing frustum
void ResizeD3DScene(int width, int height)
{
	if (height == 0 )				// Prevent A Divide By Zero By
		height = 1;					// Making Height Equal One

	// Compute aspect ratio
	float fAspectRatio = width / (FLOAT)height;

	// Setup Projection matrix
	g_Camera.SetProjParams( D3DX_PI/4, fAspectRatio, 1.0f, 1000.0f );

	g_Camera.SetWindow( width, height );
}

// Reset the device, this function is called when back-buffer size changed
// and is invoked when user resize the window
HRESULT ResetDevice(int width, int height)
{
	// update the back-buffer, the back-buffer size is always same as window size
	d3dpp.Windowed          = TRUE;
	d3dpp.SwapEffect        = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferCount	= 1 ;
	d3dpp.BackBufferFormat	= D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE ;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16 ;
	d3dpp.BackBufferWidth	= width ;
	d3dpp.BackBufferHeight	= height ;
	
	// Reset the device
	HRESULT hr = g_pd3dDevice->Reset(&d3dpp) ;
	if(FAILED(hr))
	{
		if (hr == D3DERR_DEVICELOST)
		{
			return D3DERR_DEVICELOST ;
		}
		else
		{
			MessageBoxA(NULL, "Reset device error", "Error", 0) ;
			return E_FAIL ;
		}
	}
	
	// Reset the scene based on the new back-buffer width and height
	ResizeD3DScene(width, height) ;

	return D3D_OK ;
}

// Switch from window mode and full-screen mode
void ToggleFullScreen()
{
	// Get Current Adapter's display mode, the width and height field is the maximum resolution 
	D3DDISPLAYMODE displayMode ;
	g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode) ;

	// Window -> Full-Screen
	if(d3dpp.Windowed)
	{
		d3dpp.Windowed = FALSE;
		d3dpp.BackBufferWidth	= displayMode.Width ;
		d3dpp.BackBufferHeight	= displayMode.Height ;
		
		// Save current window size
		OldWindowWidth = WindowWidth ;
		OldWindowHeight = WindowHeight ;

		// Get and save window placement
		GetWindowPlacement(g_hHandle, &wp) ;
	}
	else // Full-Screen -> Window
	{
		d3dpp.Windowed = TRUE;

		// Restore old window size
		d3dpp.BackBufferWidth	=  OldWindowWidth;
		d3dpp.BackBufferHeight	=  OldWindowHeight;

		// Set window placement
		SetWindowPlacement(g_hHandle, &wp) ;
	}

	// Reset the device
	HRESULT hr = g_pd3dDevice->Reset(&d3dpp) ;
	if(FAILED(hr))
		MessageBox(NULL, L"Reset device failed!", L"Error!", 0);

	// Reset the scene based on the new back-buffer width and height
	ResizeD3DScene(d3dpp.BackBufferWidth, d3dpp.BackBufferHeight) ;
}

// Initialize DiretX
void InitD3D(HWND hWnd)
{
	// Create the D3D object.
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		MessageBox(hWnd, L"Create Direct3D9 failed!", L"error!", 0) ;
	
	ZeroMemory( &d3dpp, sizeof(d3dpp) );

	// We didn't specify the back-buffer width and height, D3D will initialize it to the window width and height
	d3dpp.Windowed          = TRUE;
	d3dpp.SwapEffect        = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferCount	= 1 ;
	d3dpp.BackBufferFormat	= D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE ;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16 ;

	// Create the D3DDevice
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		MessageBox(hWnd, L"Create Direct3D9 device failed!", L"error!", 0) ;
	}

	// Turn off culling, so we see the front and back of the triangle
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	// Setup view matrix
	D3DXVECTOR3 vecEye(0.0f, 0.0f, -1.0f);
	D3DXVECTOR3 vecAt (0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vecUp (0.0f, 1.0f, 0.0f) ;
	g_Camera.SetViewParams( &vecEye, &vecAt, &vecUp );

	// Setup projection matrix
	float aspectRatio = (float)d3dpp.BackBufferWidth / (float)d3dpp.BackBufferHeight ;
	g_Camera.SetProjParams(D3DX_PI / 4, aspectRatio, 1.0f, 1000.0f) ;

	// Load all the meshes from .x files
	for(int i = 0; i < 27; i++)
	{
		Cubes[i].LoadMesh(g_pd3dDevice, xFileName[i]);
	}
}

// Create game window
void CreateGameWindow(const WCHAR *strWindowTitle, HINSTANCE hInstance, int iCmdShow)
{
	WNDCLASSEX winClass ;

	winClass.lpszClassName = L"MY_WINDOWS_CLASS";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = WndProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = NULL ;
	winClass.hIconSm	   = NULL ;
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW) ;
	winClass.hbrBackground = NULL ;
	winClass.lpszMenuName  = NULL ;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	RegisterClassEx (&winClass) ;  

	g_hHandle = CreateWindowEx(NULL,  
		L"MY_WINDOWS_CLASS",        // window class name
		strWindowTitle,				// window caption
		WS_OVERLAPPEDWINDOW, 		// window style
		32,							// initial x position
		32,							// initial y position
		WindowWidth,				// initial x size
		WindowHeight,				// initial y size
		NULL,						// parent window handle
		NULL,						// window menu handle
		hInstance,					// program instance handle
		NULL) ;						// creation parameters

	InitD3D(g_hHandle) ;

	ShowWindow (g_hHandle, iCmdShow) ;
	UpdateWindow (g_hHandle) ;
}

// Animate the scene
void FrameMove()
{
	// Update light, the light position is always same as the camera eye point
	// so no matter how you rotate the camera, the cube will keep the same brightness
	// BUT...
	// Setting up a light in every frame is very expensive! any other way?
	SetupLight() ;
	g_Camera.OnFrameMove() ;
}

// Render the game
void Render()
{
	// if window is minimized or inactive stop rendering... yields 50ms to other program
	if(Inactive)
		Sleep(50) ;

	// Update frame
	FrameMove() ;

	// Update view and projection matrix
	SetupMatrix() ;

	// Clear the back buffer to a black color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0 );

	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		//draw all unit cubes to build the Rubik cube
		for(int i = 0; i <= 26; i++)
		{
			Cubes[i].Draw(g_pd3dDevice) ;
		}

		// Restore world matrix since the Draw function in class Cube has set the world matrix for each cube
		D3DXMATRIX matWorld = *g_Camera.GetWorldMatrix() ;
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld) ;

		g_pd3dDevice->EndScene();
	}

	// How to handle device lost here?
	// Present the back buffer contents to the display
	HRESULT hr = g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	// Check for device lost
	if(FAILED(hr))
	{
		// Device was lost
		if(hr == D3DERR_DEVICELOST)
		{
			// Test current device state, wait if it still in lost state
			if(FAILED(hr = g_pd3dDevice->TestCooperativeLevel()))
			{
				if(hr == D3DERR_DEVICELOST)
				{
					// The device has been lost but cannot be reset at this time.
					// So wait until it can be reset.
					return;
				}
			}
			else
			{
				hr == ResetDevice(WindowWidth, WindowHeight) ;

				if (hr == D3DERR_DEVICELOST)
				{
					return ;
				}
			}
		}

		else if(hr == D3DERR_DRIVERINTERNALERROR)
		{
			MessageBoxA(NULL, "Driver internal error!", "Error", 0) ;
		}
	}
}

// Main message loop
HRESULT MainMessageLoop()
{
	MSG    msg ; 
	ZeroMemory( &msg, sizeof(msg) );
	PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

	while (msg.message != WM_QUIT)  
	{
		if( PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0)
		{
			TranslateMessage (&msg) ;
			DispatchMessage (&msg) ;
		}
		else // Render the game if no message to process
		{
			Render() ;
		}
	}

	return msg.wParam ;
}

// Main entry point of program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	CreateGameWindow(L"Rubik Cube", hInstance, iCmdShow) ;
	MainMessageLoop() ;
}
