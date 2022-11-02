//////////////////////////////////////////////////////////////////
cbuffer TEntity : register(RS_User)
{
	float4x4 Transform;
	float3x3 NormalMatrix;
	int EntityId;
};

cbuffer TCamera : register(RS_Camera)
{
	float4x4 ViewProjection;
};

cbuffer TLight : register(RS_Light)
{
	float4 Ambient;
	float4 DirectionalLightDirection;
	float4 DirectionalLightColor;
	float DirectionalLightIntensity;
};

float3 CalculateAmbient()
{
	return Ambient.xyz;
}

float3 CalculateDirection(float3 normal, float3 lightDir)
{
	// const float3 lightDir = mul(DirectionalLightDirection.xyz, TBN);
	float3 diffuseImpact = max(dot(normal, -lightDir), 0.0f);
	return diffuseImpact * DirectionalLightColor.xyz * DirectionalLightIntensity;
}

float3 CalculateLighting(float3 normal, float3 lightDir)
{
	return CalculateAmbient() + CalculateDirection(normal, lightDir);
}