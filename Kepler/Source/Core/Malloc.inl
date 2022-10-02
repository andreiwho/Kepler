#pragma once
#include <type_traits>

namespace ke
{
	template<typename T, typename ... Args>
	T* New(Args&&... args)
	{
		TMalloc* allocator = TMalloc::Get();
		void* addr = allocator->Allocate(sizeof(T));
		assert(addr && "Failed to allocate object");
		return new(addr) T(std::forward<Args>(args)...);
	}

	template<typename T>
	void Delete(const T* pObj)
	{
		if (pObj)
		{
			pObj->~T();
			TMalloc* pAlloc = TMalloc::Get();
			pAlloc->Free(pObj);
		}
	}
}