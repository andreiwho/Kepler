#pragma once
#include "Core/Types.h"

#include <memory_resource>
#include <list>

namespace ke
{
	struct TMemoryPoolBlockHeader
	{
		usize Size;
		class TMemoryPool* Pool;
	};

#ifndef USE_PMR_ALLOCATORS
	// Pool resource manages single contiguous block of memory which exists and can be resized and is not thread safe.
	// Though this class doesn't handle free lists, so should be used as a backing resource for the PoolAllocator
	class PoolResource
	{
	public:
		PoolResource(usize initialSize);
		~PoolResource();

		u8* AcquireBlock(usize size);

	private:
		void AllocateMemory();
		void DeallocateMemory();

	private:
		u8* m_Data{};
		usize m_Capacity{};
		usize m_AllocatedSize{};
	};

	// Pool allocator - allocation backend for the memory pool, should handle allocation and free-listing
	class PoolAllocator
	{
	public:
		PoolAllocator(PoolResource* pResource);

		void* allocate(usize size);
		void deallocate(TMemoryPoolBlockHeader* pBlock, usize size);

	private:
		struct FreeListBlock
		{
			u8* Block;
			usize Size;

			friend static bool operator==(const FreeListBlock& lh, const FreeListBlock& rh)
			{
				return lh.Block == rh.Block;
			}
		};

		struct FreeList
		{
			FreeList();

			std::pmr::monotonic_buffer_resource InternalPoolResource;
			std::pmr::unsynchronized_pool_resource InternalPool;
			std::pmr::list<FreeListBlock> Blocks;

			inline decltype(auto) begin() const { return Blocks.begin(); }
			inline decltype(auto) end() const { return Blocks.end(); }

			void Push(FreeListBlock&& block) { Blocks.push_back(std::move(block)); }
			void Remove(const FreeListBlock& block) { Blocks.remove(block); }
		};

		u8* FindInFreeList(usize size);

		PoolResource* m_Resource = nullptr;
		FreeList m_FreeList{};
		std::mutex m_Mutex{};
	};
#else
	using PoolResource = std::pmr::monotonic_buffer_resource;
	using PoolAllocator = std::pmr::synchronized_pool_resource;
#endif

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
		PoolResource m_ContiguousMemoryResource;
		PoolAllocator m_PoolManager;
		std::mutex m_Mutex;
	};
}