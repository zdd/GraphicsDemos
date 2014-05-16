/*
This demo show how to build the matrix for rotation around an arbitrary axis

A arbitrary axis is any axis that not the coordinate axis, so we can conclude them as 3 types, 
the axis are represent by two points v1 and v2
1. the axis parallel to the coordinate axis, such as v1(1, 2, 3), v2(1, 4, 3) 
2. the axis pass the origin, such as v1(-1, -1, -1), v2(1, 1, 1)
3. the axis that not 1 and 2

The steps below describe how to do the rotation for 3 kinds of axis
for axis of type 1
1. translate the axis to make it coincide with the coordinate axis(such as Z-axis)
2. rotate
3. translate the axis back

for axis of type 2
1. rotate the axis to make it lay on the coordinate plane(such as XOZ plane)
2. rotate the axis to make it coincide with the coordinate axis(such as Z-axis)
3. rotate
4. inverse of step 2
5. inverse of step 1

for axis of type 3
1. move the axis to make it pass the origin
2. do same steps as type 2
1. move back 
*/

#include <d3dx9.h>
#include <MMSystem.h>

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device
ID3DXMesh*				g_pTeapotMesh		= NULL ; // Hold the teapot
float					g_totalAngle		= 0.0f;

// for rotation
D3DXMATRIX				g_pWorldMat;				// world matrix
D3DXVECTOR3				g_pAxis(0.0f, 1.0f, 0.0f);
D3DXVECTOR3				g_pV1(-1.0, 1.00, 0.0);
D3DXVECTOR3				g_pV2(1.0f, 1.0f, 0.0f);

bool					g_bActive			= true ; // Is window active?

#define SAFE_RELEASE(P) if(P){ P->Release(); P = NULL;}

HRESULT InitD3D( HWND hWnd )
{
	// Create the D3D object, which is needed to create the D3DDevice.
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		MessageBoxA(NULL, "Create D3D9 object failed!", "Error", 0) ;
		return E_FAIL;
	}

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );

	d3dpp.Windowed = TRUE; // use window mode, not full screen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	// Create device
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		MessageBoxA(NULL, "Create D3D9 device failed!", "Error", 0) ;
		return E_FAIL;
	}

	// Disable lighting, since we didn't specify color for vertex
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING , FALSE );   

	// Create teapot
	D3DXCreateTeapot(g_pd3dDevice, &g_pTeapotMesh, NULL) ;

	// wire frame
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return S_OK;
}

D3DXMATRIX* Rotate(D3DXMATRIX* pOut, D3DXVECTOR3 v1, D3DXVECTOR3 v2, float theta)
{
	g_totalAngle += theta;

	// v1 and v2 is the same point, return
	if (v1 == v2)
		return NULL;

	D3DXMatrixIdentity(pOut);

	//Step 1: Move v1 to origin
	D3DXMATRIX T;
	//D3DXMatrixTranslation(&T, -v1.x, -v1.y, -v1.z);
	D3DXMatrixTranslation(&T, 0, 0, 0);

	// Calculate and normalize the direction of the axis
	D3DXVECTOR3 p;
	D3DXVECTOR3 q = v2 -v1;
	//D3DXVec3Normalize(&p, &q);

	float a = p.x;
	float b = p.y;
	float c = p.z;

	D3DXMATRIX rotX;
	D3DXMatrixIdentity(&rotX);

	float cosalpha = c / sqrt(b * b + c * c);
	float sinalpha = b / sqrt(b * b + c * c);
	if (b < 0)
		sinalpha = -sinalpha;
		
	rotX.m[1][1] = cosalpha;
	rotX.m[1][2] = sinalpha;
	rotX.m[2][1] = -sinalpha;
	rotX.m[2][2] = cosalpha;

	//// Step 3: Rotate axis to make it coincide to the Z-axis
	D3DXMATRIX rotY;
	D3DXMatrixIdentity(&rotY);

	float cosbeta = sqrt(b * b + c * c) / sqrt(a * a + b * b + c * c);
	float sinbeta = a / sqrt(a * a + b * b + c * c);
	if (a > 0)
		sinbeta = -sinbeta;

	rotY.m[0][0] = cosbeta;
	rotY.m[0][2] = -sinbeta;
	rotY.m[2][0] = sinbeta;
	rotY.m[2][2] = cosbeta;

	// Step 4: Rotate theta degree around Z-axis
	D3DXMATRIX rotZ;
	D3DXMatrixRotationZ(&rotZ, g_totalAngle);

	//// inverse of step 3

	D3DXMATRIX inRotY;
	D3DXMatrixIdentity(&inRotY);

	float negcosbeta = cosbeta;
	float negsinbeta = -sinbeta;

	rotY.m[0][0] = negcosbeta;
	rotY.m[0][2] = -negsinbeta;
	rotY.m[2][0] = negsinbeta;
	rotY.m[2][2] = negcosbeta;

	//// inverse of step 2

	D3DXMATRIX inRotX;
	D3DXMatrixIdentity(&inRotX);

	float negcosalpha = cosalpha;
	float negsinalpha = -sinalpha;

	rotX.m[1][1] = negcosalpha;
	rotX.m[1][2] = negsinalpha;
	rotX.m[2][1] = -negsinalpha;
	rotX.m[2][2] = negcosalpha;

	// inverse of step 1
	D3DXMATRIX invT;
	D3DXMatrixTranslation(&invT, v1.x, v1.y, v1.z);

	D3DXMATRIX final = rotX * rotY * rotZ * inRotY * inRotX;
	*pOut = final;
	return pOut;
}

