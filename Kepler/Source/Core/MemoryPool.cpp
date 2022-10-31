#include "MemoryPool.h"

namespace ke
{
#ifndef USE_PMR_ALLOCATORS
	//////////////////////////////////////////////////////////////////////////
	// Pool resource
	//////////////////////////////////////////////////////////////////////////
	PoolResource::PoolResource(usize initialSize)
		:	m_Capacity(initialSize)
	{
		AllocateMemory();
	}

	PoolResource::~PoolResource()
	{
		DeallocateMemory();
	}

	u8* PoolResource::AcquireBlock(usize size)
	{
		CHECK(m_AllocatedSize + size <= m_Capacity);
		u8* pBlock = m_Data + m_AllocatedSize;
		m_AllocatedSize += size;
		return pBlock;
	}

	void PoolResource::AllocateMemory()
	{
		m_Data = (u8*)malloc(m_Capacity);
		CHECK(m_Data);
		ZeroMemory(m_Data, m_Capacity);
	}

	void PoolResource::DeallocateMemory()
	{
		if (m_Data)
		{
			free(m_Data);
			m_Data = nullptr;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Pool allocator
	//////////////////////////////////////////////////////////////////////////
	PoolAllocator::PoolAllocator(PoolResource* pResource)
		:	m_Resource(pResource)
	{
	}

	void* PoolAllocator::allocate(usize size)
	{
		KEPLER_PROFILE_SCOPE();
		std::scoped_lock lck{ m_Mutex };
		if (u8* pBlock = FindInFreeList(size))
		{
			return pBlock;
		}
		return m_Resource->AcquireBlock(size);
	}

	void PoolAllocator::deallocate(TMemoryPoolBlockHeader* pBlock, usize size)
	{
		KEPLER_PROFILE_SCOPE();
		std::scoped_lock lck{ m_Mutex };
		m_FreeList.Push({ (u8*)pBlock, size });
		return;
	}


	PoolAllocator::FreeList::FreeList()
		: InternalPoolResource(1024 * 1024 * 10)
		, InternalPool(&InternalPoolResource)
		, Blocks(&InternalPool)
	{
	}


	u8* PoolAllocator::FindInFreeList(usize size)
	{
		KEPLER_PROFILE_SCOPE();
		u8* outBlock = nullptr;
		usize outBlockSize = 0;
		for (const auto& entry : m_FreeList)
		{
			if (entry.Size >= size)
			{
				outBlock = entry.Block;
				outBlockSize = entry.Size;
				m_FreeList.Remove(entry);
				break;
			}
		}
		return outBlock;
	}
#endif

	//////////////////////////////////////////////////////////////////////////
	// Memory Pool
	//////////////////////////////////////////////////////////////////////////
	TMemoryPool::TMemoryPool(usize InitialSize)
		: m_ContiguousMemoryResource(InitialSize)
		, m_PoolManager(&m_ContiguousMemoryResource)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	void* TMemoryPool::Allocate(usize size)
	{
		KEPLER_PROFILE_SCOPE();
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

	//////////////////////////////////////////////////////////////////////////
	void TMemoryPool::Deallocate(const void* pBlock)
	{
		KEPLER_PROFILE_SCOPE();
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

	//////////////////////////////////////////////////////////////////////////
	usize TMemoryPool::GetAllocationSize(const void* pBlock)
	{
		if (!pBlock)
		{
			return 0;
		}

		TMemoryPoolBlockHeader* pHeader = GetAllocationHeader(pBlock);
		return pHeader->Size;
	}

	//////////////////////////////////////////////////////////////////////////
	TMemoryPool* TMemoryPool::GetAllocationPool(const void* pBlock)
	{
		if (!pBlock)
		{
			return nullptr;
		}

		TMemoryPoolBlockHeader* pHeader = GetAllocationHeader(pBlock);
		return pHeader->Pool;
	}

	//////////////////////////////////////////////////////////////////////////
	TMemoryPoolBlockHeader* TMemoryPool::GetAllocationHeader(const void* pBlock)
	{
		return (TMemoryPoolBlockHeader*)((ubyte*)pBlock - sizeof(TMemoryPoolBlockHeader));
	}
	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
}