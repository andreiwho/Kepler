//////////////////////////////////////////////////////////////////
struct TVertex
{
	float3 Position : POSITION0;
	float3 Color : COLOR0;
	float2 UV0 : TEXCOORD0;
};

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

//////////////////////////////////////////////////////////////////
float4 PSMain(in TPixel Input) : SV_Target0
{
	return tRenderTarget.Sample(RenderTarget, Input.UV0);
}