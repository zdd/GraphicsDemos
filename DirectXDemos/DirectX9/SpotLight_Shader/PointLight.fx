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

technique Tech1
{
	pass p0
    {
		/////////////////////////////////////
		//           Shaders               //
		/////////////////////////////////////
		vertexShader = null;
		pixelShader = null;

		/////////////////////////////////////
		//           Transform             //
		/////////////////////////////////////
		
		// Setup world matrix
		WorldTransform[0] = BuildWorldMatrix(0.0f, 0.0f, 0.0f);

		// Setup view matrix
		ViewTransform = BuildViewMatrix(
			float3(0.0f, 0.0f, -5.0f),
			float3(0.0f, 0.0f,  0.0f),
			float3(0.0f, 1.0f,  1.0f));

		// Setup projection matrix
		ProjectionTransform = BuildProjectionMatrix( 
			3.1415926 / 4, 
			1.0f, 
			1.0f, 
			1000.0f);

		/////////////////////////////////////
		//           Material              //
        /////////////////////////////////////
		MaterialAmbient  = float3(1.0f, 0.0f, 0.0f);
		MaterialDiffuse  = float3(1.0f, 0.0f, 0.0f);
		MaterialSpecular = float3(1.0f, 0.0f, 0.0f);
		MaterialEmissive = float3(0.0f, 0.0f, 0.0f);

		/////////////////////////////////////
		//           Lighting              //           
		/////////////////////////////////////
		// Enable lighting
		Lighting = True;
		LightEnable[0] = True;
		LightType[0] = POINT;

		// Light color is white
		LightAmbient[0]  = {0.6f, 0.6f, 0.6f, 0.0f};
		LightDiffuse[0]  = {1.0f, 1.0f, 1.0f, 0.0f};
		LightSpecular[0] = {0.6f, 0.6f, 0.6f, 0.0f};

		// Light position
		LightPosition[0] = {10.0f, 10.0f, -5.0f};

		// Light range
		LightRange[0] = 1000.0f;

		// Light falloff
		LightFalloff[0] = 1.0f;

		// Attenuations
		LightAttenuation0[0] = 1.0f;
		LightAttenuation1[0] = 0.0f;
		LightAttenuation2[0] = 0.0f;
    }
}