// Rotate around an arbitrary axis, theta is the rotate angle in radius
// The rotation direction is clockwise when looking against origin through the axis
void RotateArbitraryAxis(D3DXMATRIX* pOut, D3DXVECTOR3* axis, float theta)
{
	D3DXVec3Normalize(axis, axis);
	float u = axis->x;
	float v = axis->y;
	float w = axis->z;

	pOut->m[0][0] = cosf(theta) + (u * u) * (1 - cosf(theta));
	pOut->m[0][1] = u * v * (1 - cosf(theta)) + w * sinf(theta);
	pOut->m[0][2] = u * w * (1 - cosf(theta)) - v * sinf(theta);
	pOut->m[0][3] = 0;

	pOut->m[1][0] = u * v * (1 - cosf(theta)) - w * sinf(theta);
	pOut->m[1][1] = cosf(theta) + v * v * (1 - cosf(theta));
	pOut->m[1][2] = w * v * (1 - cosf(theta)) + u * sinf(theta);
	pOut->m[1][3] = 0;

	pOut->m[2][0] = u * w * (1 - cosf(theta)) + v * sinf(theta);
	pOut->m[2][1] = v * w * (1 - cosf(theta)) - u * sinf(theta);
	pOut->m[2][2] = cosf(theta) + w * w * (1 - cosf(theta));
	pOut->m[2][3] = 0;

	pOut->m[3][0] = 0;
	pOut->m[3][1] = 0;
	pOut->m[3][2] = 0;
	pOut->m[3][3] = 1;

	//// Rotate p around x-axis to the XOZ plane
	//float alpha = asinf(b / sqrt(b * b + c * c));
	//D3DXMATRIX rotX;
	//D3DXMatrixRotationX(&rotX, alpha);

	//float beta = asinf(a / sqrt(a * a + b * b + c * c));

	//D3DXMATRIX rotY;
	//D3DXMatrixRotationY(&rotY, beta);

	//D3DXMATRIX rotZ;
	//D3DXMatrixRotationZ(&rotZ, g_totalAngle);

	//D3DXMATRIX inverseRotY;
	//D3DXMatrixRotationY(&inverseRotY, -beta);

	//D3DXMATRIX inverseRotX;
	//D3DXMatrixRotationX(&inverseRotX, -alpha);

	//*pOut = rotX * rotY * rotZ * inverseRotY * inverseRotX ;
	////D3DXMatrixRotationAxis(pOut, &p, g_totalAngle);
}
// Rotate around an arbitrary axis(not pass the origin), theta is the rotate angle in radius
// The rotation direction is clockwise when looking against origin through the axis

void RotateArbitraryLine(D3DXMATRIX* pOut, D3DXVECTOR3* v1, D3DXVECTOR3* v2, float theta)
{
	float a = v1->x;
	float b = v1->y;
	float c = v1->z;

	D3DXVECTOR3 p = *v2 - *v1;
	D3DXVec3Normalize(&p, &p);
	float u = p.x;
	float v = p.y;
	float w = p.z;

	float uu = u * u;
	float uv = u * v;
	float uw = u * w;
	float vv = v * v;
	float vw = v * w;
	float ww = w * w;
	float au = a * u;
	float av = a * v;
	float aw = a * w;
	float bu = b * u;
	float bv = b * v;
	float bw = b * w;
	float cu = c * u;
	float cv = c * v;
	float cw = c * w;

	float costheta = cosf(theta);
	float sintheta = sinf(theta);

	pOut->m[0][0] = uu + (vv + ww) * costheta;
	pOut->m[0][1] = uv * (1 - costheta) + w * sintheta;
	pOut->m[0][2] = uw * (1 - costheta) - v * sintheta;
	pOut->m[0][3] = 0;

	pOut->m[1][0] = uv * (1 - costheta) - w * sintheta;
	pOut->m[1][1] = vv + (uu + ww) * costheta;
	pOut->m[1][2] = vw * (1 - costheta) + u * sintheta;
	pOut->m[1][3] = 0;

	pOut->m[2][0] = uw * (1 - costheta) + v * sintheta;
	pOut->m[2][1] = vw * (1 - costheta) - u * sintheta;
	pOut->m[2][2] = ww * (uu + vv) * costheta;
	pOut->m[2][3] = 0;

	pOut->m[3][0] = (a * (vv + ww) - u * (bv + cw)) * (1 - costheta) + (bw - cv) * sintheta;
	pOut->m[3][1] = (b * (uu + ww) - v * (au + cw)) * (1 - costheta) + (cu - aw) * sintheta;
	pOut->m[3][2] = (c * (uu + vv) - w * (au + bv)) * (1 - costheta) + (av - bu) * sintheta;
	pOut->m[3][3] = 1;
}

