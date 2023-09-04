#pragma once

namespace bite
{
	namespace algorithm
	{
		inline constexpr int greatest_common_divisor(int a, int b)
		{
			int tmp{};
			while (b)
			{
				tmp = b;
				b = a % b;
				a = tmp;
			}

//			if (b)
//				return greatest_common_divisor(b, a % b);
			return a;
		}
	}
}
