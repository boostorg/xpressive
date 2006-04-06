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

#include <vector>
#include <boost/ref.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/not_equal_to.hpp>
#include <boost/xpressive/detail/detail_fwd.hpp>
#include <boost/xpressive/detail/static/as_xpr.hpp>
#include <boost/xpressive/detail/static/width_of.hpp>

namespace boost { namespace xpressive { namespace detail
{
    ///////////////////////////////////////////////////////////////////////////////
    // use_simple_repeat
    //
    template<typename Xpr>
    struct use_simple_repeat;

    ///////////////////////////////////////////////////////////////////////////////
    // is_pure
    //
    template<typename Xpr>
    struct is_pure;

    template<>
    struct is_pure<no_next>
      : mpl::true_
    {
    };

    template<typename Matcher>
    struct is_pure<proto::unary_op<Matcher, proto::noop_tag> >
      : mpl::bool_<as_matcher<Matcher>::type::pure>
    {
    };

    template<typename Left, typename Right>
    struct is_pure<proto::binary_op<Left, Right, proto::right_shift_tag> >
      : mpl::and_<is_pure<Left>, is_pure<Right> >
    {
    };

    template<typename Left, typename Right>
    struct is_pure<proto::binary_op<Left, Right, proto::bitor_tag> >
      : mpl::and_<is_pure<Left>, is_pure<Right> >
    {
    };

    template<typename Right>
    struct is_pure<proto::binary_op<mark_tag, Right, proto::assign_tag> >
      : mpl::false_
    {
    };

    template<typename Right>
    struct is_pure<proto::binary_op<set_initializer_type, Right, proto::assign_tag> >
      : mpl::true_
    {
    };

    template<typename Modifier, typename Xpr>
    struct is_pure<proto::binary_op<Modifier, Xpr, modifier_tag> >
      : is_pure<Xpr>
    {
    };

    template<typename Xpr, bool Positive>
    struct is_pure<proto::unary_op<Xpr, lookahead_tag<Positive> > >
      : is_pure<Xpr>
    {
    };

    template<typename Xpr, bool Positive>
    struct is_pure<proto::unary_op<Xpr, lookbehind_tag<Positive> > >
      : is_pure<Xpr>
    {
    };

    template<typename Xpr>
    struct is_pure<proto::unary_op<Xpr, keeper_tag> >
      : is_pure<Xpr>
    {
    };

    template<typename BidiIter>
    struct is_pure<proto::unary_op<reference_wrapper<basic_regex<BidiIter> const>, proto::noop_tag> >
      : mpl::false_
    {
    };

    // when complementing a set or an assertion, the purity is that of the set (true) or the assertion
    template<typename Op>
    struct is_pure<proto::unary_op<Op, proto::complement_tag> >
      : is_pure<Op>
    {
    };

    // The comma is used in list-initialized sets, which are pure
    template<typename Left, typename Right>
    struct is_pure<proto::binary_op<Left, Right, proto::comma_tag> >
      : mpl::true_
    {
    };

    // The subscript operator[] is used for sets, as in set['a' | range('b','h')]
    // It is also used for actions, which by definition have side-effects and thus are impure
    template<typename Left, typename Right>
    struct is_pure<proto::binary_op<Left, Right, proto::subscript_tag> >
      : is_same<Left, set_initializer_type>
    {
        // If Left is "set" then make sure that Right is pure
        BOOST_MPL_ASSERT
        ((
            mpl::or_
            <
                mpl::not_<is_same<Left, set_initializer_type> >
              , is_pure<Right>
            >
        ));
    };

    // Quantified expressions are pure IFF they use the simple_repeat_matcher
    template<typename Op>
    struct is_pure<proto::unary_op<Op, proto::unary_plus_tag> >
      : use_simple_repeat<Op>
    {
    };

    template<typename Op>
    struct is_pure<proto::unary_op<Op, proto::unary_star_tag> >
      : use_simple_repeat<Op>
    {
    };

    template<typename Op>
    struct is_pure<proto::unary_op<Op, proto::logical_not_tag> >
      : use_simple_repeat<Op>
    {
    };

    template<typename Op, uint_t Min, uint_t Max>
    struct is_pure<proto::unary_op<Op, generic_quant_tag<Min, Max> > >
      : use_simple_repeat<Op>
    {
    };

    template<typename Op>
    struct is_pure<proto::unary_op<Op, proto::unary_minus_tag> >
      : is_pure<Op>
    {
    };

    ///////////////////////////////////////////////////////////////////////////////
    // use_simple_repeat
    //  TODO this doesn't optimize +(_ >> "hello")
    template<typename Xpr>
    struct use_simple_repeat
      : mpl::and_<mpl::not_equal_to<width_of<Xpr>, unknown_width>, is_pure<Xpr> >
    {
        // should never try to quantify something of 0-width
        BOOST_MPL_ASSERT_RELATION(0, !=, width_of<Xpr>::value);
    };

    template<bool B, quant_enum Q> struct use_simple_repeat_helper : mpl::false_ {};
    template<> struct use_simple_repeat_helper<true, quant_fixed_width> : mpl::true_ {};

    template<typename Matcher>
    struct use_simple_repeat<proto::unary_op<Matcher, proto::noop_tag> >
      : use_simple_repeat_helper<as_matcher<Matcher>::type::pure, as_matcher<Matcher>::type::quant>
    {
        BOOST_MPL_ASSERT_RELATION(0, !=, as_matcher<Matcher>::type::width);
    };

    template<typename Op, typename Arg>
    struct is_pure<proto::op_proxy<Op, Arg> >
      : is_pure<Op>
    {
    };

}}} // namespace boost::xpressive::detail

#endif
