#pragma once
#include <string>

#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#define BITE_FORCEINLINE
#endif

namespace bite
{
	std::string strlower(const std::string &str);
	std::string strupper(const std::string &str);

	void strlower(char *dst, const char *const src, const size_t len);
	void strupper(char *dst, const char *const src, const size_t len);

	void strlower(char *dst, const char *const src);
	void strupper(char *dst, const char *const src);

	void strupper(char *cstr, size_t len);
	void strlower(char *cstr, size_t len);

	void strupper(char *cstr);
	void strlower(char *cstr);

}

template <typename _T>
FORCEINLINE _T *ptrcpy(const _T *ptr)
{
	return (_T *)memcpy(new _T{}, ptr, sizeof(_T));
}

template <typename _T, class... _Valty>
FORCEINLINE _T *ptrcpy(const _T *ptr, _Valty&&... _Val)
{
	return (_T *)memcpy(new _T{ _Val... }, ptr, sizeof(_T));
}

#ifdef BITE_FORCEINLINE
#undef FORCEINLINE
#undef BITE_FORCEINLINE
#endif