#include <array>
#include <tuple>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "../include/boost/reduce.h"
#include <type_traits>


// 1. -------------------------------------------------------------------------
struct Printer
{
	template <class T>
	void operator()(T&& arg)
	{
		std::cout << arg;
	}
};
// 2. -------------------------------------------------------------------------
namespace detail
{
	template <class... Ts>
	constexpr auto sum_(Ts&&... args)
	{
		return (args + ...);
	}

	template <typename T, std::size_t N, std::size_t... Is>
	constexpr T sum_impl(std::array<T, N> const& arr,
	                     std::index_sequence<Is...>)
	{
		return sum_(arr[Is]...);
	}
}

template <typename T, std::size_t N>
constexpr T sum(std::array<T, N> const& arr)
{
	return detail::sum_impl(arr, std::make_index_sequence<N>{});
}
// 3. -------------------------------------------------------------------------
struct Summer
{
	template <class... Ts>
	constexpr auto operator()(Ts&&... args)
	{
		return (args + ...);
	}
};
// 4. -------------------------------------------------------------------------
template <class T>
std::string Stringify(T const& value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

struct Join
{
	template <class T1, class T2>
	std::string operator()(T1 const& lhs, T2 const& rhs)
	{
		return Stringify(lhs) + Stringify(rhs);
	}
};

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

struct Vettore
{
	std::vector<int> operator()(int a, int b) { return {a, b}; }

	std::vector<int> operator()(int b, std::vector<int> const& a)
	{
		auto ret(a);
		ret.insert(ret.begin(), b);
		return ret;
	}
};

struct Max
{
	int use_count = 0;
#if 0
	template <class T1, class T2>
	constexpr auto operator()(T1&& lhs, T2&& rhs)
	{
		return lhs > rhs ? std::forward<T1>(lhs) : std::forward<T2>(rhs);
	}
#else
	template <class T1, class T2>
	constexpr auto operator()(T1 lhs, T2 rhs)
	{
		++use_count;
		return lhs > rhs ? lhs : rhs;
	}

	constexpr auto operator()()
	{
		++use_count;
		return 0;
	}
#endif
};
// ~ --------------------------------------------------------------------------

struct XXP
{
	template <class T>
	void operator()(T&& xx)
	{
		std::cout << xx << std::endl;
	}
};

template <size_t... Is>
void foo(std::index_sequence<Is...>&&)
{
	std::cout << "max of sequence = " << fld::foldr(Max{}, Is...).yield()
	          << std::endl;
}

#define STRESS_TEST 1
#define STRING_TEST 0
#define NUMBRS_TEST 0

int main()
{
	using namespace std::rel_ops;
#if 0
	// 1. A for each lambda ---------------------------------------------------
	auto ForEach = [](auto&& fun, auto&&... args) {
		(fun(args), ...);
	};
	ForEach(Printer{}, 1, " to the ", 2, " to the ", 3, '\n');
	// 2. Sum the contents of an std::array at compile time -------------------
	constexpr std::array<int, 4> arr{ { 1, 1, 2, 3 } };
	std::cout << "Array sum : " << std::integral_constant<int, sum(arr)>{} << std::endl;
	// 3. Sum the contents of a tuple at compile time -------------------------
	constexpr std::tuple<int, int, int, int> tup{ 1, 1, 2, 3 };
	std::cout << "Tuple sum : " << std::integral_constant<int,
		cpp17::apply(Summer{}, tup)>{} << std::endl;
#endif

#if STRESS_TEST
	foo(std::make_index_sequence<16>{});
#endif

#if STRING_TEST
	// 4. Use fold expressions for arbitrary operators ------------------------

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
#endif

#if NUMBRS_TEST
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

	auto xp2 = fld::foldr(JoinAc{}, numa);

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
#endif

#if 0
	auto k = fld::foldl<Vettore>(1, 2, 30, 12);
	for (auto&& i : k) std::cout << i << std::endl;

	//static_assert(20 == fld::foldl<Max>(1, 20, 3, 5), "ET phone home");
	std::cout << "Reducing the maximum of [1, 20, 3, 5] : "
		<< fld::foldl<Max>(1, 20, 3, 5) << std::endl;
#endif // 0
}
