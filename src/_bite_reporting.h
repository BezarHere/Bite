#pragma once
#include "_bite_utility.h"
#include "_bite_dye.h"

namespace bite
{

	__forceinline void raise(const std::exception &exc)
	{
		dye::put_colors({ dye::ColorCode::Red, dye::ColorCode::Black });
		std::cerr << exc.what() << '\n';
		dye::clear_colors();
		throw exc;
	}

	__forceinline void raise(const std::string &msg)
	{
		raise(std::runtime_error(msg));
	}


	__forceinline void raise(const char *const msg)
	{
		raise(std::runtime_error(msg));
	}

	__forceinline void warn(const char *const msg)
	{
		if (!msg)
			return warn("Warning!");
		dye::put_colors({ dye::ColorCode::LightYellow, dye::ColorCode::Black });
		std::cerr << msg << '\n';
		dye::clear_colors();
	}

	__forceinline void warn(const std::string &msg)
	{
		dye::put_colors({ dye::ColorCode::LightYellow, dye::ColorCode::Black });
		std::cerr << msg << '\n';
		dye::clear_colors();
	}
}
