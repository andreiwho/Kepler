#pragma once

namespace Kepler
{
	template<typename T, ERingQueueThreadPolicy ThreadPolicy>
	bool TRingQueue<T, ThreadPolicy>::Enqueue(T&& Value)
	{
		if constexpr (ThreadPolicy == ERingQueueThreadPolicy::Safe)
		{
			std::lock_guard lck{ Mutex };
			return InternalEnqueue(std::move(Value));
		}
		else
		{
			return InternalEnqueue(std::move(Value));
		}
	}

	template<typename T, ERingQueueThreadPolicy ThreadPolicy>
	bool TRingQueue<T, ThreadPolicy>::Dequeue(T& OutValue)
	{
		if constexpr (ThreadPolicy == ERingQueueThreadPolicy::Safe)
		{
			std::lock_guard lck{ Mutex };
			return InternalDequeue(OutValue);
		}
		else
		{
			return InternalDequeue(OutValue);
		}
	}


	template<typename T, ERingQueueThreadPolicy ThreadPolicy>
	bool TRingQueue<T, ThreadPolicy>::Peek(T& OutValue) const
	{
		if constexpr (ThreadPolicy == ERingQueueThreadPolicy::Safe)
		{
			std::lock_guard lck{ Mutex };
			return InternalPeek(OutValue);
		}
		else
		{
			return InternalPeek(OutValue);
		}
	}

	template<typename T, ERingQueueThreadPolicy ThreadPolicy>
	bool TRingQueue<T, ThreadPolicy>::InternalEnqueue(T&& Value)
	{
		if (!Memory)
		{
			return false;
		}

		CHECK(Length != Capacity);
		new(Memory + Tail) T(std::move(Value));
		Tail = (Tail + 1) % Capacity;
		Length++;
		return true;
	}


	template<typename T, ERingQueueThreadPolicy ThreadPolicy>
	bool Kepler::TRingQueue<T, ThreadPolicy>::InternalDequeue(T& OutValue)
	{
		if (!Memory)
		{
			return false;
		}

		if (Length == 0)
		{
			return false;
		}

		new(&OutValue) T(std::move(Memory[Head]));
		Memory[Head].~T();
		Head = (Head + 1) % Capacity;
		Length--;
		return true;
	}


	template<typename T, ERingQueueThreadPolicy ThreadPolicy>
	bool TRingQueue<T, ThreadPolicy>::InternalPeek(T& OutValue) const
	{
		if (!Memory)
		{
			return false;
		}

		if (Length == 0)
		{
			return false;
		}

		new(&OutValue) T(Memory[Head]);
		return true;
	}
}