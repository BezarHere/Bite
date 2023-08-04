#include "_bite_private.h"

#define isupper(n) n >= 'A' && n <= 'Z'
#define islower(n) n >= 'a' && n <= 'z'
#define lower ('a' - 'A')
#define upper ('A' - 'a')

namespace bite
{

	std::string strlower(const std::string &str)
	{
		std::string new_str{ str };
		for (size_t i{}; i < new_str.size(); i++)
		{
			if (isupper(new_str[ i ])) new_str[ i ] += lower;
		}
		return std::move(new_str);
	}

	std::string strupper(const std::string &str)
	{
		std::string new_str{ str };
		for (size_t i{}; i < new_str.size(); i++)
		{
			if (islower(new_str[ i ])) new_str[ i ] += upper;
		}
		return std::move(new_str);
	}

	void strlower(char *dst, const char *const src, const size_t len)
	{
		for (size_t i{}; i < len; i++)
		{
			if (isupper(src[ i ])) dst[ i ] = src[ i ] + lower;
		}
	}

	void strupper(char *dst, const char *const src, const size_t len)
	{
		for (size_t i{}; i < len; i++)
		{
			if (islower(src[ i ])) dst[ i ] = src[ i ] + upper;
		}
	}

	void strlower(char *dst, const char *const src)
	{
		size_t i{};
		do
		{
			if (!src[ i ]) return;
			if (isupper(src[ i ])) dst[ i ] = src[ i ] + lower;
		} while (++i);
	}

	void strupper(char *dst, const char *const src)
	{
		size_t i{};
		do
		{
			if (!src[ i ]) return;
			if (islower(src[ i ])) dst[ i ] = src[ i ] + upper;
		} while (++i);
	}

	void strupper(char *cstr, size_t len)
	{
		return strupper(cstr, (const char *const)cstr, len);
	}

	void strlower(char *cstr, size_t len)
	{
		return strlower(cstr, (const char *const)cstr, len);
	}

	void strupper(char *cstr)
	{
		return strupper(cstr, (const char *const)cstr);
	}

	void strlower(char *cstr)
	{
		return strlower(cstr, (const char *const)cstr);
	}
}
