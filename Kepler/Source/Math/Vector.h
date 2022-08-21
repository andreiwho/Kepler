#pragma once
#include "MathCommon.h"

namespace Kepler
{
	template<typename T, usize V>
	struct TVectorType
	{
	};

	template<typename T>
	struct TVectorType<T, 2>
	{
		constexpr TVectorType() = default;
		constexpr TVectorType(T InX, T InY) : X(InX), Y(InY) {}
		constexpr explicit TVectorType(T Scalar) : TVectorType(Scalar, Scalar) {}
		constexpr explicit TVectorType(typename Handles::TSelectHandle<T, 2>::Type Value) : X(Value.x), Y(Value.y) {}

		T X, Y;

		inline constexpr typename Handles::TSelectHandle<T, 2>::Type ToHandle() const { return { X, Y }; }
		inline b128 Pack() const { return Handles::Pack<T, 2>(ToHandle()); }

		inline bool Unpack(b128 Simd) {
			typename Handles::TSelectHandle<T, 2>::Type Value;
			bool bSucceeded = Handles::Unpack(Simd, &Value);
			new(this) TVectorType<T, 2>(Value);
			return bSucceeded;
		}
	};

	template<typename T>
	struct TVectorType<T, 3>
	{
		constexpr TVectorType() = default;
		constexpr TVectorType(T InX, T InY, T InZ) : X(InX), Y(InY), Z(InZ) {}
		constexpr explicit TVectorType(T Scalar) : TVectorType(Scalar, Scalar, Scalar) {}
		constexpr explicit TVectorType(typename Handles::TSelectHandle<T, 3>::Type Value) : X(Value.x), Y(Value.y), Z(Value.z) {}

		T X, Y, Z;
		inline constexpr typename Handles::TSelectHandle<T, 3>::Type ToHandle() const { return { X, Y, Z }; }
		inline b128 Pack() const { return Handles::Pack<T, 3>(ToHandle()); }
		
		inline bool Unpack(b128 Simd) { 
			typename Handles::TSelectHandle<T, 3>::Type Value; 
			bool bSucceeded = Handles::Unpack(Simd, &Value);  
			new(this) TVectorType<T, 3>(Value);
			return bSucceeded; 
		}
	};

	template<typename T>
	struct TVectorType<T, 4>
	{
		constexpr TVectorType() = default;
		constexpr TVectorType(T InX, T InY, T InZ, T InW = 1.0f) : X(InX), Y(InY), Z(InZ), W(InW) {}
		constexpr explicit TVectorType(T Scalar) : TVectorType(Scalar, Scalar, Scalar, Scalar) {}
		constexpr explicit TVectorType(typename Handles::TSelectHandle<T, 4>::Type Value) : X(Value.x), Y(Value.y), Z(Value.z), W(Value.w) {}

		T X, Y, Z, W;
		inline constexpr typename Handles::TSelectHandle<T, 4>::Type ToHandle() const { return { X, Y, Z, W }; }
		inline b128 Pack() const { return Handles::Pack<T, 4>(ToHandle()); }

		inline bool Unpack(b128 Simd) {
			typename Handles::TSelectHandle<T, 4>::Type Value;
			bool bSucceeded = Handles::Unpack(Simd, &Value);
			new(this) TVectorType<T, 4>(Value);
			return bSucceeded;
		}
	};

	using TVector2 = TVectorType<float, 2>;
	using TVector3 = TVectorType<float, 3>;
	using TVector4 = TVectorType<float, 4>;

	using TIntVector2 = TVectorType<i32, 2>;
	using TIntVector3 = TVectorType<i32, 3>;
	using TIntVector4 = TVectorType<i32, 4>;

	using TUIntVector2 = TVectorType<u32, 2>;
	using TUIntVector3 = TVectorType<u32, 3>;
	using TUIntVector4 = TVectorType<u32, 4>;
}
