#pragma once
#include "Core/Types.h"
#include "Core/Malloc.h"
#include <initializer_list>

namespace ke
{
	template<typename T, typename TAllocator = TMallocator<T>>
	class TDynArray
	{
	public:
		TDynArray() = default;

		TDynArray(std::initializer_list<T> data)
			:	m_Container(data.begin(), data.end())
		{}

		TDynArray(T* pBegin, T* pEnd)
			:	m_Container(pBegin, pEnd)
		{
		}

		TDynArray(usize size)
			:	m_Container(size)
		{}

		TDynArray(const TDynArray& other)
			:	m_Container(other.m_Container)
		{
		}

		TDynArray& operator=(const TDynArray& other)
		{
			m_Container = other.m_Container;
			return *this;
		}

		TDynArray(TDynArray&& other) noexcept
			:	m_Container(std::move(other.m_Container))
		{
		}

		TDynArray& operator=(TDynArray&& other) noexcept
		{
			m_Container = std::move(other.m_Container);
			return *this;
		}

		inline void Reserve(usize size)
		{
			m_Container.reserve(size);
		}
		
		inline void Resize(usize size)
		{
			m_Container.resize(size);
		}

		void Clear()
		{
			m_Container.clear();
		}

		template<typename TIterType>
		void Remove(TIterType at)
		{
			m_Container.erase(at);
		}

		template<typename TIterType>
		inline bool IsValidIterator(TIterType iter) const { return iter != m_Container.end(); }

		inline void Shrink()
		{
			m_Container.shrink_to_fit();
		}

		inline usize AppendBack(const T& el)
		{
			usize idx = GetLength();
			m_Container.push_back(el);
			return idx;
		}

		inline usize AppendBack(T&& el)
		{
			usize idx = GetLength();
			m_Container.push_back(std::forward<T>(el));
			return idx;
		}

		template<typename ... ARGS>
		inline decltype(auto) EmplaceBack(ARGS&&... Args)
		{
			return m_Container.emplace_back(std::forward<ARGS>(Args)...);
		}

		template<typename TIterType, typename ... ARGS>
		inline decltype(auto) Emplace(TIterType Iter, ARGS&&... Args)
		{
			return m_Container.emplace(Iter, std::forward<ARGS>(Args)...);
		}

		inline usize GetLength() const { return m_Container.size(); }
		
		inline usize GetCapacity() const { return m_Container.capacity(); }
		
		inline bool IsEmpty() const { return m_Container.empty(); }

		inline auto GetData() { return m_Container.data(); }

		inline auto GetData() const { return m_Container.data(); }

		inline T& operator[](usize idx)
		{ 
			return m_Container[idx];
		} 

		inline const T& operator[](usize idx) const
		{
			return m_Container.at(idx);
		}

		template<typename TPred>
		void Sort(TPred Pred)
		{
			std::sort(begin(), end(), Pred);
		}

		template<typename TPred>
		inline decltype(auto) FindIterator(TPred&& predicate) const
		{
			return std::find_if(std::begin(m_Container), std::end(m_Container), std::forward<TPred>(predicate));
		}

		template<typename TPred>
		inline const T* Find(TPred&& predicate) const
		{
			auto Iter = FindIterator(std::forward<TPred>(predicate));
			if (Iter != m_Container.end())
			{
				return &(*Iter);
			}
			return nullptr;
		}

		template<typename TPred>
		inline T* Find(TPred&& predicate)
		{
			auto Iter = FindIterator(std::forward<TPred>(predicate));
			if (Iter != m_Container.end())
			{
				return &(*Iter);
			}
			return nullptr;
		}

		// STD interface
		inline decltype(auto) begin() { return m_Container.begin(); }
		inline decltype(auto) end() { return m_Container.end(); }
		inline decltype(auto) begin() const { return m_Container.begin(); }
		inline decltype(auto) end() const { return m_Container.end(); }

	private:
		std::vector<T, TAllocator> m_Container;
	};
}