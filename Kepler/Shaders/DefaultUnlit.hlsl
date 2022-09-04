#include "Core/Unlit.hlsl"

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
float4 PSMain(in TPixel Input) : SV_Target0
{
	return AlbedoTexture.Sample(AlbedoSampler, Input.UV0);
}