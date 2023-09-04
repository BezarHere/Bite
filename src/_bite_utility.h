#pragma once
#include <string>
#include <vector>

#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#define BITE_FORCEINLINE
#endif

#define isupper(n) n >= 'A' && n <= 'Z'
#define islower(n) n >= 'a' && n <= 'z'
#define lower ('a' - 'A')
#define upper ('A' - 'a')

namespace bite
{
	typedef uint8_t byte_t;
	
	// modifies the dst to be a copy of src lowered(uncapitalized) from the start to 'len'
	inline void strlower(char *dst, const char *const src, const size_t len)
	{
		for (size_t i{}; i < len; i++)
		{
			if (isupper(src[i])) dst[i] = src[i] + lower;
		}
	}

	// modifies the dst to be a copy of src uppered(capitalized) from the start to 'len'
	inline void strupper(char *dst, const char *const src, const size_t len)
	{
		for (size_t i{}; i < len; i++)
		{
			if (islower(src[i])) dst[i] = src[i] + upper;
		}
	}


	inline std::string strlower(const std::string &str)
	{
		char *f = new char[str.size() + 1];
		strlower(f, (const char *)str.c_str(), str.size());
		f[str.size()] = '\0';
		return std::move(std::string(f));
	}

	inline std::string strupper(const std::string &str)
	{
		char *f = new char[str.size() + 1];
		strupper(f, (const char *)str.c_str(), str.size());
		f[str.size()] = '\0';
		return std::move(std::string(f));
	}

	// stops at the first null char in 'src'
	// modifies the dst to be a copy of src lowered(uncapitalized) from the start
	// to the index of the first null char in src
	inline void strlower(char *dst, const char *const src)
	{
		size_t i{};
		do
		{
			if (!src[i]) return;
			if (isupper(src[i])) dst[i] = src[i] + lower;
		} while (++i);
	}
	
	// stops at the first null char in 'src'
	// modifies the dst to be a copy of src uppered(capitalized) from the start
	// to the index of the first null char in src
	inline void strupper(char *dst, const char *const src)
		{
		size_t i{};
		do
		{
			if (!src[ i ]) return;
			if (islower(src[ i ])) dst[ i ] = src[ i ] + upper;
		} while (++i);
	}

	// modifies the cstring from the start to len to be upper
	inline void strupper(char *cstr, size_t len)
	{
		strupper(cstr, cstr, len);
	}
	
	// modifies the cstring from the start to len to be lower
	inline void strlower(char *cstr, size_t len)
	{
		strlower(cstr, cstr, len);
	}

	// modifies the cstring to be upper
	inline void strupper(char *cstr)
	{
		strupper(cstr, cstr);
	}
	
	// modifies the cstring to be lower
	inline void strlower(char *cstr)
	{
		strlower(cstr, cstr);
	}

	// retruns: a cstring copy of src to upper with size len + 1
	//	as the last additional char is a null char
	inline char *strupper(const char *const src, size_t len)
	{
		char *temp = new char[len + 1];
		strupper(temp, src, len);
		temp[len] = 0;
		return temp;
	}
	
	// retruns: a cstring copy of src to lower with size len + 1
	//	as the last additional char is a null char
	inline char *strlower(const char *const src, size_t len)
	{
		char *temp = new char[len + 1];
		strlower(temp, src, len);
		temp[len] = 0;
		return temp;
	}

	// stops at the first null char
	// retruns: a cstring copy of src to upper
	inline char *strupper(const char *const src)
	{
		// -1 to remove the added null char/len++ at the other strupper()
		const size_t len = sizeof(src) - 1;
		return strupper(src, len);
	}

	// stops at the first null char
	// retruns: a cstring copy of src to lower
	inline char *strlower(const char *const src)
	{
		// -1 to remove the added null char/len++ at the other strupper()
		const size_t len = sizeof(src) - 1;
		return strlower(src, len);
	}

	// returns a substr *copy* from index to len, doesn't modify the original str
	inline char *strslice(const std::string &src, const size_t index, const size_t len)
	{
		return (char *)memcpy(new char[len + 1] {}, src.c_str() + index, len);
	}

	inline char *strslice(const char *const src, const size_t index, const size_t len)
	{
		return (char *)memcpy(new char[len + 1] {}, src + index, len);
	}

	inline size_t strcount(const std::string &str, const char c, const size_t start = 0, size_t end = 0)
	{
		if (end <= start)
			end = str.length();
		size_t n{};
		for (size_t i{ start }; i < end; i++)
			if (str[ i ] == c) n++;
		return n;
	}

	inline size_t strcount(const char *const cstr, const char c, const size_t len)
	{
		size_t n{};
		for (size_t i{}; i < len; i++)
			if (cstr[ i ] == c) n++;
		return n;
	}

}

template <typename _T>
FORCEINLINE _T *ptrcpy(const _T *const ptr)
{
	return (_T *)memcpy(new _T{}, ptr, sizeof(_T));
}

template <typename _T, class... _Valty>
FORCEINLINE _T *ptrcpy(const _T *const ptr, _Valty&&... _Val)
{
	return (_T *)memcpy(new _T{ _Val... }, ptr, sizeof(_T));
}

template <typename _T>
FORCEINLINE _T *memdup(const _T *const src, const size_t len)
{
	return (_T *)memcpy(new _T[ len ], src, sizeof(_T) * len);
}

FORCEINLINE void *memdup(const void *const src, const size_t len)
{
	return memcpy((void *)new char[len], src, len);
}

// is right and left equal? if true then they are equal! otherwise they are not equal! 
FORCEINLINE bool memeq(const void *const right, const void *const left, const size_t len)
{
	return !memcmp(right, left, len);
}

#ifdef BITE_FORCEINLINE
#undef FORCEINLINE
#undef BITE_FORCEINLINE
#endif
#undef islower
#undef isupper
#undef upper
#undef lower