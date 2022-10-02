#pragma once
#include "Core/Types.h"

#include <memory_resource>

namespace ke
{
	struct TMemoryPoolBlockHeader
	{
		usize Size;
		class TMemoryPool* Pool;
	};

	class TMemoryPool
	{
	public:
		TMemoryPool(usize size);

		void* Allocate(usize size);
		void Deallocate(const void* pBlock);

	public:
		static usize GetAllocationSize(const void* pBlock);
		static TMemoryPool* GetAllocationPool(const void* pBlock);

	private:
		static TMemoryPoolBlockHeader* GetAllocationHeader(const void* pBlock);

	private:
		ubyte* m_pMemory{};
		usize m_Capacity{};
		usize m_Size{};

		std::pmr::monotonic_buffer_resource m_ContiguousMemoryResource;
		std::pmr::synchronized_pool_resource m_PoolManager;
	};
}