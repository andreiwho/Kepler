#pragma once
#include "Core/Types.h"
#include "Core/Malloc.h"

#include <unordered_map>

namespace ke
{
	template<typename TKey, typename TValue>
	class TChaoticMap
	{
		using TMapType = std::unordered_map<TKey, TValue, std::hash<TKey>, std::equal_to<TKey>, ke::TMallocator<std::pair<const TKey, TValue>>>;
	public:
		TChaoticMap() = default;
		TChaoticMap(const TChaoticMap& other) noexcept 
			: m_Container(other.m_Container) {}
		TChaoticMap& operator=(const TChaoticMap& other) noexcept
		{
			m_Container = other.m_Container;
			return *this;
		}

		TChaoticMap(TChaoticMap&& other) noexcept : m_Container(std::move(other.m_Container)) {}
		TChaoticMap& operator=(TChaoticMap&& other) noexcept
		{
			m_Container = std::move(other.m_Container);
			return *this;
		}

		void Reserve(usize size)
		{
			m_Container.reserve(size);
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

		void Insert(const TKey& key, TValue&& val)
		{
			m_Container[key] = std::move(val);
		}

		void Insert(const TKey& key, const TValue& val)
		{
			m_Container[key] = val;
		}

		TValue& operator[](const TKey& key) noexcept
		{
			return m_Container[key];
		}

		const TValue& operator[](const TKey& key) const noexcept
		{
			return m_Container.at(key);
		}

		usize GetLength() const
		{
			return m_Container.size();
		}

		inline bool IsEmpty() const { return m_Container.empty(); }

		inline decltype(auto) FindIterator(const TKey& key) const
		{
			return m_Container.find(key);
		}

		inline bool Contains(const TKey& key) const
		{
			return FindIterator(key) != m_Container.end();
		}

		inline const TValue* Find(const TKey& key) const
		{
			if (Contains(key))
			{
				return &m_Container.at(key);
			}
			return nullptr;
		}

		inline TMapType& GetUnderlyingContainer() { return m_Container; }
		inline const TMapType& GetUnderlyingContainer() const { return m_Container; }

		inline decltype(auto) begin() { return m_Container.begin(); }
		inline decltype(auto) end() { return m_Container.end(); }
		inline decltype(auto) begin() const { return m_Container.begin(); }
		inline decltype(auto) end() const { return m_Container.end(); }

	private:
		TMapType m_Container;
	};
}