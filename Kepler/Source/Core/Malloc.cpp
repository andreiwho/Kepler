#include "Core/Malloc.h"

#include <cassert>

namespace Kepler
{
	TMalloc* TMalloc::Instance = nullptr;

	TMalloc::TMalloc()
	{
		assert(!Instance);
		Instance = this;
	}

	TMalloc::~TMalloc()
	{
		Instance = nullptr;
	}

	void* TMalloc::Allocate(usize size)
	{
		const usize sizeWithHeader = size + sizeof(TAllocationHeader);
		void* newData = ::operator new(sizeWithHeader);
		assert(newData && "TMalloc::Allocate - failed");
		TAllocationHeader* header = (TAllocationHeader*)newData;
		header->AllocationSize = size;
		return (ubyte*)newData + sizeof(*header);
	}

	void TMalloc::Free(void* block)
	{
		void* headerPtr = GetHeaderPtr(block);
		assert(headerPtr);
		::operator delete(headerPtr);
	}

	usize TMalloc::GetSize(void* block) const
	{
		auto header = GetHeaderPtr(block);
		assert(header);
		return header->AllocationSize;
	}

	TMalloc::TAllocationHeader* TMalloc::GetHeaderPtr(void* block)
	{
		auto userMemory = (ubyte*)block;
		auto headerPtr = (TAllocationHeader*)(userMemory - sizeof(TAllocationHeader));
		assert(headerPtr && "Failed to get proper allocation ptr");
		return headerPtr;
	}

	usize MemorySize(void* data)
	{
		return CHECKED(TMalloc::Get())->GetSize(data);
	}

}