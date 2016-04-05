#ifndef VISIT_TUPLE_2531324_H
#define VISIT_TUPLE_2531324_H


#include <tuple>
#include <type_traits>
#include <functional>


#define fw(...) ::std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)


namespace vtu
{
	namespace detail
	{
		using std::size_t;
		template <size_t I>
		struct visit_impl // TODO: Elaborate on how this is an "unrolled" switch
		{
			template <class T, class F, class... Args>
			static constexpr decltype(auto) visit1(T&& tup, size_t idx, F&& fun,
			                                       Args&&... args)
			{ // TODO: elaborate on why we forward before getting (member access
				// rules)
				return idx == I
				           ? fw(fun)(std::get<I>(fw(tup)).give(), fw(args)...)
				           : visit_impl<I - 1>::visit1(fw(tup), idx, fw(fun),
				                                       fw(args)...);
			}

			template <class T, class F, class... Args>
			static constexpr decltype(auto) visit2(T&& tup, size_t idx, F&& fun,
			                                       Args&&... args)
			{
				return idx == I
				           ? fw(fun)(fw(args)..., std::get<I>(fw(tup)).give())
				           : visit_impl<I - 1>::visit2(fw(tup), idx, fw(fun),
				                                       fw(args)...);
			}
		};

		template <>
		struct visit_impl<0>
		{
			template <class T, class F, class... Args>
			static constexpr decltype(auto) visit1(T&& tup, size_t, F&& fun,
			                                       Args&&... args)
			{ // TODO: elaborate on std::invoke not beign constexpr
				return fw(fun)(std::get<0>(fw(tup)).give(), fw(args)...);
			}

			template <class T, class F, class... Args>
			static constexpr decltype(auto) visit2(T&& tup, size_t, F&& fun,
			                                       Args&&... args)
			{
				return fw(fun)(fw(args)..., std::get<0>(fw(tup)).give());
			}
		};
	}
	// ~ detail

	template <class F, class Tuple, class... Args>
	constexpr decltype(auto) call_with_tuple_element_first(F&& fun, Tuple&& tup,
	                                                       size_t idx,
	                                                       Args&&... args)
	{
		return detail::visit_impl<std::tuple_size<std::decay_t<Tuple>>::value -
		                          1>::visit1(fw(tup), idx, fw(fun),
		                                     fw(args)...);
	}

	template <class F, class Tuple, class... Args>
	constexpr decltype(auto) call_with_tuple_element_last(F&& fun, Tuple&& tup,
	                                                      size_t idx,
	                                                      Args&&... args)
	{
		return detail::visit_impl<std::tuple_size<std::decay_t<Tuple>>::value -
		                          1>::visit2(fw(tup), idx, fw(fun),
		                                     fw(args)...);
	}
}
// ~ vtu

namespace gut
{
	template <class T>
	struct remove_rvalue_reference
	{
		typedef T type;
	};

	template <class T>
	struct remove_rvalue_reference<T&&>
	{
		typedef T type;
	};

	template <class T>
	using remove_rvalue_reference_t = typename remove_rvalue_reference<T>::type;

	template <class F, class... Ts>
	using strip_rr_result_t =
	    remove_rvalue_reference_t<std::result_of_t<F(Ts...)>>;
}
// ~ gut


#endif

