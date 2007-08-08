///////////////////////////////////////////////////////////////////////////////
/// \file transform2.hpp
/// Includes all the transforms in the transform2/ sub-directory.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_TRANSFORM2_HPP_EAN_06_23_2007
#define BOOST_PROTO_TRANSFORM2_HPP_EAN_06_23_2007

#include <boost/xpressive/proto/detail/prefix.hpp> // must be first include
#include <boost/xpressive/proto/transform2/apply.hpp>
#include <boost/xpressive/proto/transform2/arg.hpp>
//#include <boost/xpressive/proto/transform2/branch.hpp>
#include <boost/xpressive/proto/transform2/compose.hpp>
#include <boost/xpressive/proto/transform2/construct.hpp>
#include <boost/xpressive/proto/transform2/fold.hpp>
#include <boost/xpressive/proto/transform2/fold_tree.hpp>
#include <boost/xpressive/proto/transform2/function.hpp>
//#include <boost/xpressive/proto/transform2/list.hpp>
//#include <boost/xpressive/proto/transform2/pass_through.hpp>
#include <boost/xpressive/proto/detail/suffix.hpp> // must be last include

namespace boost { namespace proto
{
    using transform2::case_;
    using transform2::default_;
    using transform2::_expr;
    using transform2::_state;
    using transform2::_visitor;
    using transform2::_arg_c;
    using transform2::apply_;
    using transform2::always;
    using transform2::compose;
    using transform2::fold;
    using transform2::reverse_fold;
    using transform2::fold_tree;
    using transform2::reverse_fold_tree;
    using transform2::_children;
    using transform2::pop_front;

    typedef _arg_c<0>  _arg0;
    typedef _arg_c<1>  _arg1;
    typedef _arg_c<2>  _arg2;
    typedef _arg_c<3>  _arg3;
    typedef _arg0      _arg;
    typedef _arg0      _left;
    typedef _arg1      _right;
}}

#endif
