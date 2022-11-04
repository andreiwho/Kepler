#include "Core/ShaderCore.hlsl"

//////////////////////////////////////////////////////////////////
struct TVertex
{
	float3 Position : POSITION0;
	float3 Color : COLOR0;
	float2 UV0 : TEXCOORD0;
};

//////////////////////////////////////////////////////////////////
struct TPixel
{
	float4 Position : SV_Position;
	float3 Color : COLOR0;
	float2 UV0 : TEXCOORD0;
};

SamplerState RenderTarget : register(s0);
Texture2D tRenderTarget : register(t0);

//////////////////////////////////////////////////////////////////
TPixel VSMain(in TVertex Vertex)
{
	TPixel Output;
	Output.Position = float4(Vertex.Position, 1.0f);
	
	Output.Color = Vertex.Color;
	Output.UV0 = Vertex.UV0;
	return Output;
}

float3 ReinhardToneMapping(float3 hdr)
{
	float3 mapped = float3(1.0f, 1.0f, 1.0f) - exp(-hdr * Exposure);
	float oneOverGamma = 1.0f / Gamma;
	mapped = pow(mapped, float3(oneOverGamma, oneOverGamma, oneOverGamma));
	return mapped;
}

//////////////////////////////////////////////////////////////////
float4 PSMain(in TPixel Input) : SV_Target0
{
	// Apply gamma correction
	float3 hdr = tRenderTarget.Sample(RenderTarget, Input.UV0).rgb;
	float3 outColor = ReinhardToneMapping(hdr);
	return float4(outColor, 1.0f);
}