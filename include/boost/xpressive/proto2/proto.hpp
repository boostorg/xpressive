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
    ///////////////////////////////////////////////////////////////////////////////
    // compile_result
    template<typename Node, typename State, typename Visitor, typename DomainTag>
    struct compile_result
    {
        typedef typename as_op_type<Node>::type op_type;
        typedef compiler<typename tag_type<op_type>::type, DomainTag> compiler;
        typedef typename compiler::BOOST_NESTED_TEMPLATE apply<op_type, State, Visitor>::type type;
    };

    ///////////////////////////////////////////////////////////////////////////////
    // compile
    template<typename Node, typename State, typename Visitor, typename DomainTag>
    typename compile_result<Node, State, Visitor, DomainTag>::type const
    compile(Node const &node, State const &state, Visitor &visitor, DomainTag)
    {
        typedef typename as_op_type<Node>::type op_type;
        typedef compiler<typename tag_type<op_type>::type, DomainTag> compiler;
        return compiler::call(proto2::as_op(node), state, visitor);
    }

}} // namespace boost::proto2

#endif
