#include "_bite_utility.h"

#define isupper(n) n >= 'A' && n <= 'Z'
#define islower(n) n >= 'a' && n <= 'z'
#define lower ('a' - 'A')
#define upper ('A' - 'a')

namespace bite
{

	std::string strlower(const std::string &str)
	{
		char *f = new char[ str.size() + 1 ];
		strlower(f, (const char *)str.c_str(), str.size());
		f[ str.size() ] = '\0';
		return std::move(std::string(f));
	}

	std::string strupper(const std::string &str)
	{
		char *f = new char[ str.size() + 1 ];
		strupper(f, (const char *)str.c_str(), str.size());
		f[ str.size() ] = '\0';
		return std::move(std::string(f));
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

	char *strupper(const char *const src, size_t len)
	{
		char *temp = new char[ len + 1 ];
		strupper(temp, src, len);
		temp[ len ] = 0;
		return temp;
	}

	char *strlower(const char *const src, size_t len)
	{
		char *temp = new char[ len + 1 ];
		strlower(temp, src, len);
		temp[ len ] = 0;
		return temp;
	}

	char *strupper(const char *const src)
	{
		// -1 to remove the added null char/len++ at the other strupper()
		const size_t len = sizeof(src) - 1;
		return strupper(src, len);
	}

	char *strlower(const char *const src)
	{
		// -1 to remove the added null char/len++ at the other strupper()
		const size_t len = sizeof(src) - 1;
		return strlower(src, len);
	}
	char *strslice(const std::string &src, const size_t index, const size_t len)
	{
		return (char *)memcpy(new char[len + 1] {}, src.c_str() + index, len);
	}
	char *strslice(const char *const src, const size_t index, const size_t len)
	{
		return (char *)memcpy(new char[len + 1] {}, src + index, len);
	}
}
