#ifndef ANY_FOLD_LAZY_HP_25KJ
#define ANY_FOLD_LAZY_HP_25KJ

#include <tuple>
#include <utility>
#include <iterator>
#include <type_traits>

#include "fold_utilities.h"
#include "tuple_slice.h"

namespace fld
{
	namespace detail
	{
		using std::tuple;
		using std::size_t;

		template <class T>
		struct O_x
		{
			T& mem;

			constexpr O_x(T& data) : mem(data) {}

			constexpr decltype(auto) give() { return mem; }
			constexpr decltype(auto) clone() { return mem; }
		};

		template <class T>
		struct O_x<T&&>
		{
			T mem;

			constexpr O_x(T&& data) : mem(std::move(data)) {}

			constexpr decltype(auto) give() { return (mem); }
			constexpr decltype(auto) clone() { return mem; }
		};

		template <class T>
		constexpr auto makeO_x(T&& data)
		{
			return O_x<decltype(data)>(fw(data));
		}

		template <class T>
		struct eval_impl;

		template <class... Ts>
		struct eval_impl<O_x<Ts...>>
		{
			template <class F, class Nodes, class State>
			constexpr inline static decltype(auto)
			apply(F&& fun, Nodes&& nodes, size_t pos, State&& state)
			{
				return vtu::call_with_tuple_element_first(fw(fun), fw(nodes),
				                                          pos, fw(state));
			}

			template <class F, class T0, class T1>
			using state_t = gut::strip_rr_result_t<F, T1, T0>;
		};

		template <class... Ts>
		struct eval_impl<O_x<Ts...>&> 
		{
			template <class F, class Nodes, class State>
			constexpr inline static decltype(auto)
			apply(F&& fun, Nodes&& nodes, size_t pos, State&& state)
			{
				return vtu::call_with_tuple_element_first(fw(fun), fw(nodes),
				                                          pos, fw(state));
			}

			template <class F, class T0, class T1>
			using state_t = gut::strip_rr_result_t<F, T1, T0>;
		};

#if 0
		template <class... Ts>
		struct eval_impl<x_O<Ts...>>
		{
			template <class F, class Nodes, class State>
			constexpr inline static decltype(auto)
			apply(F&& fun, Nodes&& nodes, size_t pos, State&& state)
			{
				return vtu::call_with_tuple_element_last(fw(fun), fw(nodes),
														 pos, fw(state));
			}

			template <class F, class T0, class T1>
			using state_t = gut::strip_rr_result_t<F, T0, T1>;
		};

        template <class... Ts>
		struct eval_impl<x_O<Ts...>&>
		{
			template <class F, class Nodes, class State>
			constexpr inline static decltype(auto)
			apply(F&& fun, Nodes&& nodes, size_t pos, State&& state)
			{
				return vtu::call_with_tuple_element_last(fw(fun), fw(nodes),
														 pos, fw(state));
			}

			template <class F, class T0, class T1>
			using state_t = gut::strip_rr_result_t<F, T0, T1>;
		};
#endif

		template <class Clb, class Expr>
		struct eval
		{
			template <class F, class Nodes, class State>
			constexpr inline static decltype(auto)
			apply(F&& fun, Nodes&& nodes, size_t pos, State&& state)
			{
				return eval_impl<std::tuple_element_t<0, std::decay_t<Nodes>>>::
				    apply(fw(fun), fw(nodes), pos, fw(state));
			}

		private:
			template <size_t I>
			using node_t = std::tuple_element_t<I, Expr>;

			template <size_t I>
			using node_data_t = decltype(std::declval<node_t<I>>().clone());

		public:
			using state_t = typename eval_impl<node_t<0>>::template state_t<
			    Clb, node_data_t<0>, node_data_t<1>>;
		};

		template <class Clb, class T>
		struct eval<Clb, tuple<T>>
		{
			template <class F, class Nodes, class State>
			constexpr inline static decltype(auto) apply(F&&, Nodes&&, size_t,
			                                             State&& state)
			{
				return fw(state);
			}

			using state_t = decltype(std::declval<T>().clone());
		};

		template <class Clb>
		struct eval<Clb, tuple<>>
		{
			template <class F, class Nodes, class State>
			constexpr inline static decltype(auto) apply(F&& fun, Nodes&&,
			                                             size_t, State&&)
			{
				return fw(fun)();
			}

			using state_t = gut::strip_rr_result_t<Clb>;
		};

		template <class... Ts>
		struct O_Om
		{
			tuple<Ts...> nodes;

			template <class... Us>
			constexpr O_Om(Us&&... args)
			    : nodes{fw(args)...}
			{
			}

			template <class StateT, class F>
			class iterator
			{
			public:
				using value_type        = std::decay_t<StateT>;
				using difference_type   = std::ptrdiff_t;
				using reference         = value_type&;
				using pointer           = value_type*;
				using iterator_category = std::input_iterator_tag;

			private:
				reference _state;
				tuple<Ts...>& _nodes;
				std::add_lvalue_reference_t<F> _func;
				size_t _pos;

			public:
				iterator(reference state, tuple<Ts...>& nodes, F& func,
				         size_t pos)
				    : _state(state), _nodes(nodes), _func(func), _pos(pos)
				{
				}

