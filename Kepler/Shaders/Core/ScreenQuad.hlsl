#include "Core.hlsl"

//////////////////////////////////////////////////////////////////
TPixel VSMain(in TVertex Vertex)
{
	TPixel Output;
	Output.Position = float4(Vertex.Position, 1.0f);
	
	Output.Color = Vertex.Color;
	Output.UV0 = Vertex.UV0;
	return Output;
}


//////////////////////////////////////////////////////////////////
SamplerState RenderResult : register(s0);
Texture2D RenderResultTexture : register(t0);

float4 PSMain(in TPixel Input) : SV_Target0
{
	return RenderResultTexture.Sample(RenderResult, Input.UV0);
}