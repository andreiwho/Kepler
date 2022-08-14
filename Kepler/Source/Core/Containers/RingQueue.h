#pragma once
#include "Core/Types.h"
#include "Core/Malloc.h"

#include <mutex>

namespace Kepler
{
	enum class ERingQueueThreadPolicy
	{
		Relaxed = 0,
		Safe = 1,
	};

	template<typename T, ERingQueueThreadPolicy ThreadPolicy>
	class TRingQueue
	{
	public:
		TRingQueue(usize InCapacity)
			:	Capacity(InCapacity)
			,	Length(0)
			,	Head(0)
			,	Tail(0)
		{
			if (InCapacity > 0)
			{
				Memory = (T*)TMalloc::Get()->Allocate(Capacity * sizeof(T));
			}
		}

		~TRingQueue()
		{
			TMalloc::Get()->Free(Memory);
		}

		bool Enqueue(T&& Value);

		bool Dequeue(T& OutValue);

		bool Peek(T& OutValue) const;

		inline usize GetLength() const 
		{ 
			if constexpr (ThreadPolicy == ERingQueueThreadPolicy::Safe)
			{
				std::lock_guard lck{ Mutex };
				return Length; 
			}
			else
			{
				return Length;
			}
		}
		inline usize GetCapacity() const { return Capacity; }

	private:
		bool InternalEnqueue(T&& Value);

		bool InternalDequeue(T& OutValue);

		bool InternalPeek(T& OutValue) const;

	private:
		T* Memory{};
		usize Capacity{};
		usize Length{};
		usize Head{};
		usize Tail{};
		mutable std::mutex Mutex{};
	};

	template<typename T>
	using TThreadRelaxedRingQueue = TRingQueue<T, ERingQueueThreadPolicy::Relaxed>;

	template<typename T>
	using TThreadSafeRingQueue = TRingQueue<T, ERingQueueThreadPolicy::Safe>;
}

#include "RingQueue.inl"