VOID Cleanup()
{
	SAFE_RELEASE(g_pTeapotMesh) ;
	SAFE_RELEASE(g_pd3dDevice) ;
	SAFE_RELEASE(g_pD3D) ;
}

void SetupMatrix(float timeDelta)
{
	g_totalAngle += timeDelta;
	RotateArbitraryAxis(&g_pWorldMat, &g_pAxis, g_totalAngle);
	//RotateArbitraryLine(&g_pWorldMat, &g_pV1, &g_pV2, g_totalAngle);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_pWorldMat);

	// set view
	D3DXVECTOR3 eyePt(0.0f, 0.0f, -10.0f) ;
	D3DXVECTOR3 upVec(0.0f, 1.0f, 0.0f) ;
	D3DXVECTOR3 lookCenter(0.0f, 0.0f, 0.0f) ;

	D3DXMATRIX view ;
	D3DXMatrixLookAtLH(&view, &eyePt, &lookCenter, &upVec) ;
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &view) ;

	// set projection
	D3DXMATRIX proj ;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f) ;
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj) ;
}

void Render(float timeDelta)
{
	if (!g_bActive)
	{
		Sleep(50) ;
	}

	SetupMatrix(timeDelta) ;

	// Clear the back-buffer to a RED color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// Draw teapot 
		g_pTeapotMesh->DrawSubset(0) ;

		// End the scene
		g_pd3dDevice->EndScene();
	}

	// Present the back-buffer contents to the display
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case VK_ESCAPE:
				SendMessage( hWnd, WM_CLOSE, 0, 0 );
				break ;
			default:
				break ;
			}
		}
		return 0 ;

	case WM_SIZE:
		if(wParam == SIZE_MAXHIDE || wParam == SIZE_MINIMIZED)
			g_bActive = false;
		else
			g_bActive = true;
		return 0;

	case WM_ACTIVATEAPP:
		if(wParam == TRUE)
			g_bActive = true ;
		else
			g_bActive = false ;
		return 0 ;

	case WM_DESTROY:
		Cleanup();
		PostQuitMessage( 0 );
		return 0;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR szCmdLine, int iCmdShow)
{
	WNDCLASSEX winClass ;

	winClass.lpszClassName = "Teapot";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = MsgProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = NULL ;
	winClass.hIconSm	   = NULL ;
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW) ;
	winClass.hbrBackground = NULL ;
	winClass.lpszMenuName  = NULL ;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	RegisterClassEx (&winClass) ;  

	HWND hWnd = CreateWindowEx(NULL,  
		winClass.lpszClassName,		// window class name
		"Teapot",					// window caption
		WS_OVERLAPPEDWINDOW, 		// window style
		32,							// initial x position
		32,							// initial y position
		600,						// initial window width
		600,						// initial window height
		NULL,						// parent window handle
		NULL,						// window menu handle
		hInstance,					// program instance handle
		NULL) ;						// creation parameters

	// Create window failed
	if(hWnd == NULL)
	{
		MessageBoxA(hWnd, "Create Window failed!", "Error", 0) ;
		return -1 ;
	}

	// Initialize Direct3D
	if( SUCCEEDED(InitD3D(hWnd)))
	{ 
		// Show the window
		ShowWindow( hWnd, SW_SHOWDEFAULT );
		UpdateWindow( hWnd );

		MSG msg ; 
		ZeroMemory( &msg, sizeof(msg) );
		PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

		// Get last time
		static DWORD lastTime = timeGetTime();

		while (msg.message != WM_QUIT)  
		{
			if(PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0)
			{
				TranslateMessage (&msg) ;
				DispatchMessage (&msg) ;
			}
			else // Render the game if there is no message to process
			{
				// Get current time
				DWORD currTime  = timeGetTime();

				// Calculate time elapsed
				float timeDelta = (currTime - lastTime) * 0.001f;

				// Render
				Render(timeDelta) ;

				// Update last time to current time for next loop
				lastTime = currTime;
			}
		}
	}

	UnregisterClass(winClass.lpszClassName, hInstance) ;
	return 0;
}