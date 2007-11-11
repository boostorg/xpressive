///////////////////////////////////////////////////////////////////////////////
/// \file null.hpp
/// Definintion of null_context\<\>, an evaluation context for
/// proto::eval() that simply evaluates each child expression, doesn't
/// combine the results at all, and returns void.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO3_CONTEXT_NULL_HPP_EAN_06_24_2007
#define BOOST_PROTO3_CONTEXT_NULL_HPP_EAN_06_24_2007

#include <boost/xpressive/proto3/proto_fwd.hpp>
#include <boost/xpressive/proto3/eval.hpp>
#include <boost/xpressive/proto3/args.hpp>

namespace boost { namespace proto
{
    namespace context
    {

        /// null_eval
        ///
        template<typename Expr, typename Context, long Arity>
        struct null_eval
        {
            typedef void result_type;
            void operator()(Expr &expr, Context &ctx) const
            {
                this->call_(expr.proto_base().proto_args_, ctx);
            }

        private:
            static void call_(argsns_::cons<> const &, Context &)
            {}

            template<typename Cons>
            static void call_(Cons &args, Context &ctx)
            {
                proto::eval(args.car, ctx);
                call_(args.cdr, ctx);
            }
        };

        template<typename Expr, typename Context>
        struct null_eval<Expr, Context, 0>
        {
            typedef void result_type;
            void operator()(Expr &, Context &) const
            {}
        };

        /// null_context
        ///
        struct null_context
        {
            /// null_context::eval
            ///
            template<typename Expr, typename ThisContext = null_context const>
            struct eval
              : null_eval<Expr, ThisContext>
            {};
        };

    }

}}

#endif
