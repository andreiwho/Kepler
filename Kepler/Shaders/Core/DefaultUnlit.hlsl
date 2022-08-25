#include "Core.hlsl"

//////////////////////////////////////////////////////////////////
cbuffer TWorldViewProj : register(b0)
{
	float4x4 mWorldViewProj;
};

//////////////////////////////////////////////////////////////////
TPixel VSMain(in TVertex Vertex)
{
	TPixel Output;
	Output.Position = float4(Vertex.Position, 1.0f);
	Output.Position = mul(Output.Position, mWorldViewProj);
	
	Output.Color = Vertex.Color;
	return Output;
}


//////////////////////////////////////////////////////////////////
float4 PSMain(in TPixel Input) : SV_Target0
{
	return float4(Input.Color, 1.0f);
}