#pragma once
#include "_bite_utility.h"
#include <utility>

namespace bite
{
	template <typename _T>
	class span
	{
	public:
		using value_type = _T;
		using ptr_type = _T *;
		using cptr_type = const _T *;
		using pair_type = std::pair<ptr_type, ptr_type>;

		inline span() : m_vals{ nullptr, nullptr } {}
		inline span(ptr_type begin, size_t end) : m_vals{ begin, begin + end } {}
		inline span(ptr_type begin, ptr_type end) : m_vals{ begin, end } { _VerfyRange(); }
		inline span(size_t size) : span(new value_type[ size ]{}, size) {}
		template <typename _U>
		inline span(const span<_U> &copy) : m_vals{ (ptr_type)copy.m_vals.first, (ptr_type)copy.m_vals.second } { _VerfyRange(); }
		template <typename _U>
		inline span(span<_U> &&move) : m_vals{ (ptr_type)move.m_vals.first, (ptr_type)move.m_vals.second } { _VerfyRange(); }

		template <typename _U>
		inline span<_T> operator=(const span<_U> &copy)
		{
			copy._VerfyRange();
			m_vals = pair_type( copy.m_vals.first, copy.m_vals.second );
			return *this;
		}

		template <typename _U>
		inline span<_T> operator=(span<_U> &&move)
		{
			move._VerfyRange();
			m_vals = pair_type( move.m_vals.first, move.m_vals.second );
			return *this;
		}

		template <typename _U>
		inline bool operator==(const span<_U> &move)
		{
			move._VerfyRange();
			m_vals = pair_type( move.m_vals.first, move.m_vals.second );
			return *this;
		}

		inline ptr_type begin() noexcept {
			return m_vals.first;
		}

		inline cptr_type begin() const noexcept {
			return m_vals.first;
		}

		inline ptr_type end() noexcept {
			return m_vals.second;
		}

		inline cptr_type end() const noexcept {
			return m_vals.second;
		}

		inline size_t size() const noexcept
		{
			return m_vals.second - m_vals.first;
		}

		inline ptr_type operator[](const size_t index)
		{
#if _CONTAINER_DEBUG_LEVEL > 0
			if (index >= size())
				throw std::exception("Out-of-range index");
#endif // _CONTAINER_DEBUG_LEVEL > 0
			return m_vals[ index ];
		}

		inline const ptr_type operator[](const size_t index) const
		{
#if _CONTAINER_DEBUG_LEVEL > 0
			if (index >= size())
				throw std::exception("Out-of-range index");
#endif // _CONTAINER_DEBUG_LEVEL > 0
			return m_vals[ index ];
		}

		// doesn't check for nullptrs
		inline static bool ValidRange(ptr_type b, ptr_type e)
		{
			return m_vals.first < m_vals.second;
		}

		inline ptr_type data() noexcept
		{
			return m_vals.first;
		}

		inline const ptr_type data() const noexcept
		{
			return m_vals.first;
		}

	private:

		inline void _VerfyRange() const
		{
			if (m_vals.first >= m_vals.second)
				throw std::exception("Invalid span range");
			if (m_vals.second == nullptr || m_vals.first == nullptr)
				throw std::exception("Invalid span ends");
		}


	private:
		pair_type m_vals;
	};
}
