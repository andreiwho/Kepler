#pragma once

namespace ke
{
	template<typename T, ERingQueueThreadPolicy ThreadPolicy>
	bool TRingQueue<T, ThreadPolicy>::Enqueue(T&& value)
	{
		if constexpr (ThreadPolicy == ERingQueueThreadPolicy::Safe)
		{
			std::lock_guard lck{ m_Mutex };
			return InternalEnqueue(std::move(value));
		}
		else
		{
			return InternalEnqueue(std::move(value));
		}
	}

	template<typename T, ERingQueueThreadPolicy ThreadPolicy>
	bool TRingQueue<T, ThreadPolicy>::Dequeue(T& outValue)
	{
		if constexpr (ThreadPolicy == ERingQueueThreadPolicy::Safe)
		{
			std::lock_guard lck{ m_Mutex };
			return InternalDequeue(outValue);
		}
		else
		{
			return InternalDequeue(outValue);
		}
	}


	template<typename T, ERingQueueThreadPolicy ThreadPolicy>
	bool TRingQueue<T, ThreadPolicy>::Peek(T& outValue) const
	{
		if constexpr (ThreadPolicy == ERingQueueThreadPolicy::Safe)
		{
			std::lock_guard lck{ m_Mutex };
			return InternalPeek(outValue);
		}
		else
		{
			return InternalPeek(outValue);
		}
	}

	template<typename T, ERingQueueThreadPolicy ThreadPolicy>
	bool TRingQueue<T, ThreadPolicy>::InternalEnqueue(T&& value)
	{
		if (!m_pMemory)
		{
			return false;
		}

		CHECK(m_Length != m_Capacity);
		new(m_pMemory + m_Tail) T(std::move(value));
		m_Tail = (m_Tail + 1) % m_Capacity;
		m_Length++;
		return true;
	}


	template<typename T, ERingQueueThreadPolicy ThreadPolicy>
	bool ke::TRingQueue<T, ThreadPolicy>::InternalDequeue(T& outValue)
	{
		if (!m_pMemory)
		{
			return false;
		}

		if (m_Length == 0)
		{
			return false;
		}

		new(&outValue) T(std::move(m_pMemory[m_Head]));
		m_pMemory[m_Head].~T();
		m_Head = (m_Head + 1) % m_Capacity;
		m_Length--;
		return true;
	}


	template<typename T, ERingQueueThreadPolicy ThreadPolicy>
	bool TRingQueue<T, ThreadPolicy>::InternalPeek(T& outValue) const
	{
		if (!m_pMemory)
		{
			return false;
		}

		if (m_Length == 0)
		{
			return false;
		}

		new(&outValue) T(m_pMemory[m_Head]);
		return true;
	}
}