#include <tuple>
#include <type_traits>


namespace gut
{
	namespace detail
	{
		template <std::size_t I, class Tuple>
		constexpr std::size_t slice_size =
		    std::tuple_size<Tuple>::value > I
		        ? std::tuple_size<Tuple>::value - I
		        : 0;

		template <std::size_t From, class Tuple, std::size_t... I>
		auto tuple_slice_impl(Tuple&& tup, std::index_sequence<I...>&&)
		{
			return std::forward_as_tuple(
			    std::get<I + From>(std::forward<Tuple>(tup))...);
		}
	}

	template <std::size_t From, class Tuple>
	auto tuple_slice(Tuple&& tup)
	{
		return detail::tuple_slice_impl<From>(
		    std::forward<Tuple>(tup),
		    std::make_index_sequence<
		        detail::slice_size<From, std::decay_t<Tuple>>>{});
	}
} // ~gut

