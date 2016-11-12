#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>


namespace cpp17
{

	namespace detail
	{
		// TODO: Write __invoke
	}

#if 0
	template <class F, class... Args>
	decltype(auto) invoke(F&& f, Args&&... args)
	{
		return detail::__invoke(std::forward<F>(f), std::forward<Args>(args)...);
	}
#endif // TODO: Activate the invoke interface

	namespace detail
	{
		template <class F, class Tuple, std::size_t... I>
		constexpr decltype(auto) apply_impl(F&& f, Tuple&& t,
		                                    std::index_sequence<I...>)
		{
#if 1
			return (std::forward<F>(f))(std::get<I>(std::forward<Tuple>(t))...);
#else
			return invoke(std::forward<F>(f),
			              std::get<I>(std::forward<Tuple>(t))...);
#endif // TODO: Implement the second branch
		}
	}

	template <class T>
	struct Valency
	{
		static constexpr std::size_t value = std::tuple_size<T>::value;
	};

	template <class T, std::size_t I>
	struct Valency<std::array<T, I>>
	{
		static constexpr std::size_t value = I;
	};

	template <class F, class C>
	constexpr decltype(auto) apply(F&& f, C&& t)
	{
		return detail::apply_impl(
		    std::forward<F>(f), std::forward<C>(t),
		    std::make_index_sequence<Valency<std::decay_t<C>>::value>{});
	}
}

namespace fld
{

	template <class F, class T>
	struct XX
	{
		T data;

		template <class D>
		constexpr XX(D&& data) : data(std::forward<D>(data))
		{
		}

		constexpr T give() const { return data; }
	};

	template <class L, class F, class R>
	struct YY
	{
		L l;
		R r;

		template <class LL, class RR>
		constexpr YY(LL&& l, RR&& r)
		    : l(std::forward<LL>(l)), r(std::forward<RR>(r))
		{
		}

		constexpr auto give() const { return F{}(l.give(), r.give()); }
	};

	template <class F, class R, class T>
	constexpr auto operator+(XX<F, T> const& lhs, R const& rhs)
	{
		return YY<XX<F, T>, F, R>(lhs, rhs);
	}

	template <class F, class T1, class T2, class R>
	constexpr auto operator+(YY<T1, F, T2> const& lhs, R const& rhs)
	{
		return YY<YY<T1, F, T2>, F, R>(lhs, rhs);
	}

	template <class F, class T>
	XX<F, T> Op(T const& val)
	{
		return XX<F, T>(val);
	}

	namespace detail
	{
		template <class... Ts>
		constexpr auto _foldl(Ts&&... args)
		{
			return (args + ...);
		}

		template <class... Ts>
		constexpr auto _foldr(Ts&&... args)
		{
			return (... + args);
		}
	}

	template <class F, class... Ts>
	constexpr decltype(auto) foldl(Ts&&... args)
	{
		return detail::_foldl(XX<F, Ts>(args)...).give();
	}

	template <class F, class... Ts>
	constexpr decltype(auto) foldr(Ts&&... args)
	{
		return detail::_foldr(XX<F, Ts>(args)...).give();
	}
}

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
	template <class T1, class T2>
	constexpr decltype(auto) operator()(T1&& lhs, T2&& rhs)
	{
		return lhs > rhs ? std::forward<T1>(lhs) : std::forward<T2>(rhs);
	}
};

struct comp
{
	template <class F1, class F2>
	auto operator()(F1 lhs, F2 rhs)
	{
		return [=](auto a) { return lhs(rhs(a)); };
	}
};

template <class... Ts>
auto compose(Ts&&... args)
{
	using fld::Op;
	return (Op<comp>(args) + ...).give();
}
// ~ --------------------------------------------------------------------------

int main()
{
	auto by2 = [](auto a) {
		std::cout << "by2\n";
		return 2 * a;
	};
	auto by3 = [](auto a) {
		std::cout << "by3\n";
		return 3 * a;
	};
	auto by4 = [](auto a) {
		std::cout << "by4\n";
		return 3 * a;
	};

	std::cout << compose(by2, by3, by4)(2) << std::endl;
}

