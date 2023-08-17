#pragma once
#include "_bite_utility.h"
#include <array>
#include <vector>
#include <memory>

namespace bite
{
	template <typename _T, size_t _Cap>
	class dynarray
	{
	public:
		using value_type = _T;
		using ptr_type = _T*;
		using unique_ptr_type = std::unique_ptr<_T>;
		using container_type = std::array<unique_ptr_type, _Cap>;

		dynarray() : m_cnt{}, m_sz{} {}

		inline bool push_back(const _T &value)
		{
			if (!can_push())
				return false;
			m_cnt[ m_sz++ ].reset(new value_type{ value });
			return true;
		}
		
		inline bool push_back(_T &&value)
		{
			if (!can_push())
				return false;
			m_cnt[ m_sz++ ].reset(new value_type{ value });
			return true;
		}

		inline bool push_back(unique_ptr_type&& value)
		{
			if (!can_push())
				return false;
			m_cnt[ m_sz++ ].reset(value.release());
			return true;
		}
		
		inline void pop(const size_t index)
		{
#if _CONTAINER_DEBUG_LEVEL > 0
			if (index >= size() || index < 0)
				throw std::exception("Out-of-range index");
#endif // _CONTAINER_DEBUG_LEVEL > 0
			_shiftDown(index);
			m_sz--;
		}
		
		inline bool insert(const size_t index, const value_type &value)
		{
			if (!can_push())
				return false;
#if _CONTAINER_DEBUG_LEVEL > 0
			if (index > size() + 1 || index < 0)
				throw std::exception("Out-of-range index");
#endif // _CONTAINER_DEBUG_LEVEL > 0
			if (index <= size())
				_shiftUp(index);
			m_cnt[ index ].reset(new value_type{ value });
			m_sz++;
		}
		
		inline bool insert(const size_t index, unique_ptr_type &&value)
		{
			if (!can_push())
				return false;
#if _CONTAINER_DEBUG_LEVEL > 0
			if (index > size() + 1 || index < 0)
				throw std::exception("Out-of-range index");
#endif // _CONTAINER_DEBUG_LEVEL > 0
			if (index <= size())
				_shiftUp(index);
			m_cnt[ index ].reset(value.release());
			m_sz++;
		}

		inline size_t size() const
		{
			return m_sz;
		}

		inline size_t maxSize() const
		{
			return _Cap;
		}

		inline size_t capacity() const
		{
			return _Cap;
		}

		inline bool can_push() const
		{
			return size() < capacity();
		}

		inline bool empty() const
		{
			return m_sz;
		}

		inline void clear()
		{
			m_sz = 0;
			for (unique_ptr_type &i : m_cnt)
				(void)i.release();
		}

		inline value_type &operator[](const size_t index)
		{
#if _CONTAINER_DEBUG_LEVEL > 0
			if (index >= size() || index < 0)
				throw std::exception("Out-of-range index");
#endif // _CONTAINER_DEBUG_LEVEL > 0
			return *m_cnt[ index ].get();
		}

		inline const value_type &operator[](const size_t index) const
		{
#if _CONTAINER_DEBUG_LEVEL > 0
			if (index >= size() || index < 0)
				throw std::exception("Out-of-range index");
#endif // _CONTAINER_DEBUG_LEVEL > 0
			return *m_cnt[ index ].get();
		}

	private:

		// pivot shouldn't be zero
		inline void _shiftDown(const size_t pivot)
		{
			for (size_t i{ pivot }; i < size(); i++)
				m_cnt[ i - 1 ].reset(m_cnt[i].release());
		}

		// this shouldn't be called when can_push() == false
		inline void _shiftUp(const size_t pivot)
		{
			for (size_t i{ size() }; i >= pivot; i--)
				m_cnt[ i + 1 ].reset(m_cnt[i].release());
		}

		inline void _compact()
		{
			size_t num{};
			for (size_t i{}; i < size() - num; i++)
			{
				if (!m_cnt[ i ])
				{
					_shiftDown(i + 1);
					i--;
				}
			}
		}

	private:
		container_type m_cnt;
		size_t m_sz{0};
	};
}
