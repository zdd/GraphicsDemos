// +-------------------+
// | Direct3D and Aero |
// +-------------------+---------------------------------+
// | Example written by Tristan Ward for codeproject.com |
// +-----------------------------------------------------+

// +------------+
// | Directives |
// +------------+
// Specify Windows version
#define WINVER         0x0600
#define _WIN32_WINNT   0x0600

// Includes
#include <d3d9.h>
#include <d3dx9.h>
#include <dwmapi.h>

// Import libraries to link with
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dwmapi.lib")

// Global constants
#define ARGB_RED     0xFFFF0000 // Full red full alpha
#define ARGB_GREEN   0xFF00FF00 // Full green full alpha
#define ARGB_BLUE    0x800000FF // Full blue 50% alpha
#define ARGB_CYAN    0xFF00FFFF // Full cyan full alpha
#define ARGB_TRANS   0x00000000 // 100% alpha
#define ARGB_AMBIENT 0xFF0A0A0A // Ambient light colour

// +---------+
// | Globals |
// +---------+
char                   *g_wcpAppName  = "VistaD3D";
INT                     g_iWidth       = 256;
INT                     g_iHeight      = 256;
MARGINS                 g_mgDWMMargins = {-1, -1, -1, -1};
IDirect3D9Ex            *g_pD3D        = NULL;
IDirect3DDevice9Ex      *g_pD3DDevice  = NULL;
IDirect3DVertexBuffer9  *g_pVB         = NULL;

struct CUSTOMVERTEX
{
  FLOAT     fX, fY, fZ; // The position for the vertex
  D3DVECTOR vctNormal;  // The normal of each vector (for lighting calcs)	
  DWORD     dwColour;   // The vertex color
};

// +--------------+
// | D3DStartup() |
// +--------------+----------------------------------+
// | Initialise Direct3D and perform once only tasks |
// +-------------------------------------------------+
HRESULT D3DStartup(HWND hWnd)
{
  BOOL                  bCompOk             = FALSE;   // Is composition enabled? 
  D3DPRESENT_PARAMETERS pp;                            // Presentation prefs
  DWORD                 msqAAQuality        = 0;       // Non-maskable quality
  D3DLIGHT9             ltDirectionalLight;            // Light description
  D3DVECTOR             vctLightDirection   = {-1.0f,  // X component
                                               -0.3f,  // Y component
                                               -1.0f}; // Z component
  D3DXMATRIX            mtxView;                       // View matrix
  D3DXMATRIX            mtxProjection;                 // Projection matrix

  // Make sure that DWM composition is enabled
  DwmIsCompositionEnabled(&bCompOk);
  if(!bCompOk) return E_FAIL;

  // Create a Direct3D object
  if(FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &g_pD3D))) return E_FAIL;

  // Setup presentation parameters
  ZeroMemory(&pp, sizeof(pp));
  pp.Windowed            = TRUE;
  pp.SwapEffect          = D3DSWAPEFFECT_DISCARD; // Required for multi sampling
  pp.BackBufferFormat    = D3DFMT_A8R8G8B8;       // Back buffer format with alpha channel

  // Set highest quality non-maskable AA available or none if not
  if(SUCCEEDED(g_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
                                                  D3DDEVTYPE_HAL,
                                                  D3DFMT_A8R8G8B8,
                                                  TRUE,
                                                  D3DMULTISAMPLE_NONMASKABLE,
                                                  &msqAAQuality
                                                  )))
  {
    // Set AA quality
   /* pp.MultiSampleType     = D3DMULTISAMPLE_NONMASKABLE;
    pp.MultiSampleQuality  = msqAAQuality - 1;*/
  }
  else
  {
    // No AA
    pp.MultiSampleType     = D3DMULTISAMPLE_NONE;
  }

  // Create a Direct3D device object
  if(FAILED(g_pD3D->CreateDeviceEx(D3DADAPTER_DEFAULT,
                                   D3DDEVTYPE_HAL,
                                   hWnd,
                                   D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                   &pp,
                                   NULL,
                                   &g_pD3DDevice
                                   ))) return E_FAIL;

  // Configure the device state
  g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);        // Enable 3D lighting
  g_pD3DDevice->SetRenderState(D3DRS_AMBIENT, ARGB_AMBIENT); // Set ambient lighting
  g_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE); // Disable specular highlighting
  
  // Create a directional light
  ZeroMemory(&ltDirectionalLight, sizeof(ltDirectionalLight));
  ltDirectionalLight.Type       = D3DLIGHT_DIRECTIONAL;
  ltDirectionalLight.Diffuse.r  = 1.0f;
  ltDirectionalLight.Diffuse.g  = 1.0f;
  ltDirectionalLight.Diffuse.b  = 1.0f;
  ltDirectionalLight.Diffuse.a  = 1.0f;				
  ltDirectionalLight.Direction  = vctLightDirection;

  // Add as light 0
  g_pD3DDevice->SetLight(0, &ltDirectionalLight);
  g_pD3DDevice->LightEnable(0, TRUE);

  // Configure camera
  D3DXMatrixLookAtLH(&mtxView,
                     &D3DXVECTOR3 (0.0f, 0.0f, 25.0f), // Camera position
                     &D3DXVECTOR3 (0.0f, 0.0f, 0.0f),  // Look-at target
                     &D3DXVECTOR3 (0.0f, 1.0f, 0.0f)); // Up direction
  g_pD3DDevice->SetTransform(D3DTS_VIEW, &mtxView);

  // Configure projection
  D3DXMatrixPerspectiveFovLH(&mtxProjection,
                             D3DXToRadian(45),                          // Horizontal field of view
                             (FLOAT)((FLOAT)g_iWidth/(FLOAT)g_iHeight),	// Aspect ratio
                             0.0f,										                  // Near view plane
                             100.0f);										                // Far view plane
  g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &mtxProjection);

  return S_OK;
}

