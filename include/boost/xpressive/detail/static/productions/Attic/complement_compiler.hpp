////////////////////////////////////////////////////////////////////////////
// complement_compiler.hpp
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_XPRESSIVE_DETAIL_STATIC_PRODUCTIONS_COMPLEMENT_COMPILER_HPP_EAN_10_04_2005
#define BOOST_XPRESSIVE_DETAIL_STATIC_PRODUCTIONS_COMPLEMENT_COMPILER_HPP_EAN_10_04_2005

#include <boost/mpl/assert.hpp>
#include <boost/xpressive/detail/detail_fwd.hpp>
#include <boost/xpressive/proto2/proto.hpp>
#include <boost/xpressive/proto2/compiler/transform.hpp>
#include <boost/xpressive/detail/utility/dont_care.hpp>
#include <boost/xpressive/detail/utility/never_true.hpp>
#include <boost/xpressive/detail/static/productions/set_compilers.hpp>
#include <boost/xpressive/detail/static/productions/domain_tags.hpp>
#include <boost/xpressive/detail/static/productions/charset_transforms.hpp>

namespace boost { namespace xpressive { namespace detail
{

    template<typename OpTag, typename Arg0, typename Arg1>
    struct complement
    {
        // If your compile breaks here, then you are applying the complement operator ~
        // to something that does not support it. For instance, ~(_ >> 'a') will trigger this
        // assertion because the sub-expression (_ >> 'a') has no complement.
        BOOST_MPL_ASSERT((never_true<OpTag>));
    };

    ///////////////////////////////////////////////////////////////////////////////
    //
    template<typename Char, bool Not>
    struct complement<proto2::noop_tag, literal_placeholder<Char, Not>, void>
    {
        template<typename Node, typename>
        struct apply
          : proto2::literal<literal_placeholder<Char, !Not> >
        {};

