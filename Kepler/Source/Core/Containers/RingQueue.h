#pragma once
#include "Core/Types.h"
#include "Core/Malloc.h"

#include <mutex>

namespace ke
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
		TRingQueue(usize capacity)
			:	m_Capacity(capacity)
			,	m_Length(0)
			,	m_Head(0)
			,	m_Tail(0)
		{
			if (capacity > 0)
			{
				m_pMemory = (T*)TMalloc::Get()->Allocate(m_Capacity * sizeof(T));
			}
		}

		~TRingQueue()
		{
			TMalloc::Get()->Free(m_pMemory);
		}

		bool Enqueue(T&& value);

		bool Dequeue(T& outValue);

		bool Peek(T& outValue) const;

		inline usize GetLength() const 
		{ 
			if constexpr (ThreadPolicy == ERingQueueThreadPolicy::Safe)
			{
				std::lock_guard lck{ m_Mutex };
				return m_Length; 
			}
			else
			{
				return m_Length;
			}
		}
		inline usize GetCapacity() const { return m_Capacity; }

	private:
		bool InternalEnqueue(T&& value);

		bool InternalDequeue(T& outValue);

		bool InternalPeek(T& outValue) const;

	private:
		T* m_pMemory{};
		usize m_Capacity{};
		usize m_Length{};
		usize m_Head{};
		usize m_Tail{};
		mutable std::mutex m_Mutex{};
	};

	template<typename T>
	using TThreadRelaxedRingQueue = TRingQueue<T, ERingQueueThreadPolicy::Relaxed>;

	template<typename T>
	using TThreadSafeRingQueue = TRingQueue<T, ERingQueueThreadPolicy::Safe>;
}

#include "RingQueue.inl"