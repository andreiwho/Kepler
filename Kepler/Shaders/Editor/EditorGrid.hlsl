struct TVertex
{
	float3 Position : POSITION0;
	float3 Color : COLOR0;
	float2 UV0 : TEXCOORD0;
	float3 Normal : NORMAL0;
};

struct TPixel
{
	float4 Position : SV_Position;
	float3 Color : COLOR0;
	float2 UV0 : TEXCOORD0;
	float3 Normal : NORMAL0;
	int Id : ENTITY_ID0;
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
TPixel VSMain(in TVertex Vertex)
{
	TPixel Output;
	Output.Position = float4(Vertex.Position, 1.0f);
	Output.Position = mul(Output.Position, Transform);
	Output.Position = mul(Output.Position, ViewProjection);
	
	Output.Color = Vertex.Color;
	Output.Normal = Vertex.Normal;
	Output.UV0 = Vertex.UV0;
	Output.Id = EntityId;
	return Output;
}

//////////////////////////////////////////////////////////////////
void PSMain(in TPixel Input, out float4 OutPixel : SV_Target0, out int OutEntityId : SV_Target1)
{
	OutPixel = float4(0.2f, 0.2f, 0.2f, 1.0f);
	OutEntityId = -1;
}