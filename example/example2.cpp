#include <iostream>
#include "../include/boost/reduce.h"

struct Max
{
	template <class T1, class T2>
	constexpr auto operator()(T1&& lhs, T2&& rhs)
	{
		return lhs > rhs ? std::forward<T1>(lhs) : std::forward<T2>(rhs);
	}

	constexpr auto operator()()
	{
		return 0;
	}
};

int main()
{
	using namespace std::rel_ops;

	std::cout << "NUMBER TEST\n===============\n";

	// a. Multiple operants
	std::cout << "\t multiple operants\n";
	int num(0);
	Max mx;

	auto xpr1 = fld::foldr(mx, 2, 5, 7, 5, 7, num);

	std::cout << "Yielded result\n------------\n" << xpr1.yield() << std::endl;

	auto xpr2 = fld::foldr(Max{}, 2, 5, 7, 5, 7);

	std::cout << "Lazy result\n------------\n";
	for (auto&& elem : xpr2)
	{
		std::cout << elem << ", ";
	}
	std::cout << std::endl;

	// b. One operant
	std::cout << "\t one operant\n";
	int numa(99);
	Max mxe;

	auto xp1 = fld::foldr(mxe, 10);

	std::cout << "Yielded result\n------------\n" << xp1.yield() << std::endl;

	auto xp2 = fld::foldr(Max{}, numa);

	std::cout << "Lazy result\n------------\n";
	for (auto&& elem : xp2)
	{
		std::cout << elem << std::endl;
	}

	// c. Zero operants
	std::cout << "\t zero operants\n";
	Max mix;

	auto x1 = fld::foldr(mix);

	std::cout << "Yielded result\n------------\n" << x1.yield() << std::endl;

	auto x2 = fld::foldr(Max{});

	std::cout << "Lazy result\n------------\n";
	for (auto&& elem : x2)
	{
		std::cout << elem << std::endl;
	}
}

