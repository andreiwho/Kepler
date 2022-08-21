#include "Core/Malloc.h"

#include <cassert>

namespace Kepler
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

	void* TMalloc::Allocate(usize Size, TMemoryPool* MemoryPool)
	{
		TMemoryPool* AllocationPool = MemoryPool;
		if (!MemoryPool)
		{
			AllocationPool = GGlobalMemoryPool.get();
		}

		void* NewData = AllocationPool->Allocate(Size);
		assert(NewData && "TMalloc::Allocate - failed");
		return NewData;
	}

	void TMalloc::Free(const void* Block)
	{
		TMemoryPool* Pool = TMemoryPool::GetAllocationPool(Block);
		CHECK(Pool != nullptr);
		Pool->Deallocate(Block);
	}

	usize TMalloc::GetSize(const void* Block) const
	{
		return TMemoryPool::GetAllocationSize(Block);
	}

	usize MemorySize(void* Data)
	{
		return CHECKED(TMalloc::Get())->GetSize(Data);
	}

	void DoRelease(void* RefCounted)
	{
		((TRefCounted*)RefCounted)->Release();
	}

	void DoAddRef(void* RefCounted)
	{
		((TRefCounted*)RefCounted)->AddRef();
	}

	usize DoGetRefCount(void* RefCounted)
	{
		return ((TRefCounted*)RefCounted)->GetRefCount();
	}

	void TRefCounted::AddRef() const
	{
		RefCount++;
	}

	void TRefCounted::Release() const
	{
		if (RefCount > 0)
		{
			RefCount--;
		}

		if (RefCount == 0)
		{
			Delete(this);
		}
	}

}