				iterator(const iterator& other)
				    : _state(other._state)
				    , _nodes(other._nodes)
				    , _func(other._func)
				    , _pos(other._pos)
				{
				}

				iterator& operator=(const iterator& other)
				{
					_state = other._state;
					_nodes = other._nodes;
					_func  = other._func;
					_pos   = other._pos;

					return *this;
				}

				bool operator==(const iterator& other) const
				{
					return _pos == other._pos;
				}
				bool operator<(const iterator& other) const
				{
					return _pos < other._pos;
				}

				iterator& operator++()
				{
					if (std::tuple_size<tuple<Ts...>>::value > _pos + 1)
					{
						_state = eval<F, tuple<Ts...>>::apply(
						    _func, gut::tuple_slice<1>(_nodes), _pos, _state);
					}
					++_pos;

					return *this;
				}

				reference operator*() const { return _state; }
				pointer operator->() const { return &_state; }
			};

			template <class T, class F>
			iterator<T, F> begin(T& state, F& func)
			{
				return iterator<T, F>(state, nodes, func, 0);
			}

			template <class T, class F>
			iterator<T, F> end(T& state, F& func)
			{
				return iterator<T, F>(state, nodes, func,
				                      gut::max_z<1, sizeof...(Ts)>);
			}

			template <class T, class F>
			constexpr T& yield(T& state, F& fun)
			{
				for (size_t i(1); i < sizeof...(Ts); ++i)
				{
					state = eval<F, tuple<Ts...>>::apply(
					    fw(fun), gut::tuple_slice<1>(nodes), i - 1, state);
				}

				return state;
			}
		};

		template <class F, class Expr>
		struct reduction
		{
			using redcr_t = gut::remove_rvalue_reference_t<F>;
			using exprs_t = Expr;
			using node_t  = decltype(std::declval<exprs_t>().nodes);
			using state_t = typename eval<F, node_t>::state_t;
			using iterator =
			    typename exprs_t::template iterator<std::decay_t<state_t>,
			                                        std::decay_t<redcr_t>>;

			redcr_t _reducer;
			exprs_t _expr;
			state_t _state;

			template <class FT, class ST>
			reduction(FT&& fun, exprs_t&& expr, ST&& state)
			    : _reducer{fw(fun)}, _expr{std::move(expr)}, _state{fw(state)}
			{
			}

			inline iterator begin()& { return _expr.begin(_state, _reducer); }
			inline iterator end() & { return _expr.end(_state, _reducer); }

			constexpr decltype(auto) yield()
			{
				return _expr.yield(_state, _reducer);
			}
		};

		template <class L, class R>
		constexpr auto operator+(O_x<L>&& lhs, O_x<R>&& acc)
		{
			return O_Om<O_x<R>, O_x<L>>(fw(acc), fw(lhs));
		}

		template <class... Ts, class L, size_t... Is>
		constexpr auto makeO_Om(L&& lhs, O_Om<Ts...>&& yy,
		                        std::index_sequence<Is...>)
		{ // A1
			return O_Om<Ts..., L>(std::move(std::get<Is>(yy.nodes))...,
			                      fw(lhs));
		}

		template <class T, template <class...> class Node>
		constexpr auto makeO_Om(Node<T>&& node)
		{ // A2 - one operant
			return O_Om<Node<T>>(fw(node));
		}

		constexpr auto makeO_Om()
		{ // A3 - zero operants
			return O_Om<>{};
		}

		template <class F, class Expr>
		constexpr auto makeReduction(F&& fun, Expr&& expr)
		{
			return reduction<F, Expr>(fw(fun), fw(expr),
			                          std::get<0>((fw(expr)).nodes).clone());
		}

		template <class F, class Expr, class State>
		constexpr auto makeReduction(F&& fun, Expr&& expr, State&& state)
		{
			return reduction<F, Expr>(fw(fun), fw(expr), fw(state));
		}

		template <class R, class... Ts>
		constexpr auto operator+(O_x<R>&& lhs, O_Om<Ts...>&& acc)
		{
			return makeO_Om(std::move(lhs), std::move(acc),
			                std::index_sequence_for<Ts...>{});
		}

		template <class... Ts, std::enable_if_t<1 < sizeof...(Ts), size_t> = 0>
		constexpr auto _foldr(Ts&&... args)
		{ // 2 or more operants
			return (fw(args) + ...);
		}

		template <class... Ts, std::enable_if_t<1 == sizeof...(Ts), size_t> = 0>
		constexpr auto _foldr(Ts&&... args)
		{ // 1 operant - calls A2
			return makeO_Om(fw(args)...);
		}
	}

	template <class F, class... Ts,
	          // contextual conversion to bool not supported for non type T.P.
	          std::enable_if_t<!!sizeof...(Ts), size_t> = 0>
	constexpr decltype(auto) foldr(F&& fun, Ts&&... args)
	{
		return detail::makeReduction(
		    fw(fun), detail::_foldr(detail::makeO_x(fw(args))...));
	}

	template <class F, class... Ts,
	          std::enable_if_t<!sizeof...(Ts), size_t> = 0>
	constexpr decltype(auto) foldr(F&& fun, Ts&&...)
	{ // zero operants
		return detail::makeReduction(fw(fun), detail::makeO_Om(), fw(fun)());
	}
}


#endif
