#pragma once
#include <string>
#include "_bite_dye.h"

namespace bite
{

	__declspec(noreturn) __forceinline void raise(const std::exception &exc)
	{
		dye::dye(exc, { dye::ColorCode::LightRed, dye::ColorCode::Black }, std::cerr);
		throw exc;
	}

	__declspec(noreturn) __forceinline void raise(const std::string &msg)
	{
		raise(std::runtime_error(msg));
	}

	__declspec(noreturn) __forceinline void raise(const char *const msg)
	{
		raise(std::runtime_error((!msg || !*msg) ? "Error" : msg));
	}

	__forceinline void warn(const char *const msg)
	{
		if (!msg || !*msg)
			return warn("Warning!");
		dye::dye(msg, { dye::ColorCode::LightYellow, dye::ColorCode::Black }, std::cerr);
	}

	__forceinline void warn(const std::string &msg)
	{
		warn(msg.c_str());
	}
}
