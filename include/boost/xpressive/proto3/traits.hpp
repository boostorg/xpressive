///////////////////////////////////////////////////////////////////////////////
/// \file traits.hpp
/// Definition of the (meta-)functions for querying for expression tree properties.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO3_TRAITS_HPP_EAN_10_28_2007
#define BOOST_PROTO3_TRAITS_HPP_EAN_10_28_2007

#include <boost/type_traits.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/xpressive/proto3/proto_fwd.hpp>

#define CV(T)\
    typename add_const<T>::type

#define REF(T)\
    typename add_reference<T>::type

#define CVREF(T)\
    REF(CV(T))

#define UNREF(T)\
    typename remove_reference<T>::type

#define UNCVREF(T)\
    typename remove_cv<UNREF(T)>::type

namespace boost { namespace proto
{

    namespace op
    {

        namespace detail
        {

            template<typename Args, int N, typename Result = args<>, bool Done = (N == 0) >
            struct resize;

            template<typename Head, typename... Tail, int N, typename... Result>
            struct resize<args<Head, Tail...>, N, args<Result...>, false>
              : resize<args<Tail...>, N-1, args<Result..., Head> >
            {};

            template<typename... Args, typename... Result>
            struct resize<args<Args...>, 0, args<Result...>, true>
            {
                typedef args<Result...> type;
            };

            template<int... I>
            struct indices
            {};

            template<int N, int... I>
            struct make_indices<N, indices<I...> >
              : make_indices<N-1, indices<N-1, I...> >
            {};

            template<int... I>
            struct make_indices<0, indices<I...> >
            {
                typedef indices<I...> type;
            };

            template<typename G, typename E, typename S, typename V
                , long GN = G::size, long EN = E::size, bool B = (GN > EN)>
            struct apply_args;

            template<typename... G, typename... E, typename S, typename V, long GN, long EN>
            struct apply_args<args<G...>, args<E...>, S, V, GN, EN, false>
              : apply_args<args<G..., typename back<args<G...> >::type>, args<E...>, S, V>
            {};

            template<typename... G, typename... E, typename S, typename V, long GN, long EN>
            struct apply_args<args<G...>, args<E...>, S, V, GN, EN, true>
              : apply_args<typename resize<args<G...>, EN>::type, args<E...>, S, V>
            {};

            template<typename... G, typename... E, typename S, typename V, long N>
            struct apply_args<args<G...>, args<E...>, S, V, N, N, false>
            {
                typedef args<
                    typename G::template apply<UNCVREF(E), S, V>::type...
                > type;

                static typename type::cons_type
                call(argsns_::cons<E...> const &a, S const &s, V &v)
                {
                    return apply_args::call_(a, s, v, typename make_indices<sizeof...(E)>::type());
                }

                template<int... I>
                static typename type::cons_type
                call_(argsns_::cons<E...> const &a, S const &s, V &v, indices<I...>)
                {
                    using result_of::detail::arg_c;
                    typename type::cons_type that = {
                        G::call(arg_c<argsns_::cons<E...>, I>::call(a), s, v)...
                    };
                    return that;
                }
            };
        }

        template<typename T>
        struct terminal
        {
            typedef expr<tag::terminal, term<T> > type;
            typedef type proto_base_expr;

            template<typename Expr, typename, typename>
            struct apply
            {
                typedef Expr type;
            };

            template<typename Expr, typename State, typename Visitor>
            static Expr const &
            call(Expr const &expr, State const &, Visitor &)
            {
                return expr;
            }
        };

        template<typename Tag, typename T>
        struct unary_expr
        {
            typedef expr<Tag, args<T> > type;
            typedef type proto_base_expr;

            template<typename Expr, typename State, typename Visitor>
            struct apply
            {
                typedef expr<typename Expr::proto_tag, args<
                    typename T::template apply<
                        typename result_of::arg_c<Expr, 0>::type, State, Visitor
                    >::type
                > > type;
            };

