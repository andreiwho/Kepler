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
	}
}