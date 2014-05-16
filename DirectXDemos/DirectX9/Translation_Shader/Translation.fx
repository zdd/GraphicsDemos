
// Delta values of translation
uniform extern float g_hoffsetX;
uniform extern float g_hoffsetY;

// Build the world matrix
float4x4 BuildWorldMatrix(float Tx, float Ty, float Tz)
{
	float4x4 matWorld = {
		1.0f, 0.0f,  0.0f, 0.0f,
		0.0f, 1.0f , 0.0f, 0.0f,
		0.0f, 0.0f,  1.0f, 0.0f,
	      Tx,   Ty,    Tz, 1.0f,
	};

	return matWorld;
}

// Build the view matrix
float4x4 BuildViewMatrix(float3 eyePt, float3 lookAt, float3 Up)
{
	float3 p = eyePt;
	float3 u = Up;

	// Calculate the direction of the view sight
	float3 d = lookAt - eyePt;
	d = normalize(d);

	// Calculate the right vector
	float3 r = -cross(d, u);
	r = normalize(r);

	// Calculate the up vector
	u = -cross(r, d);
	u = normalize(u);

	float x = -dot(p, r);
	float y = -dot(p, u);
	float z = -dot(p, d);

	// Fill in the view matrix
	float4x4 M;
	M[0][0] = r.x; M[0][1] = u.x; M[0][2] = d.x; M[0][3] = 0.0f;
	M[1][0] = r.y; M[1][1] = u.y; M[1][2] = d.y; M[1][3] = 0.0f;
	M[2][0] = r.z; M[2][1] = u.z; M[2][2] = d.z; M[2][3] = 0.0f;
	M[3][0] = x;   M[3][1] = y;   M[3][2] = z;   M[3][3] = 1.0f;

	return M;
}

// Build perspective projection matrix
float4x4 BuildProjectionMatrix(float fov, float aspect, float zn, float zf)
{
	float4x4 proj = 0;

	proj[0][0] = 1 / (tan(fov * 0.5f) *aspect) ;
	proj[1][1] = 1 / tan(fov * 0.5f) ;
	proj[2][2] = zf / (zf - zn) ;
	proj[2][3] = 1.0f; 
	proj[3][2] = (zn * zf) / (zn - zf);

	return proj ;
}

// Get the product of world, view and projection matrix.
float4x4 BuildWVP(float4x4 world, float4x4 view, float4x4 proj)
{
	//return world * view * proj; // DO NOT use *, use mul.
	return mul(mul(world, view), proj);
}

// Output Vertex structure
struct OutputVS
{
      float4 posH : POSITION0;
};

OutputVS BasicVS(float4 posL : POSITION0)
{
	// Zero out our output.
    OutputVS outVS = (OutputVS)0;

	// Build world matrix
	float4x4 matWorld = BuildWorldMatrix(g_hoffsetX, g_hoffsetY, 0.0f);

	// Build view matrix
	float3      eyePt = {0.0f, 0.0f, -10.0f} ;
	float3 lookCenter = {0.0f, 0.0f, 0.0f} ;
	float3      upVec = {0.0f, 1.0f, 0.0f} ;
	float4x4  matView = BuildViewMatrix(eyePt, lookCenter, upVec);

	// Build projection matrix
	float viewAngle = radians(45); // PI / 4;
	float4x4 matProj = BuildProjectionMatrix(viewAngle, 1.0f, 1.0f, 1000);

	float4x4 matWVP = BuildWVP(matWorld, matView, matProj);

	//float4x4 mat_wvp = BuildWVP(g_hWorldMatrix, g_hViewMatrix, g_hProjMatrix);
	outVS.posH = mul(posL, matWVP);

	// Done--return the output.
	return outVS;
}

float4 BasicPS() : COLOR
{
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}

technique Tech1
{
	pass p0
    {
		FillMode = WireFrame;
		vertexShader = compile vs_2_0 BasicVS();
		pixelShader  = compile ps_2_0 BasicPS();
    }
}
