///////////////////////////////////////////////////////////////////////////////
// is_pure.hpp
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_XPRESSIVE_DETAIL_STATIC_IS_PURE_HPP_EAN_10_04_2005
#define BOOST_XPRESSIVE_DETAIL_STATIC_IS_PURE_HPP_EAN_10_04_2005

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/static_assert.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/xpressive/detail/detail_fwd.hpp>
#include <boost/xpressive/detail/static/as_xpr.hpp>
#include <boost/xpressive/detail/static/width_of.hpp>

///////////////////////////////////////////////////////////////////////////////
// equivalent to mpl::and_<X, Y>
#define BOOST_XPR_AND_PURE_(X, Y)                                                                   \
    mpl::bool_<X::value && Y::value>

namespace boost { namespace xpressive { namespace detail
{
    ///////////////////////////////////////////////////////////////////////////////
    // use_simple_repeat
    //
    template<typename Node, typename Tag = typename Node::tag_type>
    struct use_simple_repeat;

    ///////////////////////////////////////////////////////////////////////////////
    // is_pure
    //
    template<typename Node>
    struct is_pure;

    template<typename Tag, typename Arg0, typename Arg1>
    struct is_pure_impl;

    template<typename Matcher>
    struct is_pure_impl<proto2::noop_tag, Matcher, void>
      : mpl::bool_<as_matcher_type<Matcher>::type::pure>
    {};

    template<typename Left, typename Right>
    struct is_pure_impl<proto2::right_shift_tag, Left, Right>
      : BOOST_XPR_AND_PURE_(is_pure<Left>, is_pure<Right>)
    {};

    template<typename Left, typename Right>
    struct is_pure_impl<proto2::bitor_tag, Left, Right>
      : BOOST_XPR_AND_PURE_(is_pure<Left>, is_pure<Right>)
    {};

    template<typename Right>
    struct is_pure_impl<proto2::assign_tag, mark_tag, Right>
      : mpl::false_
    {};

    template<typename Right>
    struct is_pure_impl<proto2::assign_tag, set_initializer_type, Right>
      : mpl::true_
    {};

    template<typename Modifier, typename Node>
    struct is_pure_impl<modifier_tag, Modifier, Node>
      : is_pure<Node>
    {};

    template<typename Node, bool Positive>
    struct is_pure_impl<lookahead_tag<Positive>, Node, void>
      : is_pure<Node>
    {};

    template<typename Node, bool Positive>
    struct is_pure_impl<lookbehind_tag<Positive>, Node, void>
      : is_pure<Node>
    {};

    template<typename Node>
    struct is_pure_impl<keeper_tag, Node, void>
      : is_pure<Node>
    {};

    // when complementing a set or an assertion, the purity is that of the set (true) or the assertion
    template<typename Node>
    struct is_pure_impl<proto2::complement_tag, Node, void>
      : is_pure<Node>
    {};

    // The comma is used in list-initialized sets, which are pure
    template<typename Left, typename Right>
    struct is_pure_impl<proto2::comma_tag, Left, Right>
      : mpl::true_
    {};

    // The subscript operator[] is used for sets, as in set['a' | range('b','h')]
    // It is also used for actions, which by definition have side-effects and thus are impure
    template<typename Left, typename Right>
    struct is_pure_impl<proto2::subscript_tag, Left, Right>
      : mpl::false_
    {};

    template<typename Right>
    struct is_pure_impl<proto2::subscript_tag, set_initializer_type, Right>
      : mpl::true_
    {
        // If Left is "set" then make sure that Right is pure
        BOOST_MPL_ASSERT((is_pure<Right>));
    };

    // Quantified expressions are pure IFF they use the simple_repeat_matcher
    template<typename Node>
    struct is_pure_impl<proto2::unary_plus_tag, Node, void>
      : use_simple_repeat<Node>
    {};

    template<typename Node>
    struct is_pure_impl<proto2::unary_star_tag, Node, void>
      : use_simple_repeat<Node>
    {};

    template<typename Node>
    struct is_pure_impl<proto2::logical_not_tag, Node, void>
      : use_simple_repeat<Node>
    {};

    template<typename Node, uint_t Min, uint_t Max>
    struct is_pure_impl<generic_quant_tag<Min, Max>, Node, void>
      : use_simple_repeat<Node>
    {};

    template<typename Node>
    struct is_pure_impl<proto2::unary_minus_tag, Node, void>
      : is_pure<Node>
    {};

    // simple_repeat_helper
    template<bool B, quant_enum Q>
    struct use_simple_repeat_helper
      : mpl::false_
    {};

    template<>
    struct use_simple_repeat_helper<true, quant_fixed_width>
      : mpl::true_
    {};

    ///////////////////////////////////////////////////////////////////////////////
    // use_simple_repeat
    //  TODO this doesn't optimize +(_ >> "hello")
    template<typename Node, typename Tag>
    struct use_simple_repeat
      : mpl::bool_<width_of<Node>::value != unknown_width::value && is_pure<Node>::value>
    {
        // should never try to repeat something of 0-width
        //BOOST_MPL_ASSERT_RELATION(0, !=, width_of<Node>::value);
        BOOST_STATIC_ASSERT(0 != width_of<Node>::value);
    };

    template<typename Node>
    struct use_simple_repeat<Node, proto2::noop_tag>
      : use_simple_repeat_helper<
            as_matcher_type<typename Node::arg0_type>::type::pure
          , as_matcher_type<typename Node::arg0_type>::type::quant
        >
    {
        //BOOST_MPL_ASSERT_RELATION(0, !=, as_matcher_type<typename Node::arg0_type>::type::width);
        BOOST_STATIC_ASSERT(0 != as_matcher_type<typename Node::arg0_type>::type::width);
    };

    // is_pure
    template<typename Node>
    struct is_pure
      : is_pure_impl<
            typename Node::tag_type
          , typename proto2::unref<typename Node::arg0_type>::type
          , typename proto2::unref<typename Node::arg1_type>::type
        >
    {};

}}} // namespace boost::xpressive::detail

#endif
