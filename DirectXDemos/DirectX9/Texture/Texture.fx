// World, View and Projection Matrix
uniform extern float4x4 gWVP; 
texture g_pCubeTexture;

sampler CubeTextureSampler =
	sampler_state
{
	Texture = <g_pCubeTexture>;
	MipFilter = LINEAR;	
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

// Output Vertex structure
struct OutputVS
{
	float4 posH : POSITION0;
	float2 TextureUV : TEXCOORD0;
};

OutputVS BasicVS(float4 posL : POSITION0, float2 vTexture : TEXCOORD0)
{

	// Zero out our output.
	OutputVS outVS = (OutputVS)0;

	// Transform to homogeneous clip space.
	outVS.posH = mul(posL, gWVP);
	outVS.TextureUV = vTexture;

	// Done--return the output.
	return outVS;
}

struct PS_OUTPUT
{
	float4 RGBColor : COLOR0; // Pixel color
} ;

PS_OUTPUT BasicPS(OutputVS In)
{
	PS_OUTPUT Output = (PS_OUTPUT)0;
	Output.RGBColor = tex2D(CubeTextureSampler, In.TextureUV);
	return Output;
}

technique Tech1
{
	pass p0
	{
		vertexShader = compile vs_2_0 BasicVS();
		pixelShader  = compile ps_2_0 BasicPS();
	}
}
