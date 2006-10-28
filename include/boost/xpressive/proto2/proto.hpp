///////////////////////////////////////////////////////////////////////////////
/// \file proto.hpp
/// The proto2 expression template compiler and supporting utilities.
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO2_HPP_EAN_04_01_2005
#define BOOST_PROTO2_HPP_EAN_04_01_2005

#include <boost/xpressive/proto2/proto_fwd.hpp>
#include <boost/xpressive/proto2/op_tags.hpp>
#include <boost/xpressive/proto2/op_base.hpp>
#include <boost/xpressive/proto2/operators.hpp>
#include <boost/xpressive/proto2/arg_traits.hpp>

namespace boost { namespace proto2
{
    namespace meta
    {
        template<typename Expr, typename State, typename Visitor, typename DomainTag>
        struct compile
        {
            typedef typename meta::as_expr<Expr>::type expr_type;
            typedef compiler<typename tag_type<expr_type>::type, DomainTag> compiler;
            typedef typename compiler::BOOST_NESTED_TEMPLATE apply<expr_type, State, Visitor>::type type;
        };
    }

    namespace op
    {
        struct compile
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor, typename DomainTag>
            struct result<This(Expr, State, Visitor, DomainTag)>
              : meta::compile<
                    typename meta::value_type<Expr>::type
                  , typename meta::value_type<State>::type
                  , typename meta::value_type<Visitor>::type
                  , typename meta::value_type<DomainTag>::type
                >
            {};

            template<typename Expr, typename State, typename Visitor, typename DomainTag>
            typename meta::compile<Expr, State, Visitor, DomainTag>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor, DomainTag) const
            {
                typedef typename meta::as_expr<Expr>::type expr_type;
                typedef compiler<typename tag_type<expr_type>::type, DomainTag> compiler;
                return compiler::call(proto2::as_expr(expr), state, visitor);
            }
        };
    }

    op::compile const compile = {};

}} // namespace boost::proto2

#endif
