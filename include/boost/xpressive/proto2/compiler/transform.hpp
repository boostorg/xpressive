///////////////////////////////////////////////////////////////////////////////
/// \file transform.hpp
/// A special-purpose proto2 compiler for applying a transformation to an
/// expression. The transformation is a proto2 lambda. The result of the
/// transformation is forwarded to the specified compiler, or to the
/// default compiler for the resulting expression is no compiler is
/// specified. Also included are some basic transforms, such as one that
/// extracts the operand of a unary node, the left and right operands of
/// a binary node, and a way to compose multiple transforms into one.
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO2_COMPILER_TRANSFORM_HPP_EAN_04_01_2005
#define BOOST_PROTO2_COMPILER_TRANSFORM_HPP_EAN_04_01_2005

#include <boost/xpressive/proto2/proto_fwd.hpp>
#include <boost/xpressive/proto2/arg_traits.hpp>

namespace boost { namespace proto2
{

    ///////////////////////////////////////////////////////////////////////////////
    // transform_compiler
    //   accepts a transformation as a lambda, applies the transformation to any
    //   parse tree passed in, and then compiles the result using the specified
    //   compiler
    template<typename Lambda, typename DomainTag, typename Compiler>
    struct transform_compiler
    {
        template<typename Node, typename State, typename Visitor>
        struct apply
        {
            typedef typename Compiler::BOOST_NESTED_TEMPLATE apply
            <
                typename Lambda::BOOST_NESTED_TEMPLATE apply<Node, State, Visitor>::type
              , State
              , Visitor
            >::type type;
        };

        template<typename Node, typename State, typename Visitor>
        static typename apply<Node, State, Visitor>::type
        call(Node const &node, State const &state, Visitor &visitor)
        {
            return Compiler::call(Lambda::call(node, state, visitor), state, visitor);
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    // if no compiler is specified, the transform_compiler forwards to the default
    // compiler
    template<typename Lambda, typename DomainTag>
    struct transform_compiler<Lambda, DomainTag, void>
    {
        template<typename Node, typename State, typename Visitor>
        struct apply
        {
            typedef typename Lambda::BOOST_NESTED_TEMPLATE apply
            <
                Node
              , State
              , Visitor
            >::type trans_type;

            typedef proto2::compiler<typename trans_type::tag_type, DomainTag> compiler_type;

            typedef typename compiler_type::BOOST_NESTED_TEMPLATE apply
            <
                trans_type
              , State
              , Visitor
            >::type type;
        };

        template<typename Node, typename State, typename Visitor>
        static typename apply<Node, State, Visitor>::type
        call(Node const &node, State const &state, Visitor &visitor)
        {
            //typename apply<Node, State, Visitor>::trans_type trans = Lambda::call(node, state, visitor);
            //return proto2::compile(trans, state, visitor, DomainTag());
            return proto2::compile(Lambda::call(node, state, visitor), state, visitor, DomainTag());
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    // identity_transform
    //   pass through without doing a transform
    struct identity_transform
    {
        template<typename Node, typename, typename>
        struct apply
        {
            typedef Node type;
        };

        template<typename Node, typename State, typename Visitor>
        static Node const &call(Node const &node, State const &, Visitor &)
        {
            return node;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    // arg_transform
    struct arg_transform
    {
        template<typename Node, typename, typename>
        struct apply
        {
            typedef typename Node::arg0_type type;
        };

        template<typename Node, typename State, typename Visitor>
        static typename apply<Node, State, Visitor>::type
        call(Node const &node, State const &, Visitor &)
        {
            return proto2::arg(node);
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    // left_transform
    struct left_transform
    {
        template<typename Node, typename, typename>
        struct apply
        {
            typedef typename Node::arg0_type type;
        };

        template<typename Node, typename State, typename Visitor>
        static typename apply<Node, State, Visitor>::type
        call(Node const &node, State const &, Visitor &)
        {
            return proto2::left(node);
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    // right_transform
    struct right_transform
    {
        template<typename Node, typename, typename>
        struct apply
        {
            typedef typename Node::arg1_type type;
        };

        template<typename Node, typename State, typename Visitor>
        static typename apply<Node, State, Visitor>::type
        call(Node const &node, State const &, Visitor &)
        {
            return proto2::right(node);
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    // unary_op_transform
    //   insert a unary operation
    template<typename Tag>
    struct unary_op_transform
    {
        template<typename Node, typename, typename>
        struct apply
        {
            typedef typename unary_op<Tag, Node>::type type;
        };

        template<typename Node, typename State, typename Visitor>
        static typename unary_op<Tag, Node>::type
        call(Node const &node, State const &, Visitor &)
        {
            typename unary_op<Tag, Node>::type that = {node};
            return that;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    // compose_transforms
    //   execute two transforms in succession
    template<typename First, typename Second>
    struct compose_transforms
    {
        template<typename Node, typename State, typename Visitor>
        struct apply
        {
            typedef typename Second::BOOST_NESTED_TEMPLATE apply
            <
                typename First::BOOST_NESTED_TEMPLATE apply<Node, State, Visitor>::type
              , State
              , Visitor
            >::type type;
        };

        template<typename Node, typename State, typename Visitor>
        static typename apply<Node, State, Visitor>::type
        call(Node const &node, State const &state, Visitor &visitor)
        {
            return Second::call(First::call(node, state, visitor), state, visitor);
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    // conditional_transform
    //   Conditionally execute a transform
    template<typename Predicate, typename IfTransform, typename ElseTransform>
    struct conditional_transform
    {
        template<typename Node, typename State, typename Visitor>
        struct apply
        {
            typedef typename boost::mpl::if_
            <
                typename Predicate::BOOST_NESTED_TEMPLATE apply<Node, State, Visitor>::type
              , IfTransform
              , ElseTransform
            >::type transform_type;

            typedef typename transform_type::BOOST_NESTED_TEMPLATE apply
            <
                Node
              , State
              , Visitor
            >::type type;
        };

        template<typename Node, typename State, typename Visitor>
        static typename apply<Node, State, Visitor>::type
        call(Node const &node, State const &state, Visitor &visitor)
        {
            return apply<Node, State, Visitor>::transform_type::call(node, state, visitor);
        }
    };

    template<typename Always>
    struct always_transform
    {
        template<typename, typename, typename>
        struct apply
        {
            typedef Always type;
        };

        template<typename Node, typename State, typename Visitor>
        static Always
        call(Node const &, State const &, Visitor &)
        {
            return Always();
        }
    };

}}

#endif
