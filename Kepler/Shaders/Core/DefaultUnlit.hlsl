#include "Core.hlsl"

struct TDefaultUnlit_VSInput
{
	float3 Position : POSITION;
	float3 Color : COLOR;
};

struct TDefaultUnlit_PSInput
{
	float4 Position : SV_Position;
	float3 Color : ATTRIB1;
};

cbuffer TWorldViewProj : register(b0)
{
	float4x4 mWorldViewProj;
};

TDefaultUnlit_PSInput VSMain(in
	TDefaultUnlit_VSInput Input)
{
	TDefaultUnlit_PSInput Output;
	Output.Position = float4(Input.Position, 1.0f);
	Output.Position = mul(Output.Position, mWorldViewProj);
	
	Output.Color = Input.Color;
	return Output;
}

float4 PSMain(in TDefaultUnlit_PSInput Input) : SV_Target0
{
	return float4(Input.Color, 1.0f);
}