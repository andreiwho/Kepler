#pragma once
#include "Core/Types.h"

#include <memory_resource>

namespace Kepler
{
	struct TMemoryPoolBlockHeader
	{
		usize Size;
		class TMemoryPool* Pool;
	};

	class TMemoryPool
	{
	public:
		TMemoryPool(usize InitialSize);

		void* Allocate(usize Size);
		void Deallocate(const void* Block);

	public:
		static usize GetAllocationSize(const void* Block);
		static TMemoryPool* GetAllocationPool(const void* Block);

	private:
		static TMemoryPoolBlockHeader* GetAllocationHeader(const void* Block);

	private:
		ubyte* Memory{};
		usize Capacity{};
		usize Size{};

		std::pmr::monotonic_buffer_resource ContiguousMemoryResource;
		std::pmr::synchronized_pool_resource PoolManager;
	};
}