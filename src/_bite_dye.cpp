#include "_bite_dye.h"
#include <Windows.h>

namespace bite
{
	namespace dye
	{

		static const TerminalTheme F_DefaultTheme{ M_GetTheme() };

		TerminalTheme M_GetTheme()
		{
			return TerminalTheme{};
		}

		TerminalTheme M_GetDefaultTheme()
		{
			return F_DefaultTheme;
		}

		void M_PutTheme(TerminalTheme theme)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)M_UnpackTheme(theme));
		}

		void M_ClearTheme()
		{
			M_PutTheme(F_DefaultTheme);
		}

		constexpr uint8_t M_UnpackTheme(TerminalTheme theme)
		{
			return (uint8_t)theme.fg + ((uint8_t)theme.bg << 4);
		}

		constexpr TerminalTheme M_PackTheme(uint8_t theme)
		{
			return { (ColorCode)(theme & 0xf), (ColorCode)(theme >> 4) };
		}

		void M_Dye(const std::string &text, TerminalTheme colors, std::ostream &stream)
		{
			TerminalTheme def_t = M_GetTheme();
			M_PutTheme(colors);
			stream << text;
			M_PutTheme(def_t);
		}

		void M_Dye(const char *text, TerminalTheme colors, std::ostream &stream)
		{
			TerminalTheme def_t = M_GetTheme();
			M_PutTheme(colors);
			stream << text;
			M_PutTheme(def_t);
		}

		void M_Dye(const std::exception &exc, TerminalTheme colors, std::ostream &stream)
		{
			TerminalTheme def_t = M_GetTheme();
			M_PutTheme(colors);
			stream << exc.what() << std::endl;
			M_PutTheme(def_t);
		}
	}
}

