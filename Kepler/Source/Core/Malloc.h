#pragma once
#include "Types.h"
#include "Macros.h"

#include <memory>

namespace Kepler
{
	/**
	 * Currently the TMalloc class does not hold any kind of state.
	 * Just in case anyone is wondering, it will.
	 */
	class TMalloc
	{
		static TMalloc* Instance;
	public:
		TMalloc();
		~TMalloc();

		static TMalloc* Get() { return CHECKED(Instance); }

		void* Allocate(usize size);
		void Free(void* block);
		usize GetSize(void* block) const;

	private:
		struct TAllocationHeader 
		{
			usize AllocationSize{};
		};

		static TAllocationHeader* GetHeaderPtr(void* block);
	};

	template<typename T, typename ... Args>
	T* New(Args&&... args);

	template<typename T>
	void Delete(T* object);

	usize MemorySize(void* data);

	template<typename T> using TRef = std::shared_ptr<T>;
	template<typename T>
	inline TRef<T> AsRef(T* Pointer) 
	{
		return std::shared_ptr<T>(Pointer, &Delete<T>);
	}

	template<typename T>
	struct TDefaultDeleter { void operator()(T* mem) { Kepler::Delete<T>(mem); } };

	template<typename T> using TScope = std::unique_ptr<T, TDefaultDeleter<T>>;
	template<typename T, typename... Args>
	inline constexpr TScope<T> AsScoped(T* Pointer) 
	{ 
		return std::unique_ptr<T, TDefaultDeleter<T>>(Pointer, TDefaultDeleter<T>());
	}
}

#include "Malloc.inl"