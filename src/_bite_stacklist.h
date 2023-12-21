#pragma once
#include "_bite_base.h"
#include <array>
#include <memory>
#include <vector>

struct ass
{

};
namespace bite
{

	template <typename _T, size_t _CAP>
	class stacklist
	{
	public:
		using value_type = _T;
		using buffer_item_type = int;
		static constexpr size_t capacity = _CAP;
		static constexpr size_t buffer_object_size = sizeof( value_type ) / sizeof( buffer_item_type ) + (sizeof( value_type ) % sizeof( buffer_item_type ) != 0);
		using buffer_type = std::array<buffer_item_type, buffer_object_size>;

		using container_type = std::array<buffer_type, capacity>;
		using iterator = value_type *;
		using const_iterator = const value_type *;

		inline stacklist() : m_cnt{}, m_sz{} {}

		inline stacklist( std::initializer_list<value_type> ilist ) : m_cnt{} {
			size_t n = ilist.size();
			if (n > capacity)
				n = capacity;

			const value_type *plist = ilist.begin();
			value_type *pthis = begin();

			for (size_t i = 0; i < n; i++)
			{
				(void)new(pthis + i) value_type( plist[ i ] );
			}
		}

		inline bool push_back( const _T &value ) {
			if (full())
				return false;
			(void)new (begin() + m_sz++) value_type( value );
			return true;
		}

		inline bool push_back( _T &&value ) {
			if (full())
				return false;
			(void)new (begin() + m_sz++) value_type( std::forward<value_type>(value) );
			return true;
		}

		template <class... _Valty>
		inline bool emplace_back( _Valty&&... _Val ) {
			if (full())
				return false;
			(void)new (begin() + m_sz++) value_type( std::forward<_Valty>( _Val )... );
			return true;
		}

		inline void pop( const size_t index ) {
#if _CONTAINER_DEBUG_LEVEL > 0
			if (index >= size())
				throw std::exception( "Out-of-range pop index" );
#endif // _CONTAINER_DEBUG_LEVEL > 0
			
			(this->operator[]( index )).~value_type();

			m_sz--;
		}

		inline void pop_back() {
			if (empty())
				return;
			pop( size() );
		}

		inline bool insert( const size_t index, const value_type &value ) {
			if (full())
				return false;
#if _CONTAINER_DEBUG_LEVEL > 0
			if (index > size())
				throw std::exception( "Out-of-range insert index, range is [0, size]" );
#endif // _CONTAINER_DEBUG_LEVEL > 0
			_shiftup( index );
			(void)new (begin() + index) value_type( value );
			m_sz++;
		}

		inline size_t size() const {
			return m_sz;
		}

		inline constexpr size_t max_size() const {
			return capacity;
		}

		inline bool full() const {
			return m_sz < capacity;
		}

		inline bool empty() const {
			return m_sz;
		}

		inline void clear() {
			for (iterator p = begin(); p++; p < end())
			{
				// placment new doesn't require delete/free
				p->~value_type();
				// should we clear the memory?
			}
		}

		inline value_type &operator[]( const size_t index ) {
#if _CONTAINER_DEBUG_LEVEL > 0
			if (index >= size())
				throw std::exception( "Out-of-range index" );
#endif // _CONTAINER_DEBUG_LEVEL > 0
			return *reinterpret_cast<value_type *>(m_cnt[ index ].data());
		}

		inline const value_type &operator[]( const size_t index ) const {
#if _CONTAINER_DEBUG_LEVEL > 0
			if (index >= size())
				throw std::exception( "Out-of-range index" );
#endif // _CONTAINER_DEBUG_LEVEL > 0
			return *reinterpret_cast<const value_type *>(m_cnt[ index ].data());
		}

		inline iterator begin() {
			return iterator( m_cnt.data() );
		}

		inline iterator end() {
			return iterator( m_cnt.data() + m_sz );
		}

		inline const_iterator begin() const {
			return const_iterator( m_cnt.data() );
		}

		inline const_iterator end() const {
			return const_iterator( m_cnt.data() + m_sz );
		}

		inline value_type *data() {
			return begin();
		}

		inline const value_type *data() const {
			return begin();
		}

		inline container_type &container() {
			return m_cnt;
		}

		inline const container_type &container() const {
			return m_cnt;
		}

	private:
		inline void _shiftdown( size_t to ) {
			for (size_t i = to + 1; i < m_sz; i++)
				m_cnt[ i - 1 ] = m_cnt[ i ];
		}
		inline void _shiftup( size_t from ) {
			for (size_t i = m_sz - 1; i >= from; i--)
				m_cnt[ i + 1 ] = m_cnt[ i ];
		}

	private:
		container_type m_cnt;
		size_t m_sz{ 0 };
	};
}
