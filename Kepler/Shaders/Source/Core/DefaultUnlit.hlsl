void VSMain(in float3 Position : ATTRIBUTE0,
			out float4 OutPosition : SV_Position)
{
	OutPosition = float4(Position, 1.0f);
}