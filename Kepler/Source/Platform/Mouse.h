#pragma once
#include "Core/Types.h"

struct EMouseButton
{
	enum EValue : ke::ubyte
	{
		Unknown = BIT(7),
		Left = BIT(0),
		Right = BIT(1),
		Middle = BIT(2),
	} const Value{Unknown};

	EMouseButton(EValue value) : Value(value) {}
	inline operator EValue() const { return Value; }
};

namespace ke
{
	struct TMouseVector2f
	{
		float X{}, Y{};
	};

	class TMouseState
	{
	public:
		void OnButtonPressed(EMouseButton button);
		void OnButtonReleased(EMouseButton button);
		void OnMouseMove(TMouseVector2f newPosition);

		inline TMouseVector2f GetPosition() const { return Position; }
		bool GetButtonState(EMouseButton button) const;
		inline TMouseVector2f GetOffset() const { return OldOffset; }
		void OnUpdate();

	private:
		TMouseVector2f Position{};
		TMouseVector2f Offset{};
		TMouseVector2f OldOffset{};
		ubyte ButtonStateMask{};
	};
}