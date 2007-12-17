///////////////////////////////////////////////////////////////////////////////
/// \file generate.hpp
/// Contains definition of generate\<\> class template, which end users can
/// specialize for generating domain-specific expression wrappers.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_GENERATE_HPP_EAN_02_13_2007
#define BOOST_PROTO_GENERATE_HPP_EAN_02_13_2007

#include <boost/mpl/long.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/matches.hpp>
#include <boost/xpressive/proto/detail/by_value_generator.hpp>
#include <boost/xpressive/proto/detail/define.hpp>

namespace boost { namespace proto
{

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
            {
                typedef typename proto::detail::by_value_generator_<typename Expr::proto_args>::type args_type;
                typedef expr<typename Expr::proto_tag, args_type> expr_type;
                typedef typename Generator::template apply<expr_type>::type type;
            };

            template<typename Expr>
            static typename apply<Expr>::type make(Expr const &expr)
            {
                typename apply<Expr>::expr_type that = {
                    proto::detail::by_value_generator_<typename Expr::proto_args>::call(expr.proto_base().proto_args_)
                };
                return Generator::make(that);
            }
        };

    }

}}

#include <boost/xpressive/proto/detail/undef.hpp>

#endif // BOOST_PROTO_GENERATE_HPP_EAN_02_13_2007
