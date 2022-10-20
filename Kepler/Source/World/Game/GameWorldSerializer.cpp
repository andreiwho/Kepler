#include "GameWorldSerializer.h"
#include "GameEntity.h"
#include "Helpers/EntityHelper.h"
#include "../Camera/CameraComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/Light/AmbientLightComponent.h"
#include "Components/Light/DirectionalLightComponent.h"

namespace ke
{

	GameWorldSerializer::GameWorldSerializer(RefPtr<GameWorld> pWorld)
		:	m_World(pWorld)
		,	m_Name(pWorld->GetName())
	{
		m_Data = SerializeWorld();
	}

	Array<SerializedEntityData> GameWorldSerializer::SerializeWorld()
	{
		KEPLER_PROFILE_SCOPE();
		Array<SerializedEntityData> outData;
		outData.Reserve(m_World->GetNumEntities());

		m_World->ForEachEntity(
			[&, this](GameEntityId entity)
			{
				auto handle = EntityHandle{ m_World, entity };
				if (handle->ShouldHideInSceneGraph())
				{
					return;
				}
				outData.EmplaceBack(std::move(GameWorldSerializer::SerializeEntity(handle)));
			});

		return outData;
	}

	SerializedEntityData GameWorldSerializer::SerializeEntity(EntityHandle& handle)
	{
		KEPLER_PROFILE_SCOPE();
		CHECK(!!handle);
		SerializedEntityData outData;
		outData.Id = handle->GetGUID();
		outData.Name = handle->GetName();
		outData.Transform = handle->GetTransform();
		outData.BuiltInComponents = {};

		//////////////////////////////////////////////////////////////
		// TODO: For best results this process has to be automated. //
		//////////////////////////////////////////////////////////////

		if (CameraComponent* pCameraComponent = handle.GetComponent<CameraComponent>())
		{
			outData.BuiltInComponents.Mask |= EBuiltInComponentMask::CameraComponent;
			SerializedCameraComponent sComp(pCameraComponent->GetCamera(), pCameraComponent->GetRenderTargetName());
			outData.OptCamera = std::move(sComp);
		}

		if (MaterialComponent* pMaterialComponent = handle.GetComponent<MaterialComponent>())
		{
			outData.BuiltInComponents.Mask |= EBuiltInComponentMask::MaterialComponent;
			SerializedMaterialComponent sComp{ pMaterialComponent->GetMaterialParentAssetPath() };
			outData.OptMaterial = std::move(sComp);
		}

		if (StaticMeshComponent* pStaticMeshComponent = handle.GetComponent<StaticMeshComponent>())
		{
			auto mesh = pStaticMeshComponent->GetStaticMesh();
			if (mesh->IsLoadedFromAsset())
			{
				outData.BuiltInComponents.Mask |= EBuiltInComponentMask::StaticMeshComponent;
				SerializedStaticMeshComponent sComp{ mesh->GetParentAssetPath(), mesh->IsForcedSingleSectionOnLoad() };
				outData.OptStaticMesh = std::move(sComp);
			}
		}

		if (AmbientLightComponent* pAmbientLightComponent = handle.GetComponent<AmbientLightComponent>())
		{
			outData.BuiltInComponents.Mask |= EBuiltInComponentMask::AmbientLightComponent;
			outData.OptAmbientLight = SerializedAmbientLightComponent{ pAmbientLightComponent->GetColor() };
		}

		if (DirectionalLightComponent* pDirectionalLightComponent = handle.GetComponent<DirectionalLightComponent>())
		{
			outData.BuiltInComponents.Mask |= EBuiltInComponentMask::DirectionalLightComponent;
			outData.OptDirectionalLight = SerializedDirectionalLightComponent{
				pDirectionalLightComponent->GetColor(),
				pDirectionalLightComponent->GetIntensity()
			};
		}

		return outData;
	}

}