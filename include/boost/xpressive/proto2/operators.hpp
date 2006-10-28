///////////////////////////////////////////////////////////////////////////////
/// \file operators.hpp
/// Contains all the overloaded operators that make it possible to build
/// expression templates using proto2 components
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO2_OPERATORS_HPP_EAN_04_01_2005
#define BOOST_PROTO2_OPERATORS_HPP_EAN_04_01_2005

#include <boost/mpl/or.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/preprocessor/punctuation/comma.hpp>
#include <boost/xpressive/proto2/proto_fwd.hpp>
#include <boost/xpressive/proto2/op_tags.hpp>
#include <boost/xpressive/proto2/op_base.hpp>

namespace boost { namespace proto2
{
#define BOOST_PROTO2_UNARY_OP(op, tag)\
    template<typename Tag, typename Args, long Arity>\
    inline basic_op<tag, mpl::vector1<ref<basic_op<Tag, Args, Arity> > > > const\
    operator op(basic_op<Tag, Args, Arity> const &arg)\
    {\
        basic_op<tag, mpl::vector1<ref<basic_op<Tag, Args, Arity> > > > that = {{arg}};\
        return that;\
    }\
    /**/

#define BOOST_PROTO2_BINARY_OP(op, tag)\
    template<typename LTag, typename LArgs, long LArity, typename Right>\
    inline basic_op<tag, mpl::vector2<ref<basic_op<LTag, LArgs, LArity> >, typename literal<Right>::type> > const\
    operator op(basic_op<LTag, LArgs, LArity> const &left, Right const &right)\
    {\
        basic_op<tag, mpl::vector2<ref<basic_op<LTag, LArgs, LArity> >, typename literal<Right>::type> > that = {{left}, proto2::lit(right)};\
        return that;\
    }\
    template<typename Left, typename RTag, typename RArgs, long RArity>\
    inline basic_op<tag, mpl::vector2<typename literal<Left>::type, ref<basic_op<RTag, RArgs, RArity> > > > const\
    operator op(Left const &left, basic_op<RTag, RArgs, RArity> const &right)\
    {\
        basic_op<tag, mpl::vector2<typename literal<Left>::type, ref<basic_op<RTag, RArgs, RArity> > > > that = {proto2::lit(left), {right}};\
        return that;\
    }\
    template<typename LTag, typename LArgs, long LArity, typename RTag, typename RArgs, long RArity>\
    inline basic_op<tag, mpl::vector2<ref<basic_op<LTag, LArgs, LArity> >, ref<basic_op<RTag, RArgs, RArity> > > > const\
    operator op(basic_op<LTag, LArgs, LArity> const &left, basic_op<RTag, RArgs, RArity> const &right)\
    {\
        basic_op<tag, mpl::vector2<ref<basic_op<LTag, LArgs, LArity> >, ref<basic_op<RTag, RArgs, RArity> > > > that = {{left}, {right}};\
        return that;\
    }\
    /**/

    BOOST_PROTO2_UNARY_OP(+, unary_plus_tag)
    BOOST_PROTO2_UNARY_OP(-, unary_minus_tag)
    BOOST_PROTO2_UNARY_OP(*, unary_star_tag)
    BOOST_PROTO2_UNARY_OP(~, complement_tag)
    BOOST_PROTO2_UNARY_OP(&, address_of_tag)
    BOOST_PROTO2_UNARY_OP(!, logical_not_tag)
    BOOST_PROTO2_UNARY_OP(++, pre_inc_tag)
    BOOST_PROTO2_UNARY_OP(--, pre_dec_tag)

    BOOST_PROTO2_BINARY_OP(<<, left_shift_tag)
    BOOST_PROTO2_BINARY_OP(>>, right_shift_tag)
    BOOST_PROTO2_BINARY_OP(*, multiply_tag)
    BOOST_PROTO2_BINARY_OP(/, divide_tag)
    BOOST_PROTO2_BINARY_OP(%, modulus_tag)
    BOOST_PROTO2_BINARY_OP(+, add_tag)
    BOOST_PROTO2_BINARY_OP(-, subtract_tag)
    BOOST_PROTO2_BINARY_OP(<, less_tag)
    BOOST_PROTO2_BINARY_OP(>, greater_tag)
    BOOST_PROTO2_BINARY_OP(<=, less_equal_tag)
    BOOST_PROTO2_BINARY_OP(>=, greater_equal_tag)
    BOOST_PROTO2_BINARY_OP(==, equal_tag)
    BOOST_PROTO2_BINARY_OP(!=, not_equal_tag)
    BOOST_PROTO2_BINARY_OP(||, logical_or_tag)
    BOOST_PROTO2_BINARY_OP(&&, logical_and_tag)
    BOOST_PROTO2_BINARY_OP(&, bitand_tag)
    BOOST_PROTO2_BINARY_OP(|, bitor_tag)
    BOOST_PROTO2_BINARY_OP(^, bitxor_tag)
    BOOST_PROTO2_BINARY_OP(BOOST_PP_COMMA(), comma_tag)
    BOOST_PROTO2_BINARY_OP(->*, mem_ptr_tag)

    BOOST_PROTO2_BINARY_OP(<<=, left_shift_assign_tag)
    BOOST_PROTO2_BINARY_OP(>>=, right_shift_assign_tag)
    BOOST_PROTO2_BINARY_OP(*=, multiply_assign_tag)
    BOOST_PROTO2_BINARY_OP(/=, divide_assign_tag)
    BOOST_PROTO2_BINARY_OP(%=, modulus_assign_tag)
    BOOST_PROTO2_BINARY_OP(+=, add_assign_tag)
    BOOST_PROTO2_BINARY_OP(-=, subtract_assign_tag)
    BOOST_PROTO2_BINARY_OP(&=, bitand_assign_tag)
    BOOST_PROTO2_BINARY_OP(|=, bitor_assign_tag)
    BOOST_PROTO2_BINARY_OP(^=, bitxor_assign_tag)

    template<typename Tag, typename Args, long Arity>
    inline basic_op<post_inc_tag, mpl::vector1<ref<basic_op<Tag, Args, Arity> > > > const
    operator ++(basic_op<Tag, Args, Arity> const &arg, int)
    {
        basic_op<post_inc_tag, mpl::vector1<ref<basic_op<Tag, Args, Arity> > > > that = {{arg}};
        return that;
    }

    template<typename Tag, typename Args, long Arity>
    inline basic_op<post_dec_tag, mpl::vector1<ref<basic_op<Tag, Args, Arity> > > > const
    operator --(basic_op<Tag, Args, Arity> const &arg, int)
    {
        basic_op<post_dec_tag, mpl::vector1<ref<basic_op<Tag, Args, Arity> > > > that = {{arg}};
        return that;
    }

#undef BOOST_PROTO2_UNARY_OP
#undef BOOST_PROTO2_BINARY_OP

}}

#endif
