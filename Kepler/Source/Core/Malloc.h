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
		void Free(const void* block);
		usize GetSize(const void* block) const;

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

	template<typename T> using TSharedPtr = std::shared_ptr<T>;

	class TRefCounted
	{
	public:
		virtual ~TRefCounted() = default;

		void AddRef() const;
		void Release() const;
		inline u64 GetRefCount() const { return RefCount.load(); }

	private:
		mutable TAtomic<u64> RefCount{ 1 };
	};

	void DoRelease(void* RefCounted);
	void DoAddRef(void* RefCounted);
	usize DoGetRefCount(void* RefCounted);

	template<typename T>
	class TRef
	{
	public:
		TRef() : Memory(nullptr) {}
		TRef(T* NewMemory)
			: Memory(NewMemory)
		{
			if (Memory && !DoGetRefCount(Memory))
			{
				DoAddRef(Memory);
			}
		}

		~TRef() { Release(); }

		TRef(const TRef& Other) noexcept
		{
			if (Memory)
			{
				DoRelease(Memory);
			}
			Memory = const_cast<T*>(Other.Raw());
			DoAddRef(Memory);
		}

		template<typename U>
		TRef<T>(const TRef<U>& Other) noexcept
		{
			static_assert(std::is_base_of_v<T, U>);
			if (Memory)
			{
				DoRelease(Memory);
			}
			Memory = (T*)const_cast<U*>(Other.Raw());
			DoAddRef(Memory);
		}

		TRef& operator=(const TRef& Other) noexcept
		{
			if (Memory)
			{
				DoRelease(Memory);
			}
			Memory = const_cast<T*>(Other.Raw());
			DoAddRef(Memory);
			return *this;
		}

		template<typename U>
		TRef<T>& operator=(const TRef<U>& Other) noexcept
		{
			static_assert(std::is_base_of_v<T, U>);
			if (Memory)
			{
				DoRelease(Memory);
			}
			Memory = (T*)const_cast<U*>(Other.Raw());
			DoAddRef(Memory);
			return *this;
		}

		TRef(TRef&& Other) noexcept
		{
			if (Memory)
			{
				DoRelease(Memory);
			}
			Memory = Other.Raw();
			Other.ResetMemoryUnsafe();
		}

		template<typename U>
		TRef<T>(TRef<U>&& Other) noexcept
		{
			static_assert(std::is_base_of_v<T, U>);
			if (Memory)
			{
				DoRelease(Memory);
			}
			Memory = Other.Raw();
			Other.ResetMemoryUnsafe();
		}

		TRef& operator=(TRef&& Other) noexcept
		{
			if (Memory)
			{
				DoRelease(Memory);
			}
			Memory = Other.Raw();
			Other.ResetMemoryUnsafe();
			return *this;
		}

		template<typename U>
		TRef<T>& operator=(TRef<U>&& Other) noexcept
		{
			static_assert(std::is_base_of_v<T, U>);
			if (Memory)
			{
				DoRelease(Memory);
			}
			Memory = Other.Raw();
			Other.ResetMemoryUnsafe();
			return *this;
		}

		T* operator->() { return Memory; }
		T* operator->() const { return Memory; }

		T& operator*() { return *Memory; }
		const T& operator*() const { return *Memory; }
		inline operator bool() const { return !!Raw(); }

		T* Raw() { return Memory; }
		const T* Raw() const { return Memory; }

		void AddRef() const { if (Memory) Memory->AddRef(); }
		void Release() const
		{
			if (Memory)
			{
				DoRelease(Memory);
				Memory = nullptr;
			}
		}
		void ResetMemoryUnsafe() { Memory = nullptr; }
	private:
		//static_assert(std::is_base_of_v<TRefCounted, T>);
		mutable T* Memory{};
	};

	template<typename T>
	TRef<T> MakeRef(T* Memory)
	{
		//static_assert(std::is_base_of_v<TRefCounted, T>);
		return TRef<T>(Memory);
	}

	template<typename T>
	struct TEnableRefFromThis : public TRefCounted
	{
		inline TRef<T> RefFromThis()
		{
			AddRef();
			return MakeRef(static_cast<T*>(this));
		}
	};


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

	template<typename T, typename U>
	TRef<T> RefCast(TRef<U> Ref)
	{
		if constexpr (std::is_base_of_v<T, U> || std::is_base_of_v<U, T>)
		{
			if (Ref)
			{
				Ref.AddRef();
				return TRef<T>(static_cast<T*>(Ref.Raw()));
			}
		}
		return nullptr;
	}


	template<typename T, typename ... ARGS>
	inline TSharedPtr<T> MakeShared(ARGS&& ... Args)
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