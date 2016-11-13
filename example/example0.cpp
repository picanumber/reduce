#include <algorithm>
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

template <class T>
using perfect_capture_t =
    std::conditional_t<std::is_lvalue_reference<T>::value,
                       std::reference_wrapper<std::remove_reference_t<T>>, T>;

struct Comp
{
	template <class F1, class F2>
	auto operator()(F1&& lhs, F2&& rhs)
	{
		return [
			lhs = perfect_capture_t<F1>(std::forward<F1>(lhs)),
			rhs = perfect_capture_t<F2>(std::forward<F2>(rhs))
		](auto&&... args)
		{
			return lhs(rhs(std::forward<decltype(args)>(args)...));
		};
	}
};

template <class... Ts>
auto compose(Ts&&... args)
{
	using fld::Op;
	return (Op<Comp>(std::forward<Ts>(args)) + ...).give();
}

struct Curry
{
	template <class F1, class F2>
	auto operator()(F1&& lhs, F2&& rhs)
	{
		return [
			lhs = perfect_capture_t<F1>(std::forward<F1>(lhs)),
			rhs = perfect_capture_t<F2>(std::forward<F2>(rhs))
		](auto&&... args)
		{
			return lhs(rhs, std::forward<decltype(args)>(args)...);
		};
	}
};

template <class... Ts>
auto curry(Ts&&... args)
{
	using fld::Op;
	return (... + Op<Curry>(std::forward<Ts>(args))).give();
}
// ~ --------------------------------------------------------------------------

int main()
{
	auto f1 = [](auto a) {
		std::cout << "f1\n";
		return 2 * a;
	};
	auto f2 = [](auto a) {
		std::cout << "f2\n";
		return 3 * a;
	};
	auto f3 = [](auto a, auto b) {
		std::cout << "f3\n";
		return b * a;
	};
	auto f4 = [](auto a, auto b, auto c) {
		std::cout << "f4\n";
		return c * b * a;
	};

	std::cout << compose(f1, f2, f3)(2, 3) << std::endl;
	auto cfs = compose(f1, f2, f3);
	std::cout << cfs(2, 3) << std::endl;


	std::cout << curry(f4, 2)(1, 2) << std::endl;
	std::cout << curry(f4, 2, 1)(1) << std::endl;

    auto ccfs = compose(f1, f2, curry(f4, 2, 1)); 
    std::cout << ccfs(1) << std::endl;
}

