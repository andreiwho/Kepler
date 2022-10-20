#pragma once
#include "GameWorld.h"
#include "Renderer/World/WorldTransform.h"
#include "Renderer/World/Camera.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	struct EBuiltInComponentMask
	{
		enum EValues
		{
			CameraComponent = BIT(0),
			StaticMeshComponent = BIT(1),
			MaterialComponent = BIT(2),
			AmbientLightComponent = BIT(3),
			DirectionalLightComponent = BIT(4),
		};

		u32 Mask{0};

		EBuiltInComponentMask() = default;
		EBuiltInComponentMask(u32 mask) : Mask(mask){}
		inline operator u32() { return Mask; }
	};

	//////////////////////////////////////////////////////////////////////////
	struct SerializedCameraComponent
	{
		SerializedCameraComponent(const MathCamera& camera, const String& renderTarget)
			:	Camera(camera)
			,	RenderTarget(renderTarget)
		{}

		MathCamera Camera;
		String RenderTarget;
	};

	//////////////////////////////////////////////////////////////////////////
	struct SerializedMaterialComponent
	{
		String AssetName;
	};

	//////////////////////////////////////////////////////////////////////////
	struct SerializedStaticMeshComponent
	{
		String AssetName;
		bool bForcedSingleSection;
	};

	//////////////////////////////////////////////////////////////////////////
	struct SerializedAmbientLightComponent
	{
		float3 Color;
	};

	//////////////////////////////////////////////////////////////////////////
	struct SerializedDirectionalLightComponent
	{
		float3 Color;
		float Intensity;
	};

	//////////////////////////////////////////////////////////////////////////
	struct SerializedEntityData
	{
		id64 Id;
		String Name;
		WorldTransform Transform;

		EBuiltInComponentMask BuiltInComponents;
		Option<SerializedCameraComponent> OptCamera;
		Option<SerializedMaterialComponent> OptMaterial;
		Option<SerializedStaticMeshComponent> OptStaticMesh;
		Option<SerializedAmbientLightComponent> OptAmbientLight;
		Option<SerializedDirectionalLightComponent> OptDirectionalLight;
	};
	 
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class GameWorldSerializer
	{
	public:
		GameWorldSerializer(RefPtr<GameWorld> pWorld);

		inline const Array<SerializedEntityData>& GetSerializedEntityData() const& { return m_Data; }
		inline const String& GetWorldName() const& { return m_Name; }

	private:
		static SerializedEntityData SerializeEntity(class EntityHandle& handle);
		Array<SerializedEntityData> SerializeWorld();
		
	private:
		RefPtr<GameWorld> m_World;
		Array<SerializedEntityData> m_Data;
		String m_Name;
	};
}