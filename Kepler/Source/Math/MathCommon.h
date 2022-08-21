#pragma once
#include "Core/Types.h"
#include "Core/Macros.h"
#include <type_traits>
#include <tuple>

namespace Kepler
{
	template<usize Index, typename... Types>
	using TStaticSwitch = typename std::tuple_element<Index, std::tuple<Types...>>::type;
}

#ifdef WIN32
# ifdef SUPPORTS_SIMD
#  ifdef USE_DIRECTX_MATH
#   include <DirectXPackedVector.h>
namespace Kepler
{
	using b128 = DirectX::XMVECTOR;
	namespace Handles
	{
		using _vec2f = DirectX::XMFLOAT2;
		using _vec3f = DirectX::XMFLOAT3;
		using _vec4f = DirectX::XMFLOAT4;

		using _vec2i = DirectX::XMINT2;
		using _vec3i = DirectX::XMINT3;
		using _vec4i = DirectX::XMINT4;

		using _vec2u = DirectX::XMUINT2;
		using _vec3u = DirectX::XMUINT3;
		using _vec4u = DirectX::XMUINT4;
	}
}
#  else
#   error Other math library backends except DirectXMath are currently unsupported
#  endif
# else
#  error SSE support required
# endif
#else
# error SSE intrinsics unsupported on this platform, turn them off by compiler flag
#endif

namespace Kepler
{
	namespace Handles
	{
		template<typename T, usize Size>
		struct TSelectHandle
		{
			using Type = void;
		};

		template<usize Size>
		struct TSelectHandle<float, Size>
		{
			static_assert(Size > 0 && Size <= 4);
			using Type = TStaticSwitch<(Size > 0) ? Size - 1 : 0, float, _vec2f, _vec3f, _vec4f>;
		};

		template<usize Size>
		struct TSelectHandle<i32, Size>
		{
			static_assert(Size > 0 && Size <= 4);
			using Type = TStaticSwitch<(Size > 0) ? Size - 1 : 0, i32, _vec2i, _vec3i, _vec4i>;
		};

		template<usize Size>
		struct TSelectHandle<u32, Size>
		{
			static_assert(Size > 0 && Size <= 4);
			using Type = TStaticSwitch<(Size > 0) ? Size - 1 : 0, u32, _vec2u, _vec3u, _vec4u>;
		};

		template<typename T, usize Size>
		b128 Pack(typename TSelectHandle<T, Size>::Type Value)
		{
#ifdef USE_DIRECTX_MATH
			if constexpr (std::is_same_v<T, float>)
			{
				TStaticSwitch < Size - 1,
					decltype([](float* Val) { CRASH(); }),
					decltype([](_vec2f* Val) { return DirectX::XMLoadFloat2(Val); }),
					decltype([](_vec3f* Val) { return DirectX::XMLoadFloat3(Val); }),
					decltype([](_vec4f* Val) { return DirectX::XMLoadFloat4(Val); }) > Loader;
				return Loader(&Value);
			}

			if constexpr (std::is_same_v<T, i32>)
			{
				TStaticSwitch < Size - 1,
					decltype([](i32* Val) { CRASH(); }),
					decltype([](_vec2i* Val) { return DirectX::XMLoadSInt2(Val); }),
					decltype([](_vec3i* Val) { return DirectX::XMLoadSInt3(Val); }),
					decltype([](_vec4i* Val) { return DirectX::XMLoadSInt4(Val); }) > Loader;
				return Loader(&Value);
			}

			if constexpr (std::is_same_v<T, u32>)
			{
				TStaticSwitch < Size - 1,
					decltype([](u32* Val) { CRASH(); }),
					decltype([](_vec2u* Val) { return DirectX::XMLoadUInt2(Val); }),
					decltype([](_vec3u* Val) { return DirectX::XMLoadUInt3(Val); }),
					decltype([](_vec4u* Val) { return DirectX::XMLoadUInt4(Val); }) > Loader;
				return Loader(&Value);
			}
			return {};
#else
# error Other math library backends except DirectXMath are currently unsupported
#endif
		}

		template<typename T, usize Size>
		bool Unpack(b128 Value, typename TSelectHandle<T, Size>::Type* OutValue)
		{
			if (!OutValue)
			{
				return false;
			}

			if constexpr (std::is_same_v<T, float>)
			{
				TStaticSwitch < Size - 1,
					decltype([](b128 Val, float* Out) { CRASH(); }),
					decltype([](b128 Val, _vec2f* Out) { DirectX::XMStoreFloat2(Out, Val); }),
					decltype([](b128 Val, _vec3f* Out) { DirectX::XMStoreFloat3(Out, Val); }),
					decltype([](b128 Val, _vec4f* Out) { DirectX::XMStoreFloat4(Out, Val); }) > Storer;
				Storer(Value, OutValue);
			}

			if constexpr (std::is_same_v<T, i32>)
			{
				TStaticSwitch < Size - 1,
					decltype([](b128 Val, i32* Out) { CRASH(); }),
					decltype([](b128 Val, _vec2i* Out) { DirectX::XMStoreSInt2(Out, Val); }),
					decltype([](b128 Val, _vec3i* Out) { DirectX::XMStoreSInt3(Out, Val); }),
					decltype([](b128 Val, _vec4i* Out) { DirectX::XMStoreSInt4(Out, Val); }) > Storer;
				Storer(Value, OutValue);
			}

			if constexpr (std::is_same_v<T, u32>)
			{
				TStaticSwitch < Size - 1,
					decltype([](b128 Val, u32* Out) { CRASH(); }),
					decltype([](b128 Val, _vec2u* Out) { return DirectX::XMStoreInt2(Out, Val); }),
					decltype([](b128 Val, _vec3u* Out) { return DirectX::XMStoreInt3(Out, Val); }),
					decltype([](b128 Val, _vec4u* Out) { return DirectX::XMStoreInt4(Out, Val); }) > Storer;
				Storer(Value, OutValue);
			}

			return true;
		}
	}
	
	namespace VectorOperations
	{
		template<typename T, usize Size>
		using TVecHandle = typename Handles::TSelectHandle<T, Size>::Type;

		template<typename T, usize Size>
		typename TVecHandle<T, Size> Add(TVecHandle<T, Size> Lhs, TVecHandle<T, Size> Rhs)
		{
#ifdef USE_DIRECTX_MATH
			TVecHandle<T, Size> OutHandle;
			auto Simd = DirectX::XMVectorAdd(Handles::Pack<T, Size>(Lhs), Handles::Pack<T, Size>(Rhs));
			Handles::Unpack<T, Size>(Simd, &OutHandle);
			return OutHandle;
#endif
		}
	}
}