        template<typename Node, typename Visitor>
        static typename apply<Node, Visitor>::type call(Node const &node, Visitor &)
        {
            typedef typename apply<Node, Visitor>::type type;
            type that = {{proto2::arg(node).ch_}};
            return that;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    //
    template<typename Traits, int Size>
    struct complement<proto2::noop_tag, set_matcher<Traits, Size>, void>
    {
        template<typename Node, typename>
        struct apply
          : proto2::literal<set_matcher<Traits, Size> >
        {};

        template<typename Node, typename Visitor>
        static typename apply<Node, Visitor>::type call(Node const &node, Visitor &)
        {
            typedef typename apply<Node, Visitor>::type type;
            type that = {proto2::arg(node)};
            that.arg0.complement();
            return that;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    //
    template<>
    struct complement<proto2::noop_tag, posix_charset_placeholder, void>
    {
        template<typename, typename>
        struct apply
          : proto2::literal<posix_charset_placeholder>
        {};

        template<typename Node, typename Visitor>
        static typename apply<Node, Visitor>::type call(Node const &node, Visitor &)
        {
            typedef typename apply<Node, Visitor>::type type;
            type that = {proto2::arg(node)};
            that.arg0.not_ = !that.arg0.not_;
            return that;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    //
    template<typename Right>
    struct complement<proto2::subscript_tag, set_initializer_type, Right>
    {
        template<typename Node, typename Visitor>
        struct apply
          : charset_transform::BOOST_NESTED_TEMPLATE apply<
                Node
              , dont_care
              , Visitor
            >
        {};

        template<typename Node, typename Visitor>
        static typename apply<Node, Visitor>::type call(Node const &node, Visitor &visitor)
        {
            return charset_transform::call(node, dont_care(), visitor, true);
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    // for complementing a list-initialized set, as in ~(set= 'a','b','c')
    template<typename Left, typename Right>
    struct complement<proto2::comma_tag, Left, Right>
    {
        // First, convert the parse tree into a set_matcher
        template<typename Node, typename Visitor>
        struct apply
          : proto2::literal<typename proto2::compiler<proto2::comma_tag, lst_tag>
                ::BOOST_NESTED_TEMPLATE apply<
                    Node
                  , dont_care
                  , Visitor
                >::type
            >
        {};

        template<typename Node, typename Visitor>
        static typename apply<Node, Visitor>::type call(Node const &node, Visitor &visitor)
        {
            typedef typename apply<Node, Visitor>::type type;
            type that = {proto2::compile(node, dont_care(), visitor, lst_tag())};
            that.arg0.complement();
            return that;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    //
    template<typename Arg>
    struct complement<lookahead_tag<true>, Arg, void>
    {
        template<typename Node, typename Visitor>
        struct apply
          : proto2::unary_op<lookahead_tag<false>, Arg>
        {};

        template<typename Node, typename Visitor>
        static typename apply<Node, Visitor>::type call(Node const &node, Visitor &)
        {
            typedef typename apply<Node, Visitor>::type type;
            type that = {proto2::arg(node)};
            return that;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    //
    template<typename Arg>
    struct complement<lookbehind_tag<true>, Arg, void>
    {
        template<typename Node, typename Visitor>
        struct apply
          : proto2::unary_op<lookbehind_tag<false>, Arg>
        {};

        template<typename Node, typename Visitor>
        static typename apply<Node, Visitor>::type call(Node const &node, Visitor &)
        {
            typedef typename apply<Node, Visitor>::type type;
            type that = {proto2::arg(node)};
            return that;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    //
    template<>
    struct complement<proto2::noop_tag, assert_word_placeholder<word_boundary<true> >, void>
    {
        template<typename Node, typename Visitor>
        struct apply
          : proto2::literal<assert_word_placeholder<word_boundary<false> > >
        {};

        template<typename Node, typename Visitor>
        static typename apply<Node, Visitor>::type call(Node const &node, Visitor &)
        {
            typedef typename apply<Node, Visitor>::type type;
            type that = {};
            return that;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    //
    template<>
    struct complement<proto2::noop_tag, logical_newline_placeholder, void>
    {
        template<typename Node, typename Visitor>
        struct apply
          : proto2::binary_op<
                proto2::right_shift_tag
              , typename proto2::unary_op<lookahead_tag<false>, Node>::type
              , proto2::literal<any_matcher>::type
            >
        {};

        template<typename Node, typename Visitor>
        static typename apply<Node, Visitor>::type call(Node const &node, Visitor &)
        {
            typedef typename apply<Node, Visitor>::type type;
            type that = {{node}, {any_matcher()}};
            return that;
       }
    };

    ///////////////////////////////////////////////////////////////////////////////
    // complementing a complement is a no-node
    template<typename Arg>
    struct complement<proto2::complement_tag, Arg, void>
    {
        template<typename Node, typename Visitor>
        struct apply
        {
            typedef Arg type;
        };

        template<typename Node, typename Visitor>
        static typename apply<Node, Visitor>::type call(Node const &node, Visitor &)
        {
            return proto2::arg(node);
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    //
    template<typename Char>
    struct complement<proto2::noop_tag, range_placeholder<Char>, void>
    {
        template<typename Node, typename Visitor>
        struct apply
          : proto2::literal<range_placeholder<Char> >
        {};

        template<typename Node, typename Visitor>
        static typename apply<Node, Visitor>::type call(Node const &node, Visitor &)
        {
            typedef typename apply<Node, Visitor>::type type;
            type that = {proto2::arg(node)};
            that.arg0.not_ = !that.arg0.not_;
            return that;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    // complement_transform
    struct complement_transform
    {
        template<typename Node, typename, typename Visitor>
        struct apply
        {
            typedef typename Node::arg0_type op_type;
            typedef typename op_type::tag_type tag_type;
            typedef typename proto2::unref<typename op_type::arg0_type>::type arg0_type;
            typedef typename proto2::unref<typename op_type::arg1_type>::type arg1_type;

            typedef complement<tag_type, arg0_type, arg1_type> complement;
            typedef typename complement::BOOST_NESTED_TEMPLATE apply<op_type, Visitor>::type type;
        };

        template<typename Node, typename State, typename Visitor>
        static typename apply<Node, State, Visitor>::type
        call(Node const &node, State const &, Visitor &visitor)
        {
            typedef typename apply<Node, State, Visitor>::complement complement;
            return complement::call(proto2::arg(node), visitor);
        }
    };

}}}

namespace boost { namespace proto2
{

    template<>
    struct compiler<complement_tag, xpressive::detail::seq_tag, void>
      : transform_compiler<xpressive::detail::complement_transform, xpressive::detail::seq_tag>
    {
    };

}}

#endif