            template<typename Expr, typename State, typename Visitor>
            static typename apply<Expr, State, Visitor>::type
            call(Expr const &expr, State const &state, Visitor &visitor)
            {
                typename apply<Expr, State, Visitor>::type that = {
                    T::call(proto::arg_c<0>(expr), state, visitor)
                };
                return that;
            }
        };

        template<typename Tag, typename T, typename U>
        struct binary_expr
        {
            typedef expr<Tag, args<T, U> > type;
            typedef type proto_base_expr;

            template<typename Expr, typename State, typename Visitor>
            struct apply
            {
                typedef expr<typename Expr::proto_tag, args<
                    typename T::template apply<typename result_of::arg_c<Expr, 0>::type, State, Visitor>::type
                  , typename U::template apply<typename result_of::arg_c<Expr, 1>::type, State, Visitor>::type
                > > type;
            };

            template<typename Expr, typename State, typename Visitor>
            static typename apply<Expr, State, Visitor>::type
            call(Expr const &expr, State const &state, Visitor &visitor)
            {
                typename apply<Expr, State, Visitor>::type that = {
                    T::call(proto::arg_c<0>(expr), state, visitor)
                  , U::call(proto::arg_c<1>(expr), state, visitor)
                };
                return that;
            }
        };

        template<typename T> struct posit : unary_expr<tag::posit, T> {};
        template<typename T> struct negate : unary_expr<tag::negate, T> {};
        template<typename T> struct dereference : unary_expr<tag::dereference, T> {};
        template<typename T> struct complement : unary_expr<tag::complement, T> {};
        template<typename T> struct address_of : unary_expr<tag::address_of, T> {};
        template<typename T> struct logical_not : unary_expr<tag::logical_not, T> {};
        template<typename T> struct pre_inc : unary_expr<tag::pre_inc, T> {};
        template<typename T> struct pre_dec : unary_expr<tag::pre_dec, T> {};
        template<typename T> struct post_inc : unary_expr<tag::post_inc, T> {};
        template<typename T> struct post_dec : unary_expr<tag::post_dec, T> {};

        template<typename T, typename U> struct shift_left : binary_expr<tag::shift_left, T, U> {};
        template<typename T, typename U> struct shift_right : binary_expr<tag::shift_right, T, U> {};
        template<typename T, typename U> struct multiplies : binary_expr<tag::multiplies, T, U> {};
        template<typename T, typename U> struct divides : binary_expr<tag::divides, T, U> {};
        template<typename T, typename U> struct modulus : binary_expr<tag::modulus, T, U> {};
        template<typename T, typename U> struct plus : binary_expr<tag::plus, T, U> {};
        template<typename T, typename U> struct minus : binary_expr<tag::minus, T, U> {};
        template<typename T, typename U> struct less : binary_expr<tag::less, T, U> {};
        template<typename T, typename U> struct greater : binary_expr<tag::greater, T, U> {};
        template<typename T, typename U> struct less_equal : binary_expr<tag::less_equal, T, U> {};
        template<typename T, typename U> struct greater_equal : binary_expr<tag::greater_equal, T, U> {};
        template<typename T, typename U> struct equal_to : binary_expr<tag::equal_to, T, U> {};
        template<typename T, typename U> struct not_equal_to : binary_expr<tag::not_equal_to, T, U> {};
        template<typename T, typename U> struct logical_or : binary_expr<tag::logical_or, T, U> {};
        template<typename T, typename U> struct logical_and : binary_expr<tag::logical_and, T, U> {};
        template<typename T, typename U> struct bitwise_and : binary_expr<tag::bitwise_and, T, U> {};
        template<typename T, typename U> struct bitwise_or : binary_expr<tag::bitwise_or, T, U> {};
        template<typename T, typename U> struct bitwise_xor : binary_expr<tag::bitwise_xor, T, U> {};
        template<typename T, typename U> struct comma : binary_expr<tag::comma, T, U> {};
        template<typename T, typename U> struct mem_ptr : binary_expr<tag::mem_ptr, T, U> {};

