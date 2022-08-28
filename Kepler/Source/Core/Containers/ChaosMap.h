#pragma once
#include "Core/Types.h"
#include "Core/Malloc.h"

#include <unordered_map>

namespace Kepler
{
	template<typename TKey, typename TValue>
	class TChaoticMap
	{
		using TMapType = std::unordered_map<TKey, TValue, std::hash<TKey>, std::equal_to<TKey>, Kepler::TMallocator<std::pair<const TKey, TValue>>>;
	public:
		TChaoticMap() = default;
		TChaoticMap(const TChaoticMap& Other) noexcept : UnderlyingContainer(Other.UnderlyingContainer) {}
		TChaoticMap& operator=(const TChaoticMap& Other) noexcept
		{
			UnderlyingContainer = Other.UnderlyingContainer;
			return *this;
		}

		TChaoticMap(TChaoticMap&& Other) noexcept : UnderlyingContainer(std::move(Other.UnderlyingContainer)) {}
		TChaoticMap& operator=(TChaoticMap&& Other) noexcept
		{
			UnderlyingContainer = std::move(Other.UnderlyingContainer);
			return *this;
		}

		void Reserve(usize Size)
		{
			UnderlyingContainer.reserve(Size);
		}

		void Clear()
		{
			UnderlyingContainer.clear();
		}

		template<typename TIterType>
		void Remove(TIterType At)
		{
			UnderlyingContainer.erase(At);
		}

		void Insert(const TKey& Key, TValue&& Value)
		{
			UnderlyingContainer[Key] = std::move(Value);
		}

		void Insert(const TKey& Key, const TValue& Value)
		{
			UnderlyingContainer[Key] = Value;
		}

		TValue& operator[](const TKey& Key) noexcept
		{
			return UnderlyingContainer[Key];
		}

		const TValue& operator[](const TKey& Key) const noexcept
		{
			return UnderlyingContainer.at(Key);
		}

		usize GetLength() const
		{
			return UnderlyingContainer.size();
		}

		inline bool IsEmpty() const { return UnderlyingContainer.empty(); }

		inline decltype(auto) FindIterator(const TKey& Key) const
		{
			return UnderlyingContainer.find(Key);
		}

		inline bool Contains(const TKey& Key) const
		{
			return FindIterator(Key) != UnderlyingContainer.end();
		}

		inline const TValue* Find(const TKey& Key) const
		{
			if (Contains(Key))
			{
				return &UnderlyingContainer.at(Key);
			}
			return nullptr;
		}

		inline TMapType& GetUnderlyingContainer() { return UnderlyingContainer; }
		inline const TMapType& GetUnderlyingContainer() const { return UnderlyingContainer; }

		inline decltype(auto) begin() { return UnderlyingContainer.begin(); }
		inline decltype(auto) end() { return UnderlyingContainer.end(); }
		inline decltype(auto) begin() const { return UnderlyingContainer.begin(); }
		inline decltype(auto) end() const { return UnderlyingContainer.end(); }

	private:
		TMapType UnderlyingContainer;
	};
}