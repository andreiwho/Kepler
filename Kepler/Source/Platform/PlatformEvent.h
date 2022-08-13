#pragma once
#include "Core/Macros.h"
#include "Core/Types.h"
#include "Mouse.h"
#include "Keyboard.h"

#include <string>

namespace Kepler
{
	struct EPlatformEventType
	{
		enum EValue : u32
		{
			MouseMove = BIT(0),
			MouseButtonDown = BIT(1),
			MouseButtonUp = BIT(2),
			MouseScrollWheel = BIT(3),

			KeyDown = BIT(4),
			KeyUp = BIT(5),
			KeyHold = BIT(6),
			Char = BIT(7),

			WindowMove = BIT(8),
			WindowSize = BIT(9),
			WindowMaximize = BIT(10),
			WindowMinimize = BIT(11),
			WindowRestore = BIT(12),
			MouseEnter = BIT(13),
			MouseLeave = BIT(14),
		} Value;

		EPlatformEventType(EValue  value)
			: Value(value)
		{
		}

		inline operator u32() const { return Value; }
		std::string ToString() const;
	};

	struct EPlatformEventCategory
	{
		enum EValue : u32
		{
			Input = BIT(17),
			Mouse = BIT(18) | Input,
			Keyboard = BIT(19) | Input,
			Window = BIT(19),
			Other = BIT(20),
		} Value;

		EPlatformEventCategory(EValue  value)
			: Value(value)
		{
		}

		inline operator u32() const { return Value; }
		std::string ToString() const;
	};

	struct TPlatformEventBase
	{
		TPlatformEventBase(class TWindow* window, EPlatformEventType type, EPlatformEventCategory category);

		bool IsOfType(EPlatformEventType type) const;
		bool IsOfCategory(EPlatformEventCategory category) const;

		template<typename T> bool IsA() const 
		{
			return TypeMask == T::StaticMask;
		}

		virtual std::string ToString() const 
		{
			return "PlatformEventBase";
		}

		u32 TypeMask;
		TWindow* Window;
		mutable bool Handled = false;
	};

