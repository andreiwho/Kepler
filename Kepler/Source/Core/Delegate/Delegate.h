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
}

#define DEFINE_MULTICAST_DELEGATE(Name) class Name : public ke::MulticastDelegate {}