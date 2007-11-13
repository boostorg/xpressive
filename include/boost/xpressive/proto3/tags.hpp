///////////////////////////////////////////////////////////////////////////////
/// \file tags.hpp
/// Contains definition of operator tag types.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO3_TAGS_HPP_EAN_10_28_2007
#define BOOST_PROTO3_TAGS_HPP_EAN_10_28_2007

#include <boost/xpressive/proto3/proto_fwd.hpp>

namespace boost { namespace proto
{

    namespace tag
    {

        struct terminal {};
        struct posit {};        
        struct negate {};
        struct dereference {};
        struct complement {};
        struct address_of {};
        struct logical_not {};
        struct pre_inc {};
        struct pre_dec {};
        struct post_inc {};
        struct post_dec {};
        struct shift_left {};
        struct shift_right {};
        struct multiplies {};
        struct divides {};
        struct modulus {};
        struct plus {};
        struct minus {};
        struct less {};
        struct greater {};
        struct less_equal {};
        struct greater_equal {};
        struct equal_to {};
        struct not_equal_to {};
        struct logical_or {};
        struct logical_and {};
        struct bitwise_and {};
        struct bitwise_or {};
        struct bitwise_xor {};
        struct comma {};
        struct mem_ptr {};
        struct assign {};
        struct shift_left_assign {};
        struct shift_right_assign {};
        struct multiplies_assign {};
        struct divides_assign {};
        struct modulus_assign {};
        struct plus_assign {};
        struct minus_assign {};
        struct bitwise_and_assign {};
        struct bitwise_or_assign {};
        struct bitwise_xor_assign {};
        struct subscript {};
        struct if_else_ {};
        struct function {};

        // For fusion
        struct proto_expr {};
        struct proto_expr_iterator {};
        struct proto_flat_view {};
    }

}}

#endif
