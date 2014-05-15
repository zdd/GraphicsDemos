// include the basic windows header files and the Direct3D header file 
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h> 
#include <math.h>


// define the screen resolution
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// include the Direct3D Library files
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 d3ddev;
LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;    // the pointer to the vertex buffer
LPDIRECT3DINDEXBUFFER9 i_buffer = NULL;    // the pointer to the index buffer
LPD3DXMESH ppMesh = NULL;

// function prototypes
void initD3D(HWND hWnd);
void render_frame(void);
void cleanD3D(void);
void init_graphics(void);

struct CUSTOMVERTEX {FLOAT X, Y, Z; DWORD COLOR;};
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE )
#define CUSTOMFVF_NORMAL (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL)

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"WindowClass";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL, L"WindowClass", L"Our Direct3D Program",
		WS_OVERLAPPEDWINDOW, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	initD3D(hWnd);

	MSG msg;

	while(TRUE)
	{
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT)
			break;

		render_frame();
	}

	cleanD3D();

	return msg.wParam;
}


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		} break;
	}

	return DefWindowProc (hWnd, message, wParam, lParam);
}

HRESULT SetupDirectionalLight(D3DXVECTOR3& direction, D3DXCOLOR color)
{
        D3DLIGHT9 dirLight;
        ZeroMemory(&dirLight, sizeof(dirLight));

        dirLight.Type = D3DLIGHT_DIRECTIONAL;
        dirLight.Range = 0.1f;
        dirLight.Direction = direction;

        dirLight.Ambient   = color * 0.6f;
        dirLight.Diffuse   = color;
        dirLight.Specular  = color * 0.6f;

        // Enable lighting
        d3ddev->SetRenderState(D3DRS_LIGHTING, TRUE);

        // Set light
        d3ddev->SetLight(0, &dirLight);

        // Enable light
        d3ddev->LightEnable(0, TRUE);

        return D3D_OK;
}

HRESULT SetupPointLight(D3DXVECTOR3& position, D3DXCOLOR color)
{
        D3DLIGHT9 pointLight;
        ZeroMemory(&pointLight, sizeof(pointLight));

        pointLight.Type = D3DLIGHT_POINT ;

        pointLight.Ambient              = color * 0.6f;
        pointLight.Diffuse              = color;
        pointLight.Specular             = color * 0.6f;
        pointLight.Position             = position;
        pointLight.Range                = 1000.0f;
        pointLight.Falloff              = 1.0f;
        pointLight.Attenuation0 = 1.0f;
        pointLight.Attenuation1 = 0.0f;
        pointLight.Attenuation2 = 0.0f;

        d3ddev->SetRenderState(D3DRS_LIGHTING, TRUE);

        // Set light
        d3ddev->SetLight(0, &pointLight);

        // Enable light
        d3ddev->LightEnable(0, TRUE);

        return D3D_OK;
}

HRESULT SetupMaterial(D3DXCOLOR color)
{
        D3DMATERIAL9 material;
        ZeroMemory(&material, sizeof(material));

        material.Ambient = color;
        material.Diffuse = color;
        material.Specular = color;
        /*
        This component is used to add to the overall color of the surface, 
        making it appear brighter like its giving off its own light.
        */
        material.Emissive = D3DXCOLOR(0, 0, 0, 0);
        material.Power = 2.0f; // only affect specular color

        d3ddev->SetMaterial(&material);

        return D3D_OK;
}

// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	init_graphics();

	d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
	d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);    // turn off culling
	d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer

	// Setup material
	D3DCOLOR red = D3DCOLOR_XRGB(255, 255, 255);
	SetupMaterial(red);

	// Setup directional light
	D3DCOLOR white = D3DCOLOR_XRGB(0, 255, 255);
	D3DXVECTOR3 direction(0, 0, -10);
	SetupDirectionalLight(direction, white);

	// Setup point  light
	D3DCOLOR white1 = D3DCOLOR_XRGB(112, 255, 0);
	D3DXVECTOR3 position(10, 10, 10);
	SetupPointLight(position, white1);
}


