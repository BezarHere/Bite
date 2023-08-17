#pragma once

#define invalidcall throw std::runtime_error(std::string("Invalid call to un implemented function ") + __FUNCTION__ + "@" + __LINE__ + '.')

#define halt(msg) throw std::runtime_error(std::string(msg) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")")
//#define halt(msg) std::cout << std::endl << (std::string(msg) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")" + "\n[ press enter to abort ]") << std::endl; std::cin.get(); abort()
#define show(x) bite::dye::M_PutTheme({ bite::dye::ColorCode::LightYellow, bite::dye::ColorCode::Black }); std::cout << #x << ": " << (x) << std::endl; bite::dye::M_PutTheme({ bite::dye::ColorCode::White, bite::dye::ColorCode::Black })
