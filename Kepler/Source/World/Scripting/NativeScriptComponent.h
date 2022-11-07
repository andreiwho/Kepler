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

		template <typename C> static True Test(decltype(&C::Update));
		template <typename C> static False Test(...);

	public:
		enum { value = sizeof(Test<T>(nullptr)) == sizeof(char) };
	};

	template<typename T>
	class HasInitFunction
	{
		typedef u8 True;
		struct False { u8 _[2]; };

		template <typename C> static True Test(decltype(&C::Init));
		template <typename C> static False Test(...);

	public:
		enum { value = sizeof(Test<T>(nullptr)) == sizeof(True) };
	};

	template<typename T>
	class HasDestroyingFunction
	{
		typedef u8 True;
		struct False { u8 _[2]; };

		template <typename C> static True Test(decltype(&C::Destroying));
		template <typename C> static False Test(...);

	public:
		enum { value = sizeof(Test<T>(nullptr)) == sizeof(char) };
	};

	reflected kmeta(hideindetails)
	class NativeScriptComponent : public EntityComponent
	{
		reflection_info();
	};
}