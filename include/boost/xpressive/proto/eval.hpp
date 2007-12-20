///////////////////////////////////////////////////////////////////////////////
/// \file eval.hpp
/// Contains the eval() expression evaluator.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_EVAL_HPP_EAN_03_29_2007
#define BOOST_PROTO_EVAL_HPP_EAN_03_29_2007

#include <boost/type_traits.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/detail/define.hpp>

namespace boost { namespace proto
{

    namespace result_of
    {
        template<typename Expr, typename Context>
        struct eval
        {
            typedef typename Context::template eval<Expr>::result_type type;
        };
    }

    namespace functional
    {
        struct eval
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename Context>
            struct result<This(Expr, Context)>
              : proto::result_of::eval<UNREF(Expr), UNREF(Context)>
            {};

        #ifdef BOOST_HAS_RVALUE_REFS
            template<typename Expr, typename Context>
            typename proto::result_of::eval<Expr, UNREF(Context)>::type
            operator ()(Expr &expr, Context &&context) const
            {
                return UNREF(Context)::template eval<Expr>()(expr, context);
            }
        #else
            template<typename Expr, typename Context>
            typename proto::result_of::eval<Expr, Context>::type
            operator ()(Expr &expr, Context &context) const
            {
                return Context::template eval<Expr>()(expr, context);
            }

            template<typename Expr, typename Context>
            typename proto::result_of::eval<Expr, Context>::type
            operator ()(Expr &expr, Context const &context) const
            {
                return Context::template eval<Expr>()(expr, context);
            }
        #endif
        };
    }

    functional::eval const eval = {};
}}

#include <boost/xpressive/proto/detail/undef.hpp>

#endif
