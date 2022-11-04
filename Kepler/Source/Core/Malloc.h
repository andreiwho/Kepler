#pragma once
#include "Types.h"
#include "Macros.h"
#include "MemoryPool.h"

#include <memory>

namespace ke
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

		void* Allocate(usize size, TMemoryPool* pPool = nullptr);
		void Free(const void* block);
		usize GetSize(const void* block) const;
	};

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

	usize MemorySize(void* data);

	template<typename T> using SharedPtr = std::shared_ptr<T>;

	class IntrusiveRefCounted
	{
	public:
		virtual ~IntrusiveRefCounted() = default;

		void AddRef() const;
		void Release() const;
		inline u64 GetRefCount() const { return m_RefCount.load(); }

	private:
		mutable TAtomic<u64> m_RefCount{ 1 };
	};

	void DoRelease(void* pRefCnt);
	void DoAddRef(void* pRefCounted);
	usize DoGetRefCount(void* pRefCounted);

	template<typename T>
	class RefPtr
	{
	public:
		RefPtr() : m_pMemory(nullptr) {}
		RefPtr(T* pNewMem)
			: m_pMemory(pNewMem)
		{
			if (m_pMemory && !DoGetRefCount(m_pMemory))
			{
				DoAddRef(m_pMemory);
			}
		}

		~RefPtr() { Release(); }

		RefPtr(const RefPtr& other) noexcept
		{
			if (m_pMemory)
			{
				DoRelease(m_pMemory);
			}
			m_pMemory = const_cast<T*>(other.Raw());
			if (m_pMemory)
			{
				DoAddRef(m_pMemory);
			}
		}

		template<typename U>
		RefPtr<T>(const RefPtr<U>& other) noexcept
		{
			static_assert(std::is_base_of_v<T, U>);
			if (m_pMemory)
			{
				DoRelease(m_pMemory);
			}
			m_pMemory = (T*)const_cast<U*>(other.Raw());
			if (m_pMemory)
			{
				DoAddRef(m_pMemory);
			}
		}

		RefPtr& operator=(const RefPtr& other) noexcept
		{
			if (m_pMemory)
			{
				DoRelease(m_pMemory);
			}
			m_pMemory = const_cast<T*>(other.Raw());
			if (m_pMemory)
			{
				DoAddRef(m_pMemory);
			}
			return *this;
		}

		template<typename U>
		RefPtr<T>& operator=(const RefPtr<U>& other) noexcept
		{
			static_assert(std::is_base_of_v<T, U>);
			if (m_pMemory)
			{
				DoRelease(m_pMemory);
			}
			m_pMemory = (T*)const_cast<U*>(other.Raw());
			if (m_pMemory)
			{
				DoAddRef(m_pMemory);
			}
			return *this;
		}

		RefPtr(RefPtr&& other) noexcept
		{
			if (m_pMemory)
			{
				DoRelease(m_pMemory);
			}
			m_pMemory = other.Raw();
			other.ResetMemoryUnsafe();
		}

		template<typename U>
		RefPtr<T>(RefPtr<U>&& other) noexcept
		{
			static_assert(std::is_base_of_v<T, U>);
			if (m_pMemory)
			{
				DoRelease(m_pMemory);
			}
			m_pMemory = other.Raw();
			other.ResetMemoryUnsafe();
		}

		RefPtr& operator=(RefPtr&& other) noexcept
		{
			if (m_pMemory)
			{
				DoRelease(m_pMemory);
			}
			m_pMemory = other.Raw();
			other.ResetMemoryUnsafe();
			return *this;
		}

		template<typename U>
		RefPtr<T>& operator=(RefPtr<U>&& other) noexcept
		{
			static_assert(std::is_base_of_v<T, U>);
			if (m_pMemory)
			{
				DoRelease(m_pMemory);
			}
			m_pMemory = other.Raw();
			other.ResetMemoryUnsafe();
			return *this;
		}

		T* operator->() { return m_pMemory; }
		T* operator->() const { return m_pMemory; }

		T& operator*() { return *m_pMemory; }
		const T& operator*() const { return *m_pMemory; }
		inline operator bool() const { return !!Raw(); }
		inline operator T* () { return Raw(); }
		inline operator const T* () const { return Raw(); }
		T* Raw() { return m_pMemory; }
		const T* Raw() const { return m_pMemory; }

		void AddRef() const { if (m_pMemory) m_pMemory->AddRef(); }
		void Release() const
		{
			if (m_pMemory)
			{
				DoRelease(m_pMemory);
				m_pMemory = nullptr;
			}
		}
		void ResetMemoryUnsafe() { m_pMemory = nullptr; }
	private:
		//static_assert(std::is_base_of_v<TRefCounted, T>);
		mutable T* m_pMemory{};
	};

	template<typename T>
	RefPtr<T> MakeRef(T* pMem)
	{
		//static_assert(std::is_base_of_v<TRefCounted, T>);
		return RefPtr<T>(pMem);
	}

	template<typename T>
	struct EnableRefPtrFromThis : public IntrusiveRefCounted
	{
		inline RefPtr<T> RefFromThis()
		{
			AddRef();
			return MakeRef(static_cast<T*>(this));
		}

		template<typename U>
		inline RefPtr<U> RefFromThis()
		{
			return RefCast<U>(RefFromThis());
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
	RefPtr<T> RefCast(RefPtr<U> ref)
	{
		if constexpr (std::is_base_of_v<T, U> || std::is_base_of_v<U, T>)
		{
			if (ref)
			{
				ref.AddRef();
				return RefPtr<T>(static_cast<T*>(ref.Raw()));
			}
		}
		return nullptr;
	}

	template<typename T, typename ... ARGS>
	inline SharedPtr<T> MakeShared(ARGS&& ... Args)
	{
		return std::allocate_shared<T>(TMallocator<T>{}, std::forward<ARGS>(Args)...);
	}

	template<typename T>
	struct TDefaultDeleter { void operator()(T* mem) { ke::Delete<T>(mem); } };

	template<typename T> using TScope = std::unique_ptr<T, TDefaultDeleter<T>>;
	template<typename T, typename... Args>
	inline constexpr TScope<T> AsScoped(T* Pointer)
	{
		return std::unique_ptr<T, TDefaultDeleter<T>>(Pointer, TDefaultDeleter<T>());
	}
}

#include "Malloc.inl"