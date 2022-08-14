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

	void TMalloc::Free(void* Block)
	{
		TMemoryPool* Pool = TMemoryPool::GetAllocationPool(Block);
		CHECK(Pool != nullptr);
		Pool->Deallocate(Block);
	}

	usize TMalloc::GetSize(void* Block) const
	{
		return TMemoryPool::GetAllocationSize(Block);
	}

	usize MemorySize(void* Data)
	{
		return CHECKED(TMalloc::Get())->GetSize(Data);
	}

}