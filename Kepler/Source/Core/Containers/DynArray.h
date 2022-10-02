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

		TDynArray(std::initializer_list<T> NewData)
			:	UnderlyingContainer(NewData.begin(), NewData.end())
		{}

		TDynArray(T* Begin, T* End)
			:	UnderlyingContainer(Begin, End)
		{
		}

		TDynArray(usize InitialSize)
			:	UnderlyingContainer(InitialSize)
		{}

		TDynArray(const TDynArray& Other)
			:	UnderlyingContainer(Other.UnderlyingContainer)
		{
		}

		TDynArray& operator=(const TDynArray& Other)
		{
			UnderlyingContainer = Other.UnderlyingContainer;
			return *this;
		}

		TDynArray(TDynArray&& Other) noexcept
			:	UnderlyingContainer(std::move(Other.UnderlyingContainer))
		{
		}

		TDynArray& operator=(TDynArray&& Other) noexcept
		{
			UnderlyingContainer = std::move(Other.UnderlyingContainer);
			return *this;
		}

		inline void Reserve(usize Size)
		{
			UnderlyingContainer.reserve(Size);
		}
		
		inline void Resize(usize Size)
		{
			UnderlyingContainer.resize(Size);
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

		template<typename TIterType>
		inline bool IsValidIterator(TIterType Iter) const { return Iter != UnderlyingContainer.end(); }

		inline void Shrink()
		{
			UnderlyingContainer.shrink_to_fit();
		}

		inline usize AppendBack(const T& Elem)
		{
			usize idx = GetLength();
			UnderlyingContainer.push_back(Elem);
			return idx;
		}

		inline usize AppendBack(T&& Elem)
		{
			usize idx = GetLength();
			UnderlyingContainer.push_back(std::forward<T>(Elem));
			return idx;
		}

		template<typename ... ARGS>
		inline decltype(auto) EmplaceBack(ARGS&&... Args)
		{
			return UnderlyingContainer.emplace_back(std::forward<ARGS>(Args)...);
		}

		template<typename TIterType, typename ... ARGS>
		inline decltype(auto) Emplace(TIterType Iter, ARGS&&... Args)
		{
			return UnderlyingContainer.emplace(Iter, std::forward<ARGS>(Args)...);
		}

		inline usize GetLength() const { return UnderlyingContainer.size(); }
		
		inline usize GetCapacity() const { return UnderlyingContainer.capacity(); }
		
		inline bool IsEmpty() const { return UnderlyingContainer.empty(); }

		inline auto GetData() { return UnderlyingContainer.data(); }

		inline auto GetData() const { return UnderlyingContainer.data(); }

		inline T& operator[](usize idx)
		{ 
			return UnderlyingContainer[idx];
		} 

		inline const T& operator[](usize idx) const
		{
			return UnderlyingContainer.at(idx);
		}

		template<typename TPred>
		void Sort(TPred Pred)
		{
			std::sort(begin(), end(), Pred);
		}

		template<typename TPred>
		inline decltype(auto) FindIterator(TPred&& Predicate) const
		{
			return std::find_if(std::begin(UnderlyingContainer), std::end(UnderlyingContainer), std::forward<TPred>(Predicate));
		}

		template<typename TPred>
		inline const T* Find(TPred&& Predicate) const
		{
			auto Iter = FindIterator(std::forward<TPred>(Predicate));
			if (Iter != UnderlyingContainer.end())
			{
				return &(*Iter);
			}
			return nullptr;
		}

		template<typename TPred>
		inline T* Find(TPred&& Predicate)
		{
			auto Iter = FindIterator(std::forward<TPred>(Predicate));
			if (Iter != UnderlyingContainer.end())
			{
				return &(*Iter);
			}
			return nullptr;
		}

		// STD interface
		inline decltype(auto) begin() { return UnderlyingContainer.begin(); }
		inline decltype(auto) end() { return UnderlyingContainer.end(); }
		inline decltype(auto) begin() const { return UnderlyingContainer.begin(); }
		inline decltype(auto) end() const { return UnderlyingContainer.end(); }

	private:
		std::vector<T, TAllocator> UnderlyingContainer;
	};
}

#include "DynArray.inl"