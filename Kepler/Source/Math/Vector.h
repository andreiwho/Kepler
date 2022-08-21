#pragma once
#include "MathCommon.h"

namespace Kepler
{
	//////////////////////////////////////////////////////////////////////////
	template<typename T, usize V>
	struct TVectorType
	{
	};

	//////////////////////////////////////////////////////////////////////////
	template<typename T>
	struct TVectorType<T, 2>
	{
		constexpr TVectorType() = default;
		constexpr TVectorType(T InX, T InY) : X(InX), Y(InY) {}
		constexpr explicit TVectorType(T Scalar) : TVectorType(Scalar, Scalar) {}
		constexpr explicit TVectorType(typename Handles::TSelectHandle<T, 2>::Type Value) : X(Value.x), Y(Value.y) {}
		constexpr TVectorType(TVectorType<T, 3> Other) : X(Other.X), Y(Other.Y) {}
		constexpr TVectorType(TVectorType<T, 4> Other) : X(Other.X), Y(Other.Y) {}

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


	//////////////////////////////////////////////////////////////////////////
	template<typename T>
	struct TVectorType<T, 3>
	{
		constexpr TVectorType() = default;
		constexpr TVectorType(T InX, T InY, T InZ) : X(InX), Y(InY), Z(InZ) {}
		constexpr explicit TVectorType(T Scalar) : TVectorType(Scalar, Scalar, Scalar) {}
		constexpr explicit TVectorType(typename Handles::TSelectHandle<T, 3>::Type Value) : X(Value.x), Y(Value.y), Z(Value.z) {}
		constexpr TVectorType(TVectorType<T, 2> Other, T InZ = 0.0f) : X(Other.X), Y(Other.Y), Z(InZ) {}
		constexpr TVectorType(TVectorType<T, 4> Other, T InZ = 0.0f) : X(Other.X), Y(Other.Y), Z(Other.Z) {}

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


	//////////////////////////////////////////////////////////////////////////
	template<typename T>
	struct TVectorType<T, 4>
	{
		constexpr TVectorType() = default;
		constexpr TVectorType(T InX, T InY, T InZ, T InW = 1.0f) : X(InX), Y(InY), Z(InZ), W(InW) {}
		constexpr TVectorType(TVectorType<T, 2> Other, T InZ = 0.0f, T InW = 1.0f) : X(Other.X), Y(Other.Y), Z(InZ), W(InW) {}
		constexpr TVectorType(TVectorType<T, 3> Other, T InW = 1.0f) : X(Other.X), Y(Other.Y), Z(Other.Z), W(InW) {}

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

	//////////////////////////////////////////////////////////////////////////
	using float2 = TVectorType<float, 2>;
	using float3 = TVectorType<float, 3>;
	using float4 = TVectorType<float, 4>;

	using int2 = TVectorType<i32, 2>;
	using int3 = TVectorType<i32, 3>;
	using int4 = TVectorType<i32, 4>;

	using uint2 = TVectorType<u32, 2>;
	using uint3 = TVectorType<u32, 3>;
	using uint4 = TVectorType<u32, 4>;

	//////////////////////////////////////////////////////////////////////////
	template<typename T, usize Size>
	TVectorType<T, Size> Normalize(TVectorType<T, Size> Lhs)
	{
		return TVectorType<T, Size>(VectorOperations::Normalize<T, Size>(Lhs.ToHandle()));
	}

	template<typename T, usize Size>
	float Length(TVectorType<T, Size> Lhs)
	{
		return VectorOperations::Length<T, Size>(Lhs.ToHandle());
	}

	template<typename T, usize Size>
	float LengthSquared(TVectorType<T, Size> Lhs)
	{
		return VectorOperations::LengthSquared<T, Size>(Lhs.ToHandle());
	}


	//////////////////////////////////////////////////////////////////////////
	template<typename T0, usize Size0, typename T1, usize Size1>
	TVectorType<T0, Size0> operator+(TVectorType<T0, Size0> Lhs, TVectorType<T1, Size1> Rhs)
	{
		using RetVal = TVectorType<T0, Size0>;
		return RetVal(VectorOperations::Add<T0, Size0>(Lhs.ToHandle(), RetVal(Rhs).ToHandle()));
	}

	template<typename T0, usize Size0, typename T1>
	TVectorType<T0, Size0> operator+(TVectorType<T0, Size0> Lhs, T1 Rhs)
	{
		using RetVal = TVectorType<T0, Size0>;
		return Lhs + RetVal(Rhs);
	}

	//////////////////////////////////////////////////////////////////////////
	template<typename T0, usize Size0, typename T1, usize Size1>
	TVectorType<T0, Size0> operator-(TVectorType<T0, Size0> Lhs, TVectorType<T1, Size1> Rhs)
	{
		using RetVal = TVectorType<T0, Size0>;
		return RetVal(VectorOperations::Sub<T0, Size0>(Lhs.ToHandle(), RetVal(Rhs).ToHandle()));
	}

	template<typename T0, usize Size0, typename T1>
	TVectorType<T0, Size0> operator-(TVectorType<T0, Size0> Lhs, T1 Rhs)
	{
		using RetVal = TVectorType<T0, Size0>;
		return Lhs - RetVal(Rhs);
	}

	//////////////////////////////////////////////////////////////////////////
	template<typename T0, usize Size0, typename T1, usize Size1>
	TVectorType<T0, Size0> operator*(TVectorType<T0, Size0> Lhs, TVectorType<T1, Size1> Rhs)
	{
		using RetVal = TVectorType<T0, Size0>;
		return RetVal(VectorOperations::Mul<T0, Size0>(Lhs.ToHandle(), RetVal(Rhs).ToHandle()));
	}

	template<typename T0, usize Size0, typename T1>
	TVectorType<T0, Size0> operator*(TVectorType<T0, Size0> Lhs, T1 Rhs)
	{
		using RetVal = TVectorType<T0, Size0>;
		return Lhs * RetVal(Rhs);
	}

	//////////////////////////////////////////////////////////////////////////
	template<typename T0, usize Size0, typename T1, usize Size1>
	TVectorType<T0, Size0> operator/(TVectorType<T0, Size0> Lhs, TVectorType<T1, Size1> Rhs)
	{
		using RetVal = TVectorType<T0, Size0>;
		return RetVal(VectorOperations::Div<T0, Size0>(Lhs.ToHandle(), RetVal(Rhs).ToHandle()));
	}

	template<typename T0, usize Size0, typename T1>
	TVectorType<T0, Size0> operator/(TVectorType<T0, Size0> Lhs, T1 Rhs)
	{
		using RetVal = TVectorType<T0, Size0>;
		return Lhs / RetVal(Rhs);
	}
}
