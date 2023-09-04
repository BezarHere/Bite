#pragma once
#include "_bite_utility.h"
#include <array>
#include <memory>
#include <vector>

namespace bite
{
	template <typename _T>
	class _const_dynarray_iterator
	{
	public:
		
		using unique_dptr = const std::unique_ptr<_T>*;

		inline _const_dynarray_iterator(unique_dptr value)
			: m_val{ value }
		{}

		inline const _T *operator->() const
		{
			std::fill();
			return m_val->get();
		}

		inline const _T &operator*() const
		{
			return *(m_val->get());
		}

		inline _const_dynarray_iterator<_T> &operator++()
		{
			m_val++;
			return *this;
		}

		inline _const_dynarray_iterator<_T> &operator--()
		{
			m_val--;
			return *this;
		}

		inline bool operator==(const _const_dynarray_iterator<_T> &other)
		{
			return other.m_val == m_val;
		}

		inline bool operator!=(const _const_dynarray_iterator<_T> &other)
		{
			return other.m_val != m_val;
		}

		inline operator bool()
		{
			return m_val->get();
		}

		inline operator const _T*()
		{
			return m_val->get();
		}

	private:
		_const_dynarray_iterator() = delete;

		template <typename _Tc>
		_const_dynarray_iterator<_T> &operator=(_const_dynarray_iterator<_Tc>) = delete;

	private:
		unique_dptr m_val;
	};

	template <typename _T>
	class _dynarray_iterator
	{
	public:
		using _MyBase = _const_dynarray_iterator<_T>;
		using unique_dptr = std::unique_ptr<_T>*;

		inline _dynarray_iterator(unique_dptr value)
			: m_val(value)
		{
		}

		inline _T *operator->()
		{
			return m_val->get();
		}

		inline _T &operator*()
		{
			return *(m_val->get());
		}

		inline _dynarray_iterator<_T> &operator++()
		{
			m_val++;
			return *this;
		}

		inline _dynarray_iterator<_T> &operator--()
		{
			m_val--;
			return *this;
		}

		inline bool operator==(const _dynarray_iterator<_T> &other)
		{
			return other.m_val == m_val;
		}

		inline bool operator!=(const _dynarray_iterator<_T> &other)
		{
			return other.m_val != m_val;
		}

		inline operator bool()
		{
			return m_val->get();
		}

		inline operator _T*()
		{
			return m_val->get();
		}

	private:
		_dynarray_iterator() = delete;


		template <typename _Tc>
		_dynarray_iterator<_T> &operator=(_dynarray_iterator<_Tc>) = delete;
	private:
		unique_dptr m_val;
	};

	// not ideal for small types, term 'small types' is smaller then a size_t
	template <typename _T, size_t _Cap>
	class dynarray
	{
	public:
		using value_type = _T;
		using ptr_type = _T*;
		using unique_ptr_type = std::unique_ptr<_T>;
		using container_type = std::array<unique_ptr_type, _Cap>;
		using iterator = _dynarray_iterator<_T>;
		using const_iterator = _const_dynarray_iterator<_T>;

		inline dynarray() : m_cnt{}, m_sz{} {}
		// will take owner ship of range from begin to begin + end
		inline dynarray(ptr_type begin, size_t end) : m_cnt{}
		{
			m_sz = max(_Cap, end);
			(void)memcpy(m_cnt.data(), begin, m_sz);
		}

		// will take owner ship of range from begin to end
		inline dynarray(ptr_type begin, ptr_type end) : m_cnt{}
		{
			if (end < begin)
				throw std::exception("Invalid begin-end range");
			m_sz = max(_Cap, end - begin);
			(void)memcpy(m_cnt.data(), begin, m_sz);
		}

		inline dynarray(std::initializer_list<ptr_type> ilist) : m_cnt{}
		{
			m_sz = max(_Cap, m_sz);
			(void)memcpy(m_cnt.data(), ilist.begin(), m_sz);
		}

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

		template <class... _Valty>
		inline bool emplace_back(_Valty&&... _Val)
		{
			return push_back(value_type(std::forward<_Valty>(_Val)...));
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
		
		inline void pop_back()
		{
			if (!size())
				return;
			(void)m_cnt[ --m_sz ].release();
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

		inline size_t max_size() const
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

		inline iterator begin()
		{
			return iterator( m_cnt.data() );
		}

		inline iterator end()
		{
			return iterator(m_cnt.data() + m_sz);
		}

		inline const_iterator begin() const
		{
			return const_iterator(m_cnt.data());
		}

		inline const_iterator end() const
		{
			return const_iterator(m_cnt.data() + m_sz);
		}

		inline unique_ptr_type *data()
		{
			return m_cnt.data();
		}

		inline const unique_ptr_type *data() const
		{
			return m_cnt.data();
		}

		inline container_type &container()
		{
			return m_cnt;
		}

		inline const container_type &container() const
		{
			return m_cnt;
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
			for (size_t i{}; i < size(); i++)
			{
				if (!m_cnt[ i ])
				{
					size_t count{ 1 };
					// counting all invalid items
					while (!m_cnt[ i + count++ ]);

					if (count >= size())
					{
						clear();
						return;
					}

					for (size_t j{ i + count }; j < size(); j++)
						m_cnt[ j - count ].reset(m_cnt[ j ].release());

					m_sz -= count;
				}
			}
		}

	private:
		container_type m_cnt;
		size_t m_sz{0};
	};
}
