#pragma once
#include "_bite_utility.h"


namespace std
{
	inline string to_string(const char *_Ptr)
	{
		return string(_Ptr);
	}
}

namespace bite
{
	template <typename _Elm>
	class basic_string_view
	{
	public:
		using string_type = std::basic_string<_Elm>;
		using ptr_type = _Elm *;
		using const_ptr_type = const _Elm *;
		using size_type = typename string_type::size_type;

		inline basic_string_view(const string_type &str)
			: m_buf{ str.c_str() }, m_sz{ str.length() }
		{
		}

		inline basic_string_view(const_ptr_type cstr, const size_type sz)
			: m_buf{ cstr }, m_sz{ sz }
		{
		}

		inline basic_string_view(const_ptr_type cstr)
			: basic_string_view(cstr, strlen(cstr))
		{
		}


		inline size_type length() const
		{
			return m_sz;
		}

		inline size_type size() const
		{
			return m_sz;
		}

		inline const_ptr_type c_str()
		{
			return m_buf;
		}

		inline const const_ptr_type c_str() const
		{
			return m_buf;
		}

		inline const_ptr_type data()
		{
			return m_buf;
		}

		inline const const_ptr_type data() const
		{
			return m_buf;
		}

		inline const_ptr_type begin()
		{
			return m_buf;
		}

		inline const const_ptr_type begin() const
		{
			return m_buf;
		}

		inline const_ptr_type end()
		{
			return m_buf + m_sz;
		}

		inline const const_ptr_type end() const
		{
			return m_buf + m_sz;
		}

	private:
		const const_ptr_type m_buf;
		const size_type m_sz; // including terminating zero
	};

	using string_view = basic_string_view<char>;
	using wstring_view = basic_string_view<wchar_t>;

	template <typename _T>
	inline std::string format(const std::string &str, const _T &v1)
	{
		return format(str, std::to_string(v1));
	}

	// TODO: update to replace more then one marker!
	template <>
	inline std::string format(const std::string &str, const std::string &sub)
	{
		const char *cstr = str.c_str();
		const size_t n = str.length();

		const char *sub_cstr = sub.c_str();
		const size_t sub_n = sub.length();

		std::vector<size_t> replace_anchors{};

		for (size_t i = 0; i < n - 1; i++)
		{
			// skip escaped chars
			if (cstr[ i ] == '\\')
			{
				i++;
				continue;
			}

			if (cstr[ i ] == '{' && cstr[ i + 1 ] == '}')
				replace_anchors.push_back(i++);
		}

		// no anchor to format
		if (!replace_anchors.size())
			return str;

		const size_t final_n = n + ((-2 + sub_n) * replace_anchors.size());
		span<char> result_cstr{ final_n };


		size_t write_offset = 0;
		size_t read_offset = 0;
		size_t replacment_str_index = 0;

		for (size_t i = 0; i < n - replace_anchors.size(); i++)
		{
			// this index is the position of a repacment string
			if (i + read_offset == replace_anchors[ replacment_str_index ])
			{

				for (size_t j = 0; j < sub_n; j++)
				{
					result_cstr[ write_offset++ ] = sub_cstr[ j ];
				}

				read_offset++;
				// now processing the next replacment
				replacment_str_index++;
			}
			else
				// no replacment str, fill with the base str chars
				result_cstr[ write_offset++ ] = str[ i + read_offset ];
		}

		return std::string{ result_cstr.data().get(), result_cstr.size() };
	}

	inline std::string formatv(const std::string &str, std::initializer_list<std::string> s)
	{
		const char *cstr = str.c_str();
		const size_t n = str.length();
		const size_t sz = s.size();
		size_t c_sn = 0, replace_tn = 0;
		span<size_t> replacment_str_pos{ sz };

		for (size_t i = 0; i < n - 1; i++)
		{
			// skip escaped chars
			if (cstr[ i ] == '\\')
			{
				i++;
				continue;
			}

			if (cstr[ i ] == '{' && cstr[ i + 1 ] == '}')
			{
				replace_tn += s.begin()[ c_sn ].length();
				replacment_str_pos[ c_sn++ ] = i++;
				if (c_sn >= sz)
					break;
			}
		}

		const size_t final_n = n - (c_sn * 2) + replace_tn;
		span<char> result_cstr{ final_n };

		size_t write_offset = 0;
		size_t read_offset = 0;
		size_t replacment_str_index = 0;

		for (size_t i = 0; i < n - c_sn; i++)
		{
			// this index is the position of a repacment string
			if (i + read_offset == replacment_str_pos[ replacment_str_index ])
			{
				const size_t rs_len = s.begin()[ replacment_str_index ].length();
				const char *const res_cstr = s.begin()[ replacment_str_index ].c_str();

				for (size_t j = 0; j < rs_len; j++)
				{
					result_cstr[ write_offset++ ] = res_cstr[ j ];
				}

				read_offset++;
				// now processing the next replacment
				replacment_str_index++;
			}
			else
				// no replacment str, fill with the base str chars
				result_cstr[ write_offset++ ] = str[ i + read_offset ];
		}

		return std::string{ result_cstr.data().get(), result_cstr.size() };
	}

	template <typename _T>
	inline void _format_unpack_stringfy(std::string *s, const _T &t)
	{
		(*s) = std::to_string(t);
	}

	template <typename _T, typename ..._VT>
	inline void _format_unpack_stringfy(std::string *s, const _T &t, const _VT &... v)
	{
		(*s++) = std::to_string(t);
		_format_unpack_stringfy(s, v...);
	}

	template <typename ..._Vals>
	inline std::string format(const std::string &str, const _Vals &... vals)
	{
		constexpr size_t ArgC = sizeof...(vals);
		std::string strs[ ArgC ];
		const std::string *pstr = strs;
		size_t i = 0;

		/// WHY IS IT GIVING ME ERRORS!!1!1! IT COMPILES FINE SO HOW IS IT AN ERROR!!12!2
		_format_unpack_stringfy(strs + i++, vals...);
		return formatv(str, std::initializer_list<std::string>(pstr, pstr + ArgC));
	}


}