// this is the function used to render a single frame
void render_frame(void)
{
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	d3ddev->BeginScene();

	d3ddev->SetFVF(CUSTOMFVF);

	// set the view transform
	D3DXMATRIX matView;    // the view transform matrix
	D3DXMatrixLookAtLH(&matView,
		&D3DXVECTOR3 (1.0f, 9.0f, 50.0f),    // the camera position
		&D3DXVECTOR3 (1.0f, 1.0f, 1.0f),      // the look-at position
		&D3DXVECTOR3 (1.0f, 2.0f, 1.0f));    // the up direction
	d3ddev->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView 

	// set the projection transform
	D3DXMATRIX matProjection;    // the projection transform matrix
	D3DXMatrixPerspectiveFovLH(&matProjection,
		D3DXToRadian(60),    // the horizontal field of view
		(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, // aspect ratio
		1.0f,   // the near view-plane
		100.0f);    // the far view-plane
	d3ddev->SetTransform(D3DTS_PROJECTION, &matProjection); // set the projection

	// set the world transform
	static float index = 0.0f; index+=0.03f; // an ever-increasing float value
	D3DXMATRIX matRotateY;    // a matrix to store the rotation for each triangle
	D3DXMATRIX matTranslateX_venus;
	D3DXMATRIX matTranslateX_merkurius;
	D3DXMATRIX matTranslateX_sun;
	D3DXMATRIX matScale_merkurius;
	D3DXMATRIX matScale_sun;
	D3DXMATRIX matTranslateX_earth;
	D3DXMATRIX matTranslateX_mars;

	D3DXMatrixRotationY(&matRotateY, index);    // the rotation matrix
	D3DXMatrixTranslation(&matTranslateX_venus,16.0f,0.0f,0.0f);
	D3DXMatrixTranslation(&matTranslateX_earth,24.0f,0.0f,0.0f);
	D3DXMatrixTranslation(&matTranslateX_mars,32.0f,0.0f,0.0f);
	D3DXMatrixTranslation(&matTranslateX_merkurius,8.0f,0.0f,0.0f);
	D3DXMatrixTranslation(&matTranslateX_sun,0.0f,0.0f,0.0f);
	D3DXMatrixScaling(&matScale_merkurius, 0.5f, 0.5f, 0.5f);
	D3DXMatrixScaling(&matScale_sun, 2.5f, 2.5f, 2.5f);

	// set the world transform
	// select the vertex buffer to display
	d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
	d3ddev->SetIndices(i_buffer);

	// draw the pyramid

	d3ddev->SetTransform(D3DTS_WORLD, &(matScale_sun*matTranslateX_sun *matRotateY)); 
	//d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 12, 0, 20);
	ppMesh->DrawSubset(0);

	/*d3ddev->SetTransform(D3DTS_WORLD, &(matTranslateX_venus * matRotateY)); 
	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 12, 0, 12, 0, 20);

	d3ddev->SetTransform(D3DTS_WORLD, &(matScale_merkurius*matTranslateX_merkurius
		*         matRotateY));
	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 24, 0, 12, 0, 20);

	d3ddev->SetTransform(D3DTS_WORLD, &(matTranslateX_earth * matRotateY));
	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 36, 0, 12, 0, 20);

	d3ddev->SetTransform(D3DTS_WORLD, &(matTranslateX_mars * matRotateY));
	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 48, 0, 12, 0, 20);*/
	d3ddev->EndScene(); 

	d3ddev->Present(NULL, NULL, NULL, NULL);
}


// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	v_buffer->Release();
	i_buffer->Release();
	d3ddev->Release();
	d3d->Release();
}

