#pragma once
#include "../Game/Components/EntityComponent.h"

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

	class NativeScriptComponent : public EntityComponent
	{
	};
}