///////////////////////////////////////////////////////////////////////////////
/// \file deep_copy.hpp
/// Replace all nodes stored by reference by nodes stored by value.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_DEEP_COPY_HPP_EAN_11_21_2006
#define BOOST_PROTO_DEEP_COPY_HPP_EAN_11_21_2006

#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/args.hpp>
#include <boost/xpressive/proto/expr.hpp>
#include <boost/xpressive/proto/traits.hpp>
#include <boost/xpressive/proto/generate.hpp>
#include <boost/xpressive/proto/detail/deep_copy.hpp>
#include <boost/xpressive/proto/detail/define.hpp>

namespace boost { namespace proto
{

    namespace result_of
    {
        template<typename Expr>
        struct deep_copy
        {
            typedef typename proto::detail::deep_copy_<typename Expr::proto_args>::type args_type;
            typedef expr<typename Expr::proto_tag, args_type> expr_type;
            typedef typename Expr::proto_domain::template apply<expr_type>::type type;

            static type call(Expr const &expr)
            {
                expr_type that = {proto::detail::deep_copy_<typename Expr::proto_args>::call(expr.proto_base().proto_args_)};
                return Expr::proto_domain::make(that);
            }
        };
    }

    namespace functional
    {
        struct deep_copy
        {
            BOOST_PROTO_CALLABLE()

            template<typename Sig>
            struct result;

            template<typename This, typename Expr>
            struct result<This(Expr)>
              : result_of::deep_copy<UNCVREF(Expr)>
            {};

            template<typename Expr>
            typename result_of::deep_copy<Expr>::type const
            operator()(Expr const &expr) const
            {
                return result_of::deep_copy<Expr>::call(expr);
            }
        };
    }

    functional::deep_copy const deep_copy = {};

}}

#include <boost/xpressive/proto/detail/undef.hpp>

#endif // BOOST_PROTO_DEEP_COPY_HPP_EAN_11_21_2006