// +---------------+
// | D3DShutdown() |
// +---------------+----------------------+
// | Release all created Direct3D objects |
// +--------------------------------------+
VOID D3DShutdown(VOID)
{
  if(g_pVB != NULL ) g_pVB->Release();
  if(g_pD3DDevice != NULL) g_pD3DDevice->Release();
  if(g_pD3D != NULL) g_pD3D->Release();
}

// +--------------+
// | CreateCube() |
// +--------------+------------------------------+
// | Populates a vertex buffer with a cube shape |
// +---------------------------------------------+
HRESULT CreateCube(VOID)
{
  VOID* pVBVertices = NULL; // Pointer to vertex buffer data 

  // Initialize 24 vertices describing the shape of a cube
  CUSTOMVERTEX cvtxCube[] = {
  {-5.0f, 5.0f, -5.0f, 0, 0, -1, ARGB_RED,},
  {5.0f, 5.0f, -5.0f, 0, 0, -1, ARGB_GREEN,},
  {-5.0f, -5.0f, -5.0f, 0, 0, -1, ARGB_BLUE,},
  {5.0f, -5.0f, -5.0f, 0, 0, -1, ARGB_CYAN,},

  {-5.0f, 5.0f, 5.0f, 0, 0, 1, ARGB_RED,},
  {-5.0f, -5.0f, 5.0f, 0, 0, 1, ARGB_GREEN,},
  {5.0f, 5.0f, 5.0f, 0, 0, 1, ARGB_BLUE,},
  {5.0f, -5.0f, 5.0f, 0, 0, 1, ARGB_CYAN,},

  {-5.0f, 5.0f, 5.0f, 0, 1, 0, ARGB_RED,},
  {5.0f, 5.0f, 5.0f, 0, 1, 0, ARGB_GREEN,},
  {-5.0f, 5.0f, -5.0f, 0, 1, 0, ARGB_BLUE,},
  {5.0f, 5.0f, -5.0f, 0, 1, 0, ARGB_CYAN,},

  {-5.0f, -5.0f, 5.0f, 0, -1, 0, ARGB_RED,},
  {-5.0f, -5.0f, -5.0f, 0, -1, 0, ARGB_GREEN,},
  {5.0f, -5.0f, 5.0f, 0, -1, 0, ARGB_BLUE,},
  {5.0f, -5.0f, -5.0f, 0, -1, 0, ARGB_CYAN,},

  {5.0f, 5.0f, -5.0f, 1, 0, 0, ARGB_RED,},
  {5.0f, 5.0f, 5.0f, 1, 0, 0, ARGB_GREEN,},
  {5.0f, -5.0f, -5.0f, 1, 0, 0, ARGB_BLUE,},
  {5.0f, -5.0f, 5.0f, 1, 0, 0, ARGB_CYAN,},

  {-5.0f, 5.0f, -5.0f, -1, 0, 0, ARGB_RED,},
  {-5.0f, -5.0f, -5.0f, -1, 0, 0, ARGB_GREEN,},
  {-5.0f, 5.0f, 5.0f, -1, 0, 0, ARGB_BLUE,},
  {-5.0f, -5.0f, 5.0f, -1, 0, 0, ARGB_CYAN,}};

  // Create a vertex buffer to hold the cube data
  if(FAILED(g_pD3DDevice->CreateVertexBuffer(24*sizeof(CUSTOMVERTEX),
                                             0,
                                             D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE,
                                             D3DPOOL_DEFAULT,
                                             &g_pVB,
                                             NULL)
                                             )) return E_FAIL;

  // Lock the vertex buffer for editing
  if(FAILED(g_pVB->Lock(0, sizeof(cvtxCube), (VOID**)&pVBVertices, 0))) return E_FAIL;
  
  // Copy vertex array to vertex buffer
  memcpy(pVBVertices, cvtxCube, sizeof(cvtxCube));
  
  // Unlock the vertex buffer
  g_pVB->Unlock();

  return S_OK;
}