// this is the function that puts the 3D models into video RAM
void init_graphics(void)
{
	// create the vertices using the CUSTOMVERTEX
	float t = (1.0 + sqrt(5.0)) / 2.0;
	struct CUSTOMVERTEX vertices[] =
	{
		{-1, t, 0, D3DCOLOR_XRGB(255,255,0), }, 
		{1, t, 0, D3DCOLOR_XRGB(255,255,0), },
		{-1, -t, 0, D3DCOLOR_XRGB(255,255,0), },
		{1, -t, 0, D3DCOLOR_XRGB(255,255,0), },
		{0, -1, t, D3DCOLOR_XRGB(255,255,0), },
		{0, 1, t, D3DCOLOR_XRGB(255,255,0), },
		{0, -1, -t, D3DCOLOR_XRGB(255,255,0), },
		{0, 1, -t, D3DCOLOR_XRGB(255,255,0), },
		{t, 0, -1, D3DCOLOR_XRGB(255,255,0), },
		{t, 0, 1, D3DCOLOR_XRGB(255,255,0), },
		{-t, 0, -1, D3DCOLOR_XRGB(255,255,0), },
		{-t, 0, 1, D3DCOLOR_XRGB(255,255,0), },

		/*{-1, t, 0, D3DCOLOR_XRGB(88,90,97), }, 
		{1, t, 0, D3DCOLOR_XRGB(88,90,97), },
		{-1, -t, 0, D3DCOLOR_XRGB(88,90,97), },
		{1, -t, 0, D3DCOLOR_XRGB(88,90,97), },
		{0, -1, t, D3DCOLOR_XRGB(88,90,97), },
		{0, 1, t, D3DCOLOR_XRGB(88,90,97), },
		{0, -1, -t, D3DCOLOR_XRGB(88,90,97), },
		{0, 1, -t, D3DCOLOR_XRGB(88,90,97), },
		{t, 0, -1, D3DCOLOR_XRGB(88,90,97), },
		{t, 0, 1, D3DCOLOR_XRGB(88,90,97), },
		{-t, 0, -1, D3DCOLOR_XRGB(88,90,97), },
		{-t, 0, 1, D3DCOLOR_XRGB(88,90,97), },

		{-1, t, 0, D3DCOLOR_XRGB(136,108,57), }, 
		{1, t, 0, D3DCOLOR_XRGB(136,108,57), },
		{-1, -t, 0, D3DCOLOR_XRGB(136,108,57), },
		{1, -t, 0, D3DCOLOR_XRGB(136,108,57), },
		{0, -1, t, D3DCOLOR_XRGB(136,108,57), },
		{0, 1, t, D3DCOLOR_XRGB(136,108,57), },
		{0, -1, -t, D3DCOLOR_XRGB(136,108,57), },
		{0, 1, -t, D3DCOLOR_XRGB(136,108,57), },
		{t, 0, -1, D3DCOLOR_XRGB(136,108,57), },
		{t, 0, 1, D3DCOLOR_XRGB(136,108,57), },
		{-t, 0, -1, D3DCOLOR_XRGB(136,108,57), },
		{-t, 0, 1, D3DCOLOR_XRGB(136,108,57), },

		{-1, t, 0, D3DCOLOR_XRGB(14,51,180), }, 
		{1, t, 0, D3DCOLOR_XRGB(14,51,180), },
		{-1, -t, 0, D3DCOLOR_XRGB(14,51,180), },
		{1, -t, 0, D3DCOLOR_XRGB(14,51,180), },
		{0, -1, t, D3DCOLOR_XRGB(14,51,180), },
		{0, 1, t, D3DCOLOR_XRGB(14,51,180), },
		{0, -1, -t, D3DCOLOR_XRGB(14,51,180), },
		{0, 1, -t, D3DCOLOR_XRGB(14,51,180), },
		{t, 0, -1, D3DCOLOR_XRGB(14,51,180), },
		{t, 0, 1, D3DCOLOR_XRGB(14,51,180), },
		{-t, 0, -1, D3DCOLOR_XRGB(14,51,180), },
		{-t, 0, 1, D3DCOLOR_XRGB(14,51,180), },

		{-1, t, 0, D3DCOLOR_XRGB(192,49,1), }, 
		{1, t, 0, D3DCOLOR_XRGB(192,49,1), },
		{-1, -t, 0, D3DCOLOR_XRGB(192,49,1), },
		{1, -t, 0, D3DCOLOR_XRGB(192,49,1), },
		{0, -1, t, D3DCOLOR_XRGB(192,49,1), },
		{0, 1, t, D3DCOLOR_XRGB(192,49,1), },
		{0, -1, -t, D3DCOLOR_XRGB(192,49,1), },
		{0, 1, -t, D3DCOLOR_XRGB(192,49,1), },
		{t, 0, -1, D3DCOLOR_XRGB(192,49,1), },
		{t, 0, 1, D3DCOLOR_XRGB(192,49,1), },
		{-t, 0, -1, D3DCOLOR_XRGB(192,49,1), },
		{-t, 0, 1, D3DCOLOR_XRGB(192,49,1), },*/
	};

	// create a vertex buffer interface called v_buffer
	d3ddev->CreateVertexBuffer(60*sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&v_buffer,
		NULL);

	VOID* pVoid;    // a void pointer

	// lock v_buffer and load the vertices into it
	v_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, vertices, sizeof(vertices));
	v_buffer->Unlock();

	// create the indices using an int array
	short indices[] =
	{
		// 5 faces around p0
		0, 11, 5,
		0, 5, 1,
		0, 1, 7,
		0, 7, 10,
		0, 10, 11,
		// 5 adjacent faces
		1, 5, 9,
		5, 11, 4,
		11, 10, 2,
		10, 7, 6,
		7, 1, 8,
		// 5 faces around point p3
		3, 9, 4,
		3, 4, 2,
		3, 2, 6,
		3, 6, 8,
		3, 8, 9,
		// 5 adjacent faces
		4, 9, 5,
		2, 4, 11,
		6, 2, 10,
		8, 6, 7,
		9, 8, 1
	};

	// create a index buffer interface called i_buffer
	d3ddev->CreateIndexBuffer(60*sizeof(short),
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&i_buffer,
		NULL);

	// lock i_buffer and load the indices into it
	i_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, indices, sizeof(indices));
	i_buffer->Unlock(); 

	// Create mesh
	HRESULT hr = D3DXCreateMeshFVF(12, 60, 0, CUSTOMFVF, d3ddev, &ppMesh);
	if (FAILED(hr)) 
	{
		MessageBoxA(NULL, "Create Mesh failed", "Error", 0);
	}

	// Fill in vertex buffer data
	LPDIRECT3DVERTEXBUFFER9 pVBuf;
	if (SUCCEEDED(ppMesh->GetVertexBuffer(&pVBuf))) 
	{
		CUSTOMVERTEX *pVert;
		
		if (SUCCEEDED(pVBuf->Lock(0,0,(void **)&pVert, 0))) 
		{
			memcpy(pVert, vertices, sizeof(vertices));
			pVBuf->Unlock();
		}
		pVBuf->Release();
	}

	// Fill in index buffer data
	LPDIRECT3DINDEXBUFFER9 pIBuf;
	if (SUCCEEDED(ppMesh->GetIndexBuffer(&pIBuf))) 
	{
		VOID* pIndex;
		if (SUCCEEDED(pIBuf->Lock(0,0,(void **)&pIndex, 0))) 
		{
			memcpy(pIndex, indices, sizeof(indices));
			pIBuf->Unlock();
		}
		pIBuf->Release();
	}

	// Clone mesh with normal added to vertex format
	DWORD fvf = ppMesh->GetFVF();
	if (!(fvf & D3DFVF_NORMAL))
	{
		fvf |= D3DFVF_NORMAL;
		ID3DXMesh* newMesh;
		hr = ppMesh->CloneMeshFVF(0, fvf, d3ddev, &newMesh);
		if (FAILED(hr)) 
		{
			MessageBoxA(NULL, "Clone Mesh failed", "Error", 0);
		}

		// Release old mesh 
		//ppMesh->Release();
		ppMesh = newMesh;
	}

	// Compute normals for new mesh
	hr = D3DXComputeNormals(ppMesh, NULL);
	if (FAILED(hr)) 
	{
		MessageBoxA(NULL, "Compute normals failed", "Error", 0);
	}
}