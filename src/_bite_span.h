#pragma once
#include "_bite_base.h"
#include "_bite_range.h"
#include <utility>



namespace bite
{

	// takes ownership of pointers
	template <typename _T>
	class span
	{
	public:
		using value_type = _T;
		using reference_type = _T &;
		using ptr_type = value_type *;
		using const_ptr_type = const value_type *;
		using smart_ptr_type = std::shared_ptr<value_type[]>;
		using cptr_type = const _T *;
		using pair_type = std::pair<smart_ptr_type, size_t>;
		using this_type = span<value_type>;

		inline static constexpr pair_type copy_range(const_ptr_type const begin, const size_t size)
		{
			ptr_type p = (ptr_type)memcpy(new value_type[ size ], begin, sizeof(value_type) * size);
			return { smart_ptr_type(p), size };
		}

		inline static constexpr void copy_range_to(const_ptr_type const begin, const size_t size, pair_type &pair)
		{
			ptr_type p = (ptr_type)memcpy(new value_type[ size ], begin, sizeof(value_type) * size);
			pair.first.reset(p);
			pair.second = size;
		}

		inline span() : m_vals{ nullptr, 0 } {  }
		inline span(ptr_type begin, ptr_type end) : m_vals{ copy_range(begin, end - begin) } { _VerfyRange(); }

		// creates a span from a container (e.g. std::vector, std::array)
		// Not recomended
		template <typename _C>
		inline span(const _C &cnt) : m_vals{ copy_range(cnt.begin(), cnt.size()) } { _VerfyRange(); }

		inline explicit span(ptr_type begin, size_t end) : m_vals{ copy_range(begin, end) } {}
		inline explicit span(size_t size) : span(new value_type[ size ]{}, size) {}

		template <typename _U>
		inline span(const span<_U> &copy) {
			copy._VerfyRange();
			_Tidy();
			copy_range_to(copy.m_vals.first.get(), copy.m_vals.second, m_vals);
		}
		template <typename _U>
		inline span(span<_U> &&move)
		{
			_Tidy();
			m_vals.first.reset(move.m_vals.first.release());
			m_vals.second = move.m_vals.second;
		}


		// clear memory
		inline ~span()
		{
		}

		inline void swap(span<_T> &other) noexcept
		{
			if (this != std::addressof(other))
			{
				// swaping ranges will be enough
				m_vals.first.swap(other.m_vals.first);
				const size_t end = m_vals.second;
				m_vals.second = other.m_vals.second;
				other.m_vals.second = end;
			}
		}

		template <typename _U>
		inline span<_T> &operator=(const span<_U> &copy)
		{
			copy._VerfyRange();
			copy_range_to(copy.m_vals.first.get(), copy.m_vals.second, m_vals);
			return *this;
		}

		template <typename _U>
		inline span<_T> &operator=(span<_U> &&move)
		{
			m_vals.first.reset(move.m_vals.first.release());
			m_vals.second = move.m_vals.second;
			return *this;
		}

		inline span<_T> &operator=(const span<_T> &copy)
		{
			copy._VerfyRange();
			copy_range_to(copy.m_vals.first.get(), copy.m_vals.second, m_vals);
			return *this;
		}

		inline span<_T> &operator=(span<_T> &&move)
		{
			m_vals.first.reset(move.m_vals.first.release());
			m_vals.second = move.m_vals.second;
			return *this;
		}

		template <typename _U>
		inline bool operator==(const span<_U> &eq)
		{
			return eq.begin() == begin() && eq.end() == end();
		}

		inline ptr_type begin() noexcept {
			return m_vals.first.get();
		}

		inline cptr_type begin() const noexcept {
			return m_vals.first.get();
		}

		inline ptr_type end() noexcept {
			return m_vals.first.get() + m_vals.second;
		}

		inline cptr_type end() const noexcept {
			return m_vals.first.get() + m_vals.second;
		}

		inline size_t size() const noexcept
		{
			return m_vals.second;
		}

		inline reference_type operator[](const size_t index)
		{
#if _CONTAINER_DEBUG_LEVEL > 0
			if (index >= size())
				throw std::exception("Out-of-range index");
#endif // _CONTAINER_DEBUG_LEVEL > 0
			return m_vals.first.get()[ index ];
		}

		inline const reference_type operator[](const size_t index) const
		{
#if _CONTAINER_DEBUG_LEVEL > 0
			if (index >= size())
				throw std::exception("Out-of-range index");
#endif // _CONTAINER_DEBUG_LEVEL > 0
			return m_vals.first.get()[ index ];
		}

		inline const this_type &operator[](const range slice_range) const
		{
#if _CONTAINER_DEBUG_LEVEL > 0
			if (slice_range.max >= size() || !slice_range)
				throw std::exception("Out-of-range index");
#endif // _CONTAINER_DEBUG_LEVEL > 0
			if (!slice_range.size())
			{
				return this_type();
			}
			// automaticly copied
			return this_type(m_vals.first.get() + slice_range.min, slice_range.size());
		}

		// doesn't check for nullptrs
		inline static bool ValidRange(ptr_type b, ptr_type e)
		{
			return b < e;
		}

		inline smart_ptr_type &data() noexcept
		{
			return m_vals.first;
		}

		inline const smart_ptr_type &data() const noexcept
		{
			return m_vals.first;
		}

	private:
		inline void _VerfyRange() const
		{
			if (!m_vals.first || !m_vals.second)
				throw std::exception("Invalid span ends");
		}

		inline void _Tidy() noexcept
		{
			m_vals.first.reset();
			_Release();
		}

		inline void _Release() noexcept
		{
			m_vals.first = nullptr;
			m_vals.second = 0;
		}

	private:
		pair_type m_vals;
	};
}
