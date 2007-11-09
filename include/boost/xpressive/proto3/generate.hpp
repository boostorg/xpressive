///////////////////////////////////////////////////////////////////////////////
/// \file generate.hpp
/// Contains definition of generate\<\> class template, which end users can
/// specialize for generating domain-specific expression wrappers.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO3_GENERATE_HPP_EAN_02_13_2007
#define BOOST_PROTO3_GENERATE_HPP_EAN_02_13_2007

#include <boost/mpl/long.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/xpressive/proto3/proto_fwd.hpp>
#include <boost/xpressive/proto3/matches.hpp>

#define UNREF(X)                                                                \
    typename remove_reference<X>::type

#define UNCVREF(X)                                                              \
    typename remove_cv<UNREF(X)>::type

namespace boost { namespace proto
{

    namespace detail
    {
        template<typename Expr>
        struct arity_;

        template<typename Tag, typename Args, long N>
        struct arity_<expr<Tag, Args, N> >
          : mpl::long_<N>
        {};

        template<typename Expr>
        struct tag_;

        template<typename Tag, typename Args, long N>
        struct tag_<expr<Tag, Args, N> >
        {
            typedef Tag type;
        };

        template<typename Expr>
        struct args_;

        template<typename Tag, typename Args, long N>
        struct args_<expr<Tag, Args, N> >
        {
            typedef Args type;
        };

        struct by_value_cons
        {
            template<typename Sig>
            struct result;

            template<typename This, typename... Args>
            struct result<This(Args...)>
            {
                typedef argsns_::cons<UNCVREF(Args)...> type;
            };

            template<typename... Args>
            argsns_::cons<Args...> operator()(Args const &... args) const
            {
                return argsns_::make_cons(std::move(args)...);
            }
        };

        template<typename Expr>
        struct by_value_generator_;

        template<typename Tag, typename... Args>
        struct by_value_generator_<expr<Tag, args<Args...> > >
        {
            typedef expr<Tag, args<UNCVREF(Args)...> > type;

            static type make(expr<Tag, args<Args...> > const &expr)
            {
                type that = {
                    argsns_::fanout_args(by_value_cons(), expr.proto_base().proto_args_)
                };
                return that;
            }
        };

        template<typename T>
        struct by_value_generator_<expr<tag::terminal, term<T>, 0> >
        {
            typedef expr<tag::terminal, term<UNCVREF(T)> > type;

            static type make(expr<tag::terminal, term<T>, 0> const &expr)
            {
                type that = {{expr.proto_base().proto_args_.car}};
                return that;
            }
        };

    }

    namespace generatorns_
    {
        struct default_generator
        {
            template<typename Expr>
            struct apply
            {
                typedef Expr type;
            };

            template<typename Expr>
            static Expr const &make(Expr const &expr)
            {
                return expr;
            }
        };

        template<template<typename> class Extends>
        struct generator
        {
            template<typename Expr>
            struct apply
            {
                typedef Extends<Expr> type;
            };

            template<typename Expr>
            static Extends<Expr> make(Expr const &expr)
            {
                return Extends<Expr>(expr);
            }
        };

        template<template<typename> class Extends>
        struct pod_generator
        {
            template<typename Expr>
            struct apply
            {
                typedef Extends<Expr> type;
            };

            template<typename Expr>
            static Extends<Expr> make(Expr const &expr)
            {
                Extends<Expr> that = {expr};
                return that;
            }
        };

        template<typename Generator>
        struct by_value_generator
        {
            template<typename Expr>
            struct apply
              : Generator::template apply<
                    typename proto::detail::by_value_generator_<Expr>::type
                >
            {};

            template<typename Expr>
            static typename apply<Expr>::type make(Expr const &expr)
            {
                return Generator::make(proto::detail::by_value_generator_<Expr>::make(expr));
            }
        };

    }

}}

#undef UNREF
#undef UNCVREF

#endif // BOOST_PROTO3_GENERATE_HPP_EAN_02_13_2007
