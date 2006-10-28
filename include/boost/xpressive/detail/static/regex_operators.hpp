///////////////////////////////////////////////////////////////////////////////
// regex_operators.hpp
//
//  Copyright 2005 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_XPRESSIVE_DETAIL_STATIC_REGEX_OPERATORS_HPP_EAN_10_04_2005
#define BOOST_XPRESSIVE_DETAIL_STATIC_REGEX_OPERATORS_HPP_EAN_10_04_2005

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/utility/enable_if.hpp>
#include <boost/xpressive/proto2/proto.hpp>
#include <boost/xpressive/basic_regex.hpp>

namespace boost { namespace xpressive
{

// BUGBUG store by reference here!!!


///////////////////////////////////////////////////////////////////////////////
// operator +
template<typename BidiIter>
inline typename proto2::unary_op
<
    proto2::unary_plus_tag
  , typename proto2::literal<basic_regex<BidiIter> >::type
>::type
operator +(basic_regex<BidiIter> const &regex)
{
    typename proto2::unary_op
    <
        proto2::unary_plus_tag
      , typename proto2::literal<basic_regex<BidiIter> >::type
    >::type that = {{regex}};
    return that;
}

///////////////////////////////////////////////////////////////////////////////
// operator *
template<typename BidiIter>
inline typename proto2::unary_op
<
    proto2::unary_star_tag
  , typename proto2::literal<basic_regex<BidiIter> >::type
>::type
operator *(basic_regex<BidiIter> const &regex)
{
    typename proto2::unary_op
    <
        proto2::unary_star_tag
      , typename proto2::literal<basic_regex<BidiIter> >::type
    >::type that = {{regex}};
    return that;
}

///////////////////////////////////////////////////////////////////////////////
// operator !
template<typename BidiIter>
inline typename proto2::unary_op
<
    proto2::logical_not_tag
  , typename proto2::literal<basic_regex<BidiIter> >::type
>::type
operator !(basic_regex<BidiIter> const &regex)
{
    typename proto2::unary_op
    <
        proto2::logical_not_tag
      , typename proto2::literal<basic_regex<BidiIter> >::type
    >::type that = {{regex}};
    return that;
}

///////////////////////////////////////////////////////////////////////////////
// operator >>
template<typename Right, typename BidiIter>
inline typename lazy_disable_if
<
    proto2::is_op<Right>
  , proto2::binary_op
    <
        proto2::right_shift_tag
      , typename proto2::literal<basic_regex<BidiIter> >::type
      , typename proto2::literal<Right>::type
    >
>::type
operator >>(basic_regex<BidiIter> const &regex, Right const &right)
{
    typename proto2::binary_op
    <
        proto2::right_shift_tag
      , typename proto2::literal<basic_regex<BidiIter> >::type
      , typename proto2::literal<Right>::type
    >::type that = {{regex}, {right}};
    return that;
}

///////////////////////////////////////////////////////////////////////////////
// operator >>
template<typename BidiIter, typename Left>
inline typename lazy_disable_if
<
    proto2::is_op<Left>
  , proto2::binary_op
    <
        proto2::right_shift_tag
      , typename proto2::literal<Left>::type
      , typename proto2::literal<basic_regex<BidiIter> >::type
    >
>::type
operator >>(Left const &left, basic_regex<BidiIter> const &regex)
{
    typename proto2::binary_op
    <
        proto2::right_shift_tag
      , typename proto2::literal<Left>::type
      , typename proto2::literal<basic_regex<BidiIter> >::type
    >::type that = {{left}, {regex}};
    return that;
}

///////////////////////////////////////////////////////////////////////////////
// operator >>
template<typename BidiIter>
inline typename proto2::binary_op
<
    proto2::right_shift_tag
  , typename proto2::literal<basic_regex<BidiIter> >::type
  , typename proto2::literal<basic_regex<BidiIter> >::type
>::type
operator >>(basic_regex<BidiIter> const &left, basic_regex<BidiIter> const &right)
{
    typename proto2::binary_op
    <
        proto2::right_shift_tag
      , typename proto2::literal<basic_regex<BidiIter> >::type
      , typename proto2::literal<basic_regex<BidiIter> >::type
    >::type that = {{left}, {right}};
    return that;
}

///////////////////////////////////////////////////////////////////////////////
// operator |
template<typename Right, typename BidiIter>
inline typename lazy_disable_if
<
    proto2::is_op<Right>
  , proto2::binary_op
    <
        proto2::bitor_tag
      , typename proto2::literal<basic_regex<BidiIter> >::type
      , typename proto2::literal<Right>::type
    >
>::type
operator |(basic_regex<BidiIter> const &regex, Right const &right)
{
    typename proto2::binary_op
    <
        proto2::bitor_tag
      , typename proto2::literal<basic_regex<BidiIter> >::type
      , typename proto2::literal<Right>::type
    >::type that = {{regex}, {right}};
    return that;
}

///////////////////////////////////////////////////////////////////////////////
// operator |
template<typename BidiIter, typename Left>
inline typename lazy_disable_if
<
    proto2::is_op<Left>
  , proto2::binary_op
    <
        proto2::bitor_tag
      , typename proto2::literal<Left>::type
      , typename proto2::literal<basic_regex<BidiIter> >::type
    >
>::type
operator |(Left const &left, basic_regex<BidiIter> const &regex)
{
    typename proto2::binary_op
    <
        proto2::bitor_tag
      , typename proto2::literal<Left>::type
      , typename proto2::literal<basic_regex<BidiIter> >::type
    >::type that = {{left}, {regex}};
    return that;
}

///////////////////////////////////////////////////////////////////////////////
// operator |
template<typename BidiIter>
inline typename proto2::binary_op
<
    proto2::bitor_tag
  , typename proto2::literal<basic_regex<BidiIter> >::type
  , typename proto2::literal<basic_regex<BidiIter> >::type
>::type
operator |(basic_regex<BidiIter> const &left, basic_regex<BidiIter> const &right)
{
    typename proto2::binary_op
    <
        proto2::bitor_tag
      , typename proto2::literal<basic_regex<BidiIter> >::type
      , typename proto2::literal<basic_regex<BidiIter> >::type
    >::type that = {{left}, {right}};
    return that;
}

}}

#endif
