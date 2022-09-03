#include "Core.hlsl"

//////////////////////////////////////////////////////////////////
cbuffer TWorldViewProj : register(b0)
{
	float4x4 ViewProjection;
	float4x4 Transform;
};

//////////////////////////////////////////////////////////////////
TPixel VSMain(in TVertex Vertex)
{
	TPixel Output;
	Output.Position = float4(Vertex.Position, 1.0f);
	Output.Position = mul(Output.Position, Transform);
	Output.Position = mul(Output.Position, ViewProjection);
	
	Output.Color = Vertex.Color;
	Output.UV0 = Vertex.UV0;
	return Output;
}


//////////////////////////////////////////////////////////////////
SamplerState AlbedoSampler : register(s0);
Texture2D AlbedoTexture : register(t0);

float4 PSMain(in TPixel Input) : SV_Target0
{
	return AlbedoTexture.Sample(AlbedoSampler, Input.UV0);
}