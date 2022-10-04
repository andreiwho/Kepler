struct TVertex
{
	float3 Position : POSITION0;
	float3 Color : COLOR0;
	float2 UV0 : TEXCOORD0;
	float3 Normal : NORMAL0;
};

cbuffer TCamera : register(RS_Camera)
{
	float4x4 ViewProjection;
};

//////////////////////////////////////////////////////////////////
cbuffer TConstants : register(RS_User)
{
	float4x4 Transform;
	int EntityId;
};

//////////////////////////////////////////////////////////////////
float4 VSMain(in TVertex Vertex) : SV_Position
{
	float4 Output;
	Output = float4(Vertex.Position, 1.0f);
	Output = mul(Output, Transform);
	Output = mul(Output, ViewProjection);
	return Output;
}