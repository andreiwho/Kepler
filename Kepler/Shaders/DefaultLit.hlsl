#include "Core/ShaderCore.hlsl"

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