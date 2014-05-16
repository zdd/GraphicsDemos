// World, View and Projection Matrix
uniform extern float4x4 gWVP; 

// Output Vertex structure
struct OutputVS
{
      float4 posH : POSITION0;
};

OutputVS BasicVS(float4 posL : POSITION0)
{

      // Zero out our output.
      OutputVS outVS = (OutputVS)0;

      // Transform to homogeneous clip space.
      outVS.posH = mul(posL, gWVP);

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
