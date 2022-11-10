#pragma once
#include "Core/Types.h"
#include "Core/Macros.h"
#include "Core/Containers/DynArray.h"

#include <functional>

namespace ke
{
	using DelegateHandle = usize;

	class MulticastDelegate
	{
	public:
		using FuncSig = std::function<void()>;

		template<typename Handler, typename Function>
		DelegateHandle Bind(Handler* pHandler, Function pFunction)
		{
			FuncSig func = [handler = pHandler, pFunc = std::forward<Function>(pFunction)]()
			{
				(handler->*pFunc)();
			};
		 	const DelegateHandle handle =  m_Callbacks.AppendBack(std::move(func));
			return handle;
		}

		void Invoke();

	private:
		Array<FuncSig> m_Callbacks;
	};

	template<typename ... ParamTypes>
	class MulticastDelegateWithParams
	{
	public:
		using FuncSig = std::function<void(ParamTypes&&...)>;

		template<typename Handler, typename Function>
		DelegateHandle Bind(Handler* pHandler, Function pFunction)
		{
			FuncSig func = [handler = pHandler, pFunc = std::forward<Function>(pFunction)](ParamTypes&&... params)
			{
				(handler->*pFunc)(std::forward<ParamTypes>(params)...);
			};
			const DelegateHandle handle = m_Callbacks.AppendBack(std::move(func));
			return handle;
		}

		void Invoke(ParamTypes&&... params)
		{
			for (auto& func : m_Callbacks)
			{
				func(std::forward<ParamTypes>(params)...);
			}
		}

	private:
		Array<FuncSig> m_Callbacks;
	};

}

#define DEFINE_MULTICAST_DELEGATE(Name) class Name : public ke::MulticastDelegate {}
#define DEFINE_MULTICAST_DELEGATE_WITH_PARAMS(Name, ...) class Name : public ke::MulticastDelegateWithParams<__VA_ARGS__> {}