#include "Core/Malloc.h"

#include <cassert>

namespace ke
{
	std::unique_ptr<TMemoryPool> GGlobalMemoryPool = nullptr;
	TMalloc* TMalloc::Instance = nullptr;

	TMalloc::TMalloc()
	{
		assert(!Instance);
		Instance = this;

		if (!GGlobalMemoryPool)
		{
			GGlobalMemoryPool = std::unique_ptr<TMemoryPool>(new TMemoryPool(1024 * 1024 * 1024));
		}
	}

	TMalloc::~TMalloc()
	{
		Instance = nullptr;
		if (GGlobalMemoryPool)
		{
			GGlobalMemoryPool.reset();
		}
	}

	void* TMalloc::Allocate(usize size, TMemoryPool* pPool)
	{
		TMemoryPool* pAllocPool = pPool;
		if (!pPool)
		{
			pAllocPool = GGlobalMemoryPool.get();
		}

		void* pNewData = pAllocPool->Allocate(size);
		assert(pNewData && "TMalloc::Allocate - failed");
		return pNewData;
	}

	void TMalloc::Free(const void* pBlock)
	{
		if (!pBlock)
		{
			return;
		}

		TMemoryPool* pPool = TMemoryPool::GetAllocationPool(pBlock);
		CHECK(pPool != nullptr);
		pPool->Deallocate(pBlock);
	}

	usize TMalloc::GetSize(const void* pBlock) const
	{
		return TMemoryPool::GetAllocationSize(pBlock);
	}

	usize MemorySize(void* pData)
	{
		return CHECKED(TMalloc::Get())->GetSize(pData);
	}

	void DoRelease(void* pRefCounted)
	{
		((IntrusiveRefCounted*)pRefCounted)->Release();
	}

	void DoAddRef(void* pRefCounted)
	{
		((IntrusiveRefCounted*)pRefCounted)->AddRef();
	}

	usize DoGetRefCount(void* pRefCounted)
	{
		return ((IntrusiveRefCounted*)pRefCounted)->GetRefCount();
	}

	void IntrusiveRefCounted::AddRef() const
	{
		m_RefCount++;
	}

	void IntrusiveRefCounted::Release() const
	{
		if (m_RefCount > 0)
		{
			m_RefCount--;
		}

		if (m_RefCount == 0)
		{
			Delete(this);
		}
	}

}