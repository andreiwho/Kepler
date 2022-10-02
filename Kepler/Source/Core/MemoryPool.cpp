#include "MemoryPool.h"

namespace ke
{

	TMemoryPool::TMemoryPool(usize InitialSize)
		:	ContiguousMemoryResource(InitialSize)
		,	PoolManager(&ContiguousMemoryResource)
	{
	}

	void* TMemoryPool::Allocate(usize Size)
	{
		const usize SizeWithHeader = Size + sizeof(TMemoryPoolBlockHeader);
		TMemoryPoolBlockHeader* NewMemoryBlock = (TMemoryPoolBlockHeader*)PoolManager.allocate(SizeWithHeader);
		if (NewMemoryBlock)
		{
			NewMemoryBlock->Pool = this;
			NewMemoryBlock->Size = Size;
			return (ubyte*)NewMemoryBlock + sizeof(TMemoryPoolBlockHeader);
		}
		return nullptr;
	}

	void TMemoryPool::Deallocate(const void* Block)
	{
		if (!Block)
		{
			return;
		}

		// Being naughty here :)
		TMemoryPoolBlockHeader* Header = GetAllocationHeader(Block);
		if (Header)
		{
			PoolManager.deallocate(Header, Header->Size + sizeof(TMemoryPoolBlockHeader));
		}
	}

	usize TMemoryPool::GetAllocationSize(const void* Block)
	{
		if (!Block)
		{
			return 0;
		}

		// Being naughty here :)
		TMemoryPoolBlockHeader* Header = GetAllocationHeader(Block);
		return Header->Size;
	}

	TMemoryPool* TMemoryPool::GetAllocationPool(const void* Block)
	{
		if (!Block)
		{
			return nullptr;
		}

		TMemoryPoolBlockHeader* Header = GetAllocationHeader(Block);
		return Header->Pool;
	}

	TMemoryPoolBlockHeader* TMemoryPool::GetAllocationHeader(const void* Block)
	{
		return (TMemoryPoolBlockHeader*)((ubyte*)Block - sizeof(TMemoryPoolBlockHeader));
	}

}