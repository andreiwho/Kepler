#include "MemoryPool.h"

namespace ke
{

	TMemoryPool::TMemoryPool(usize InitialSize)
		: m_ContiguousMemoryResource(InitialSize)
		, m_PoolManager(&m_ContiguousMemoryResource)
	{
	}

	void* TMemoryPool::Allocate(usize size)
	{
		const usize sizeWithHeader = size + sizeof(TMemoryPoolBlockHeader);
		TMemoryPoolBlockHeader* pNewBlock = std::invoke(
			[&, this]
			{
				std::lock_guard lck{ m_Mutex };
				return (TMemoryPoolBlockHeader*)m_PoolManager.allocate(sizeWithHeader);
			});
		if (pNewBlock)
		{
			pNewBlock->Pool = this;
			pNewBlock->Size = size;
			return (ubyte*)pNewBlock + sizeof(TMemoryPoolBlockHeader);
		}
		return nullptr;
	}

	void TMemoryPool::Deallocate(const void* pBlock)
	{
		if (!pBlock)
		{
			return;
		}

		// Being naughty here :)
		TMemoryPoolBlockHeader* pHeader = GetAllocationHeader(pBlock);
		if (pHeader)
		{
			std::lock_guard lck{ m_Mutex };
			m_PoolManager.deallocate(pHeader, pHeader->Size + sizeof(TMemoryPoolBlockHeader));
		}
	}

	usize TMemoryPool::GetAllocationSize(const void* pBlock)
	{
		if (!pBlock)
		{
			return 0;
		}

		TMemoryPoolBlockHeader* pHeader = GetAllocationHeader(pBlock);
		return pHeader->Size;
	}

	TMemoryPool* TMemoryPool::GetAllocationPool(const void* pBlock)
	{
		if (!pBlock)
		{
			return nullptr;
		}

		TMemoryPoolBlockHeader* pHeader = GetAllocationHeader(pBlock);
		return pHeader->Pool;
	}

	TMemoryPoolBlockHeader* TMemoryPool::GetAllocationHeader(const void* pBlock)
	{
		return (TMemoryPoolBlockHeader*)((ubyte*)pBlock - sizeof(TMemoryPoolBlockHeader));
	}

}