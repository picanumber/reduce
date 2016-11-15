#include "../include/boost/reduce.h"
#include <array>
#include <iostream>
#include <list>
#include <vector>

int main()
{
	std::list<int> l1{10, 20, 30};
	std::vector<int> v1{1, 2, 3};
	std::array<int, 3> a1{{100, 200, 300}};

	auto output_vf = [](auto input, auto out) {
		for (auto&& elem : input)
			*out++ = elem;
		return out;
	};

    std::vector<int> vp; 
	fld::foldr(output_vf, a1, l1, v1, std::back_inserter(vp)).yield();

	using namespace std::rel_ops;
	for (auto&& elem : vp)
		std::cout << elem << std::endl;
}

