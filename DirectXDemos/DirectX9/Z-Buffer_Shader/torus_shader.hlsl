
float4x4 matWorld : register(c0);	   
float4x4 matViewProj : register(c4); 

struct VS_INPUT
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float3 Normal : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 Color : COLOR0;
};

VS_OUTPUT VS(in VS_INPUT In)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	Out.Position = mul(In.Position, matWorld);
	Out.Position = mul(Out.Position, matViewProj);
	Out.Normal = normalize(mul(float4(In.Normal, 0), matWorld)).xyz;
 
	return Out;
};

PS_OUTPUT PS(in VS_OUTPUT In)
{
	PS_OUTPUT Out = (PS_OUTPUT)0;

	float4 ambient = {0.1, 0.0, 0.0, 1.0};
	float4 diffuse = {1.0, 0.0, 0.0, 1.0};
	float3 light = {0, 0, -1};
	Out.Color = ambient + diffuse * saturate(dot(light, In.Normal));

	return Out;
};