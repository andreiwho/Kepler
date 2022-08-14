#pragma once
#include "Types.h"
#include "Macros.h"
#include "MemoryPool.h"

#include <memory>

namespace Kepler
{
	extern std::unique_ptr<TMemoryPool> GGlobalMemoryPool;

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

		void* Allocate(usize Size, TMemoryPool* MemoryPool = nullptr);
		void Free(void* block);
		usize GetSize(void* block) const;

	private:
		struct TAllocationHeader
		{
			usize AllocationSize{};
		};
	};

	template<typename T, typename ... Args>
	T* New(Args&&... args);

	template<typename T>
	void Delete(T* object);

	usize MemorySize(void* data);


	template<typename T> using TRef = std::shared_ptr<T>;

	template <typename T>
	class TMallocator
	{
	public:
		using value_type = T;

		TMallocator() noexcept {}
		template <typename U>
		TMallocator(const TMallocator<U>&) noexcept {}
		~TMallocator() = default;

		T* allocate(uint64_t n) noexcept
		{
			return reinterpret_cast<T*>(TMalloc::Get()->Allocate(n * sizeof(T)));
		}

		void deallocate(T* p, uint64_t n) noexcept
		{
			if (p)
			{
				TMalloc::Get()->Free(p);
			}
		}

		template <typename U>
		struct rebind
		{
			using other = TMallocator<U>;
		};

		TMallocator(const TMallocator&) = default;
		TMallocator& operator=(const TMallocator&) = default;
		TMallocator(TMallocator&& other) noexcept = default;
		TMallocator& operator=(TMallocator&& other) noexcept = default;
	};


	template<typename T, typename ... ARGS>
	inline TRef<T> MakeRef(ARGS&& ... Args)
	{
		return std::allocate_shared<T>(TMallocator<T>{}, std::forward<ARGS>(Args)...);
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