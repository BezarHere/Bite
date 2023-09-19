#pragma once
#include "_bite_utility.h"

//namespace bite
//{
//	consteval size_t INVALID_INDEX = (size_t)-1;
//	// SIZE as in: how much of T should this hold?
//	template <typename T, size_t SIZE>
//	class MemoryPool
//	{
//	public:
//		inline ~MemoryPool()
//		{
//			for (size_t i{}; i < SIZE; i++)
//			{
//				if (m_space[ i ])
//				{
//					free(m_pool + i);
//				}
//			}
//		}
//
//		MemoryPool(const MemoryPool &copy) = delete;
//		void operator=(const MemoryPool<T, SIZE> &other) = delete;
//
//		inline T *alloc()
//		{
//			const size_t fs = _firstFreeSpace();
//			m_space[ fs ] = true;
//			return new(m_pool + fs) T{};
//		}
//
//		template <class ...Vals>
//		inline T *alloc(Vals&&... vals)
//		{
//			const size_t fs = _firstFreeSpace();
//			m_space[ fs ] = true;
//			return new(m_pool + fs) T{ std::forward(vals)... };
//		}
//		
//		inline void free(T *const ptr)
//		{
//			if (!_hasPtr(ptr))
//				return;
//			ptr->~T();
//			memset(ptr, 0, sizeof(T));
//		}
//
//
//		inline size_t _firstFreeSpace() const
//		{
//			for (size_t i{}; i < SIZE; i++)
//			{
//				if (!m_space[ i ])
//					return i;
//			}
//			return (size_t)-1;
//		}
//
//
//		__forceinline bool _hasPtr(const T *const ptr) const
//		{
//			return (ptr - m_pool) < SIZE;
//		}
//
//	private:
//		T m_pool[ SIZE ];
//		bool m_space[ SIZE ];
//	};
//
//}
