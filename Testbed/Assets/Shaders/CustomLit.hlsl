//////////////////////////////////////////////////////////////////
cbuffer TRenderer : register(RS_Renderer)
{
	float Gamma;
	float Exposure;
	float BloomThereshold;
	float BloomStrength;
};

//////////////////////////////////////////////////////////////////
cbuffer TEntity : register(RS_User)
{
	float4x4 Transform;
	float3x3 NormalMatrix;
	float3 CustomData;
	float2 UVTiling;
	float4 Offset;
	int EntityId;
};

//////////////////////////////////////////////////////////////////
cbuffer TCamera : register(RS_Camera)
{
	float4x4 ViewProjection;
};

//////////////////////////////////////////////////////////////////
cbuffer TLight : register(RS_Light)
{
	float4 Ambient;
	float4 DirectionalLightDirection;
	float4 DirectionalLightColor;
	float DirectionalLightIntensity;
};

//////////////////////////////////////////////////////////////////
float3 CalculateAmbient()
{
	return Ambient.xyz;
}

//////////////////////////////////////////////////////////////////
float3 CalculateDirection(float3 normal, float3 lightDir)
{
	// const float3 lightDir = mul(DirectionalLightDirection.xyz, TBN);
	float3 diffuseImpact = max(dot(normal, -lightDir), 0.0f);
	return diffuseImpact * DirectionalLightColor.xyz * DirectionalLightIntensity;
}

//////////////////////////////////////////////////////////////////
float3 CalculateLighting(float3 normal, float3 lightDir)
{
	return CalculateAmbient() + CalculateDirection(normal, lightDir);
}

//////////////////////////////////////////////////////////////////
float4 MakeFloat4(float scalar) { return float4(scalar, scalar, scalar, scalar); }
//////////////////////////////////////////////////////////////////
float3 MakeFloat3(float scalar) { return float3(scalar, scalar, scalar); }
//////////////////////////////////////////////////////////////////
float2 MakeFloat2(float scalar) { return float2(scalar, scalar); }

struct TVertex
{
	float3 Position : POSITION0;
	float3 Color : COLOR0;
	float2 UV0 : TEXCOORD0;
	float3 Normal : NORMAL0;
	float3 Tangent : TANGENT0;
	float3 Bitangent : BITANGENT0;
};

struct TPixel
{
	float4 Position : SV_Position;
	float3 Color : COLOR0;
	float2 UV0 : TEXCOORD0;
	float3 Normal : NORMAL0;
	float3 PixelPos : PIXELPOS0;
	float3x3 TBN : TBN0;
	float3 TangentSpaceLight : TGLIGHT0;
	int Id : ENTITY_ID0;
};

//////////////////////////////////////////////////////////////////
SamplerState Albedo : register(s0);
Texture2D AlbedoTexture : register(t0);

SamplerState Normal : register(s1);
Texture2D NormalMap : register(t1); 

//////////////////////////////////////////////////////////////////
TPixel VSMain(in TVertex Vertex)
{
	TPixel Output;
	Output.PixelPos = mul(float4(Vertex.Position, 1.0f), Transform).xyz;
	Output.Position = mul(float4(Output.PixelPos, 1.0f), ViewProjection);
	
	Output.Normal =  mul(Vertex.Normal, NormalMatrix);

	Output.Color = Vertex.Color;
	Output.UV0 = Vertex.UV0;
	Output.Id = EntityId;

	float3 T = normalize(mul(float4(Vertex.Tangent, 0.0f), Transform)).xyz;
	float3 N = normalize(mul(float4(Vertex.Normal, 0.0f), Transform)).xyz;
	T = normalize(T - dot(T, N) * N);
	float3 B = cross(N, T);
	Output.TBN = transpose(float3x3(T, B, N));

	Output.TangentSpaceLight = mul(DirectionalLightDirection.xyz, Output.TBN);

	return Output;
}

//////////////////////////////////////////////////////////////////
void PSMain(in TPixel Input, out float4 OutPixel : SV_Target0, out int OutEntityId : SV_Target1)
{
	// float3 lightingData = CalculateLighting(Input.Normal, Input.PixelPos);
	float3 normal = NormalMap.Sample(Normal, Input.UV0).xyz;
	normal = normalize(normal * 2.0f -1.0f);

	float3 lightingData = CalculateLighting(normal, Input.TangentSpaceLight);
	OutPixel = float4(lightingData, 1.0f) * AlbedoTexture.Sample(Albedo, Input.UV0);
	OutEntityId = Input.Id;
}