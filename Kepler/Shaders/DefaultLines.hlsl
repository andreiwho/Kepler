struct TVertex
{
	float3 Position : POSITION0;
	float3 Color : COLOR0;
};

struct TPixel
{
	float4 Position : SV_Position;
	float3 Color : COLOR0;
};

cbuffer TCamera : register(RS_Camera)
{
	float4x4 ViewProjection;
};

//////////////////////////////////////////////////////////////////
TPixel VSMain(in TVertex Vertex)
{
	TPixel Output;
	Output.Position = mul(float4(Vertex.Position, 1.0f), ViewProjection);
	Output.Color = Vertex.Color;	
	return Output;
}

//////////////////////////////////////////////////////////////////
void PSMain(in TPixel Input, out float4 OutPixel : SV_Target0)
{
	OutPixel = float4(Input.Color, 1.0f);
}