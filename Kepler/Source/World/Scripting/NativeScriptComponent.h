#pragma once
#include "../Game/Components/EntityComponent.h"
#include "NativeScriptComponent.gen.h"

namespace ke
{
	template<typename T>
	class HasUpdateFunction
	{
		typedef u8 True;
		struct False { u8 _[2]; };

		template <typename C> static True test(decltype(&C::Update));
		template <typename C> static False test(...);

	public:
		enum { value = sizeof(test<T>(nullptr)) == sizeof(char) };
	};

	template<typename T>
	class HasInitFunction
	{
		typedef u8 True;
		struct False { u8 _[2]; };

		template <typename C> static True test(decltype(&C::Init));
		template <typename C> static False test(...);

	public:
		enum { value = sizeof(test<T>(nullptr)) == sizeof(True) };
	};

	template<typename T>
	class HasDestroyingFunction
	{
		typedef u8 True;
		struct False { u8 _[2]; };

		template <typename C> static True test(decltype(&C::Destroying));
		template <typename C> static False test(...);

	public:
		enum { value = sizeof(test<T>(nullptr)) == sizeof(char) };
	};

	reflected class NativeScriptComponent : public EntityComponent
	{
	};

	reflected class NativeTestComponent : public NativeScriptComponent
	{
	public:
		reflected float TestValue = 15.0f;
	};

	reflected class OtherNativeTestComponent : public NativeTestComponent
	{
	public:
		reflected float3 OtherTestFloat3 = { 10.0f, 0.0f, 0.0f };
		reflected bool bCheckThisOut = false;
	};
}