// +----------+
// | Render() |
// +----------+-------------------------+
// | Renders a scene to the back buffer |
// +------------------------------------+
VOID Render(VOID)
{
  static float	fRotator      = 0.0f; // Cube rotation factor
  D3DXMATRIX    mtxRotateYPR;         // Yaw/Pitch/Roll matrix

  // Sanity check
  if(g_pD3DDevice == NULL) return;

  // Increment static float
  fRotator += 0.025f;
	
  // Rotate the cube using the static float
  D3DXMatrixRotationYawPitchRoll(&mtxRotateYPR, fRotator, fRotator, fRotator);
  g_pD3DDevice->SetTransform(D3DTS_WORLD, &mtxRotateYPR);

  // Clear the back buffer and z buffer to transparent
  g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, ARGB_TRANS, 1.0f, 0);

  // Render scene
  if(SUCCEEDED(g_pD3DDevice->BeginScene()))
  {
    // Draw the cube
    g_pD3DDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
    g_pD3DDevice->SetFVF(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE);
    g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2);
    g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 8, 2);
    g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 12, 2);
    g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 16, 2);
    g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 20, 2);
    g_pD3DDevice->EndScene();
  }

  // Update display
  g_pD3DDevice->PresentEx(NULL, NULL, NULL, NULL, NULL);
}

// +--------------+
// | WindowProc() |
// +--------------+------------------+
// | The main window message handler |
// +---------------------------------+
LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg)
  {
    case WM_DESTROY:
      // Signal application to terminate
      PostQuitMessage(0);
      return 0;
		
    case WM_KEYDOWN:
      // If ESC has been pressed then signal window should close
      if (LOWORD(wParam) == VK_ESCAPE) SendMessage(hWnd, WM_CLOSE, NULL, NULL);
    break;
    
    case WM_LBUTTONDOWN:
      // Trick OS into thinking we are dragging on title bar for any clicks on the main window
      SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, NULL);
      return TRUE;

    case WM_ERASEBKGND:
      // We dont want to call render twice so just force Render() in WM_PAINT to be called
      SendMessage(hWnd, WM_PAINT, NULL, NULL);
      return TRUE;

    case WM_PAINT:
      // Force a render to keep the window updated
      Render();
      return 0;
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// +-----------+
// | WinMain() |
// +-----------+---------+
// | Program entry point |
// +---------------------+
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT)
{
  HWND       hWnd  = NULL;
  MSG        uMsg;     
  WNDCLASSEX wc    = {sizeof(WNDCLASSEX),              // cbSize
                      NULL,                            // style
                      WindowProc,                      // lpfnWndProc
                      NULL,                            // cbClsExtra
                      NULL,                            // cbWndExtra
                      hInstance,                       // hInstance
                      LoadIcon(NULL, IDI_APPLICATION), // hIcon
                      LoadCursor(NULL, IDC_ARROW),     // hCursor
                      NULL,                            // hbrBackground
                      NULL,                            // lpszMenuName
                      g_wcpAppName,                    // lpszClassName
                      LoadIcon(NULL, IDI_APPLICATION)};// hIconSm

  RegisterClassEx(&wc);
  hWnd = CreateWindowEx(WS_EX_COMPOSITED,             // dwExStyle
                        g_wcpAppName,                 // lpClassName
                        g_wcpAppName,                 // lpWindowName
                        WS_POPUP | WS_SIZEBOX,        // dwStyle
                        CW_USEDEFAULT, CW_USEDEFAULT, // x, y
                        g_iWidth, g_iHeight,          // nWidth, nHeight
                        NULL,                         // hWndParent
                        NULL,                         // hMenu
                        hInstance,                    // hInstance
                        NULL);                        // lpParam

  // Extend glass to cover whole window
  DwmExtendFrameIntoClientArea(hWnd, &g_mgDWMMargins);

  // Initialise Direct3D
  if(SUCCEEDED(D3DStartup(hWnd)))
  {
    if(SUCCEEDED(CreateCube()))
    {
      // Show the window
      ShowWindow(hWnd, SW_SHOWDEFAULT);
      UpdateWindow(hWnd);
				
      // Enter main loop
      while(TRUE)
      {
        // Check for a message
        if(PeekMessage(&uMsg, NULL, 0, 0, PM_REMOVE))
        {
          // Check if the message is WM_QUIT
          if(uMsg.message == WM_QUIT)
          {
            // Break out of main loop
            break;
          }

          // Pump the message
          TranslateMessage(&uMsg);
          DispatchMessage(&uMsg);
        }

        // Render a frame
        Render();
      }
    }
  }

  // Shutdown Direct3D
  D3DShutdown();

  // Exit application
  return 0;
}