        template<typename T, typename U> struct assign : binary_expr<tag::assign, T, U> {};
        template<typename T, typename U> struct shift_left_assign : binary_expr<tag::shift_left_assign, T, U> {};
        template<typename T, typename U> struct shift_right_assign : binary_expr<tag::shift_right_assign, T, U> {};
        template<typename T, typename U> struct multiplies_assign : binary_expr<tag::multiplies_assign, T, U> {};
        template<typename T, typename U> struct divides_assign : binary_expr<tag::divides_assign, T, U> {};
        template<typename T, typename U> struct modulus_assign : binary_expr<tag::modulus_assign, T, U> {};
        template<typename T, typename U> struct plus_assign : binary_expr<tag::plus_assign, T, U> {};
        template<typename T, typename U> struct minus_assign : binary_expr<tag::minus_assign, T, U> {};
        template<typename T, typename U> struct bitwise_and_assign : binary_expr<tag::bitwise_and_assign, T, U> {};
        template<typename T, typename U> struct bitwise_or_assign : binary_expr<tag::bitwise_or_assign, T, U> {};
        template<typename T, typename U> struct bitwise_xor_assign : binary_expr<tag::bitwise_xor_assign, T, U> {};
        template<typename T, typename U> struct subscript : binary_expr<tag::subscript, T, U> {};

        template<typename Tag, typename... Args>
        struct nary_expr
        {
            typedef expr<Tag, args<Args...> > type;
            typedef type proto_base_expr;

            template<typename Expr, typename State, typename Visitor>
            struct apply
            {
                typedef detail::apply_args<args<Args...>, typename Expr::proto_args, State, Visitor> apply_;
                typedef expr<typename Expr::proto_tag, typename apply_::type> type;
            };

            template<typename Expr, typename State, typename Visitor>
            static typename apply<Expr, State, Visitor>::type
            call(Expr const &expr, State const &state, Visitor &visitor)
            {
                typename apply<Expr, State, Visitor>::type that = 
                    {apply<Expr, State, Visitor>::apply_::call(expr.proto_args_, state, visitor)};
                return that;
            }
        };

        template<typename... Args>
        struct function
          : nary_expr<tag::function, Args...>
        {};

    }

    namespace result_of
    {

        namespace detail
        {
            // TODO unroll this loop with a few extra specializations
            template<typename Head, typename... Tail, long N>
            struct arg_c<argsns_::cons<Head, Tail...>, N>
            {
                typedef arg_c<argsns_::cons<Tail...>, N-1> base_type;
                typedef typename base_type::type type;
                typedef typename base_type::reference reference;
                typedef typename base_type::const_reference const_reference;

                static reference call(argsns_::cons<Head, Tail...> &args)
                {
                    return base_type::call(args.cdr);
                }

                static const_reference call(argsns_::cons<Head, Tail...> const &args)
                {
                    return base_type::call(args.cdr);
                }
            };

            template<typename Head, typename... Tail>
            struct arg_c<argsns_::cons<Head, Tail...>, 0>
            {
                typedef UNCVREF(Head) type;
                typedef REF(Head) reference;
                typedef CVREF(Head) const_reference;

                static reference call(argsns_::cons<Head, Tail...> &args)
                {
                    return args.car;
                }

                static const_reference call(argsns_::cons<Head, Tail...> const &args)
                {
                    return args.car;
                }
            };

        } // namespace detail

        template<typename Expr, long N>
        struct arg_c
            : detail::arg_c<typename Expr::proto_args::cons_type, N>
        {};

        template<typename Expr>
        struct arg
          : detail::arg_c<typename Expr::proto_args::cons_type, 0>
        {};

        template<typename Expr>
        struct left
          : detail::arg_c<typename Expr::proto_args::cons_type, 0>
        {};

        template<typename Expr>
        struct right
          : detail::arg_c<typename Expr::proto_args::cons_type, 1>
        {};

        // as_expr
        template<typename T, typename Domain, typename EnableIf>
        struct as_expr
        {
            typedef typename mpl::if_<
                mpl::or_<is_array<UNCVREF(T)>, is_function<UNCVREF(T)> >
              , REF(T)
              , UNCVREF(T)
            >::type value_type;