	template<EPlatformEventType::EValue Type, EPlatformEventCategory::EValue Category>
	struct TEvent : public TPlatformEventBase
	{};

#define INTERNAL_DEFINE_PLATFORM_EVENT_0P(Type, Category)\
	template<>\
	struct TEvent<EPlatformEventType::##Type, EPlatformEventCategory::##Category> : public TPlatformEventBase\
	{\
		TEvent(class TWindow* window)\
			: TPlatformEventBase(window, EPlatformEventType::##Type, EPlatformEventCategory::##Category){}\
		static constexpr u32 StaticMask = EPlatformEventType::##Type | EPlatformEventCategory::##Category;\
		virtual inline std::string ToString() const override {return #Type;}\
	};\
	using T##Type##Event = TEvent<EPlatformEventType::##Type, EPlatformEventCategory::##Category>


#define INTERNAL_DEFINE_PLATFORM_EVENT_1P(Type, Category, p0t, p0n)\
	template<>\
	struct TEvent<EPlatformEventType::##Type, EPlatformEventCategory::##Category> : public TPlatformEventBase\
	{\
		TEvent(class TWindow* window, p0t arg0)\
			: TPlatformEventBase(window, EPlatformEventType::##Type, EPlatformEventCategory::##Category)\
			, p0n(arg0) {}\
		const p0t p0n;\
		static constexpr u32 StaticMask = EPlatformEventType::##Type | EPlatformEventCategory::##Category;\
		virtual inline std::string ToString() const override {return #Type;}\
	};\
	using T##Type##Event = TEvent<EPlatformEventType::##Type, EPlatformEventCategory::##Category>

#define INTERNAL_DEFINE_PLATFORM_EVENT_2P(Type, Category, p0t, p0n, p1t, p1n)\
	template<>\
	struct TEvent<EPlatformEventType::##Type, EPlatformEventCategory::##Category> : public TPlatformEventBase\
	{\
		TEvent(class TWindow* window, p0t arg0, p1t arg1)\
			: TPlatformEventBase(window, EPlatformEventType::##Type, EPlatformEventCategory::##Category)\
			, p0n(arg0)\
			, p1n(arg1) {}\
		const p0t p0n;\
		const p1t p1n;\
		static constexpr u32 StaticMask = EPlatformEventType::##Type | EPlatformEventCategory::##Category;\
		virtual inline std::string ToString() const override {return #Type;}\
	};\
	using T##Type##Event = TEvent<EPlatformEventType::##Type, EPlatformEventCategory::##Category>

#define INTERNAL_DEFINE_PLATFORM_EVENT_3P(Type, Category, p0t, p0n, p1t, p1n, p2t, p2n)\
	template<>\
	struct TEvent<EPlatformEventType::##Type, EPlatformEventCategory::##Category> : public TPlatformEventBase\
	{\
		TEvent(class TWindow* window, p0t arg0, p1t arg1, p2t arg2)\
			: TPlatformEventBase(window, EPlatformEventType::##Type, EPlatformEventCategory::##Category)\
			, p0n(arg0)\
			, p1n(arg1)\
			, p2n(arg2) {}\
		const p0t p0n;\
		const p1t p1n;\
		const p2t p2n;\
		static constexpr u32 StaticMask = EPlatformEventType::##Type | EPlatformEventCategory::##Category;\
		virtual inline std::string ToString() const override {return #Type;}\
	};\
	using T##Type##Event = TEvent<EPlatformEventType::##Type, EPlatformEventCategory::##Category>

	/************************************************************************/
	/* EVENTS                                                               */
	/************************************************************************/
	INTERNAL_DEFINE_PLATFORM_EVENT_2P(MouseMove, Mouse, float, X, float, Y);
	INTERNAL_DEFINE_PLATFORM_EVENT_1P(MouseButtonDown, Mouse, EMouseButton, Button);
	INTERNAL_DEFINE_PLATFORM_EVENT_1P(MouseButtonUp, Mouse, EMouseButton, Button);
	INTERNAL_DEFINE_PLATFORM_EVENT_1P(MouseScrollWheel, Mouse, float, Amount);
	INTERNAL_DEFINE_PLATFORM_EVENT_0P(MouseEnter, Mouse);
	INTERNAL_DEFINE_PLATFORM_EVENT_0P(MouseLeave, Mouse);
	
	INTERNAL_DEFINE_PLATFORM_EVENT_1P(KeyDown, Keyboard, EKeyCode, Key);
	INTERNAL_DEFINE_PLATFORM_EVENT_1P(KeyUp, Keyboard, EKeyCode, Key);
	INTERNAL_DEFINE_PLATFORM_EVENT_1P(KeyHold, Keyboard, EKeyCode, Key);
	INTERNAL_DEFINE_PLATFORM_EVENT_1P(Char, Keyboard, char, Char);

	INTERNAL_DEFINE_PLATFORM_EVENT_2P(WindowMove, Window, i32, X, i32, Y);
	INTERNAL_DEFINE_PLATFORM_EVENT_2P(WindowSize, Window, i32, Width, i32, Height);
	INTERNAL_DEFINE_PLATFORM_EVENT_0P(WindowMaximize, Window);
	INTERNAL_DEFINE_PLATFORM_EVENT_0P(WindowMinimize, Window);
	INTERNAL_DEFINE_PLATFORM_EVENT_0P(WindowRestore, Window);

	// Interface for the window event listener
	class IPlatformEventListener
	{
	public:
		virtual ~IPlatformEventListener() = default;
		virtual void OnPlatformEvent(const TPlatformEventBase& event){};
	};

	class TPlatformEventDispatcher
	{
	public:
		const TPlatformEventBase& Event;

		TPlatformEventDispatcher(const TPlatformEventBase& event) : Event(event) {}

		template<typename T>
		void Dispatch(bool(*handleFunc)(const T& e))
		{
			if (!Event.Handled && Event.IsA<T>())
			{
				Event.Handled = handleFunc(static_cast<const T&>(Event));
			}
		}

		template<typename T, typename Handler>
		void Dispatch(Handler* handler, bool(Handler::* handleFunc)(const T&))
		{
			if (!Event.Handled && Event.IsA<T>())
			{
				Event.Handled = (handler->*handleFunc)(static_cast<const T&>(Event));
			}
		}

		void Dispatch(EPlatformEventCategory category, bool(*handlerFunc)(const TPlatformEventBase&))
		{
			if (!Event.Handled && Event.IsOfCategory(category))
			{
				Event.Handled = handlerFunc(Event);
			}
		}

		template<typename Handler>
		void Dispatch(EPlatformEventCategory category, Handler* handler, bool(Handler::*handlerFunc)(const TPlatformEventBase&))
		{
			if (!Event.Handled && Event.IsOfCategory(category))
			{
				Event.Handled = (handler->*handlerFunc)(Event);
			}
		}
	};
}