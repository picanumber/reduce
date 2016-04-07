#include <string>
#include <sstream>
#include <iostream>
#include "../include/boost/reduce.h"


template <class T>
std::string Stringify(T const& value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

struct JoinAc
{
	template <class T1, class T2>
	std::string& operator()(T1 const& lhs, T2& rhs)
	{
		return (rhs += Stringify(lhs));
	}

	template <class T1, class T2>
	std::string operator()(T1 const& lhs, T2&& rhs)
	{
		return (rhs + Stringify(lhs));
	}

	std::string operator()() { return std::string("identity element"); }
};

int main()
{
	using namespace std::rel_ops;

	std::cout << "\nSTRING TEST\n===============\n";

	// a. Multiple operants
	std::cout << "\t multiple operants\n";
	std::string acuml;
	JoinAc joiner;

	auto expr1 =
	    fld::foldr(joiner, std::string(" < in the bush >"), 10,
	               std::string(" < bird in the hand, is worth > "), 1, acuml);

	std::cout << "Yielded result\n------------\n" << expr1.yield() << std::endl;

	auto expr2 = fld::foldr(JoinAc{}, std::string(" < in the bush >"), 10,
	                        std::string(" < bird in the hand, is worth > "), 1,
	                        std::string{});

	std::cout << "Lazy result\n------------\n";
	for (auto&& elem : expr2)
	{
		std::cout << elem << std::endl;
	}

	// b. One operant
	std::cout << "\t one operant\n";
	std::string acum("default string acum");
	JoinAc joine;

	auto exp1 = fld::foldr(joine, 10);

	std::cout << "Yielded result\n------------\n" << exp1.yield() << std::endl;

	auto exp2 = fld::foldr(JoinAc{}, acum);

	std::cout << "Lazy result\n------------\n";
	for (auto&& elem : exp2)
	{
		std::cout << elem << std::endl;
	}

	// c. Zero operants
	std::cout << "\t zero operants\n";
	JoinAc join;

	auto ex1 = fld::foldr(join);

	std::cout << "Yielded result\n------------\n" << ex1.yield() << std::endl;

	auto ex2 = fld::foldr(JoinAc{});

	std::cout << "Lazy result\n------------\n";
	for (auto&& elem : ex2)
	{
		std::cout << elem << std::endl;
	}
	std::cout << std::endl;
}

