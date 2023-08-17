#pragma once
#include "_bite_utility.h"
#include "_bite_dynarray.h"

namespace bite
{
	template <typename _T, size_t _SIZE, typename _C = dynarray<_T, _SIZE>>
	class static_stack
	{
	public:
		using value_type = _T;
		using container_type = _C;
		
		void a()
		{
			m_cnt.size()
		}

	private:
		container_type m_cnt;
	};
}
