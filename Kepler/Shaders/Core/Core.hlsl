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