///////////////////////////////////////////////////////////////////////////////
/// \file branch.hpp
/// A special-purpose proto2 compiler for compiling one branch of the expression
/// tree separately from the rest. Given an expression and a proto2 lambda, it
/// compiles the expression using an initial state determined by the lambda.
/// It then passes the result along with the current state and the visitor
/// to the lambda for further processing.
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO2_COMPILER_BRANCH_HPP_EAN_04_01_2005
#define BOOST_PROTO2_COMPILER_BRANCH_HPP_EAN_04_01_2005

#include <boost/xpressive/proto2/proto_fwd.hpp>

namespace boost { namespace proto2
{

    ///////////////////////////////////////////////////////////////////////////////
    // branch_compiler
    template<typename Lambda, typename DomainTag>
    struct branch_compiler
    {
        template<typename Expr, typename State, typename Visitor>
        struct apply
        {
            typedef proto2::compiler<typename Expr::tag_type, DomainTag> compiler_type;

            // Compile the branch
            typedef typename compiler_type::BOOST_NESTED_TEMPLATE apply
            <
                Expr
              , typename Lambda::state_type
              , Visitor
            >::type branch_type;

            // Pass the branch, state and visitor to the lambda
            typedef typename Lambda::BOOST_NESTED_TEMPLATE apply
            <
                branch_type
              , State
              , Visitor
            >::type type;
        };

        template<typename Expr, typename State, typename Visitor>
        static typename apply<Expr, State, Visitor>::type
        call(Expr const &expr, State const &state, Visitor &visitor)
        {
            return Lambda::call
            (
                proto2::compile(expr, typename Lambda::state_type(), visitor, DomainTag())
              , state
              , visitor
            );
        }
    };

}}

#endif
