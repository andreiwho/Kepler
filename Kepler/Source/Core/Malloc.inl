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
	void Delete(const T* object)
	{
		if (object)
		{
			object->~T();
			TMalloc* allocator = TMalloc::Get();
			allocator->Free(object);
		}
	}
}