            typedef expr<tag::terminal, term<value_type> > expr_type;
            typedef typename Domain::template apply<expr_type>::type type;
            typedef type const result_type;

            template<typename T2>
            static result_type call(T2 &&t)
            {
                return Domain::make(expr_type::make(t));
            }
        };

        template<typename T, typename Domain>
        struct as_expr<T, Domain, typename T::proto_is_expr_>
        {
            typedef typename T::proto_derived_expr type;
            typedef T &result_type;

            template<typename T2>
            static result_type call(T2 &&t)
            {
                return t;
            }
        };

        template<typename T, typename Domain, typename EnableIf>
        struct as_arg
        {
            typedef expr<tag::terminal, term<CVREF(T) > > expr_type;
            typedef typename Domain::template apply<expr_type>::type type;
            static type call(T &&t)
            {
                return Domain::make(expr_type::make(t));
            }
        };

        template<typename T, typename Domain>
        struct as_arg<T, Domain, typename T::proto_is_expr_>
        {
            typedef T const &type;
            static T const &call(T const &t)
            {
                return t;
            }
        };

        template<typename T, typename Domain>
        struct as_arg<T &, Domain, typename T::proto_is_expr_>
        {
            typedef T &type;
            static T &call(T &t)
            {
                return t;
            }
        };

        template<typename T, typename _>
        struct is_expr
          : mpl::false_
        {};

        template<typename T>
        struct is_expr<T, typename T::proto_is_expr_>
          : mpl::true_
        {};

        template<typename T>
        struct is_expr<T &, typename T::proto_is_expr_>
          : mpl::true_
        {};

    }

    namespace functional
    {

        // TODOD

    }

    template<typename Expr>
    typename result_of::arg<Expr>::reference arg(Expr &expr)
    {
        return result_of::arg<Expr>::call(expr.proto_args_);
    }

    template<typename Expr>
    typename result_of::arg<Expr>::const_reference arg(Expr const &expr)
    {
        return result_of::arg<Expr>::call(expr.proto_args_);
    }

    template<typename Expr>
    typename result_of::left<Expr>::reference left(Expr &expr)
    {
        return result_of::left<Expr>::call(expr.proto_args_);
    }

    template<typename Expr>
    typename result_of::left<Expr>::const_reference left(Expr const &expr)
    {
        return result_of::left<Expr>::call(expr.proto_args_);
    }

    template<typename Expr>
    typename result_of::right<Expr>::reference right(Expr &expr)
    {
        return result_of::right<Expr>::call(expr.proto_args_);
    }

    template<typename Expr>
    typename result_of::right<Expr>::const_reference right(Expr const &expr)
    {
        return result_of::right<Expr>::call(expr.proto_args_);
    }

    template<long N, typename Expr>
    typename result_of::arg_c<Expr, N>::reference arg_c(Expr &expr)
    {
        return result_of::arg_c<Expr, N>::call(expr.proto_args_);
    }

    template<long N, typename Expr>
    typename result_of::arg_c<Expr, N>::const_reference arg_c(Expr const &expr)
    {
        return result_of::arg_c<Expr, N>::call(expr.proto_args_);
    }

    template<typename T>
    typename result_of::as_expr<T>::result_type as_expr(T &&t)
    {
        return result_of::as_expr<T>::call(t);
    }

    template<typename Domain, typename T>
    typename result_of::as_expr<T, Domain>::result_type as_expr(T &&t)
    {
        return result_of::as_expr<T, Domain>::call(t);
    }

    template<typename T>
    typename result_of::as_arg<T>::type as_arg(T &&t)
    {
        return result_of::as_arg<T>::call(std::forward<T>(t));
    }

    template<typename Domain, typename T>
    typename result_of::as_arg<T, Domain>::type as_arg(T &&t)
    {
        return result_of::as_arg<T, Domain>::call(t);
    }

}}

#undef CV
#undef REF
#undef CVREF
#undef UNREF
#undef UNCVREF

#endif
