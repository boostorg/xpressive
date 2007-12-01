///////////////////////////////////////////////////////////////////////////////
/// \file proto_typeof.hpp
/// Type registrations so that proto expression templates can be used together
/// with the Boost.Typeof library.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_PROTO_TYPEOF_H
#define BOOST_PROTO_PROTO_TYPEOF_H

#include <boost/config.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/deep_copy.hpp>
#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::terminal)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::posit)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::negate)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::dereference)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::complement)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::address_of)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::logical_not)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::pre_inc)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::pre_dec)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::post_inc)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::post_dec)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::shift_left)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::shift_right)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::multiplies)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::divides)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::modulus)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::plus)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::minus)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::less)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::greater)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::less_equal)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::greater_equal)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::equal_to)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::not_equal_to)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::logical_or)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::logical_and)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::bitwise_and)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::bitwise_or)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::bitwise_xor)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::comma)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::mem_ptr)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::assign)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::shift_left_assign)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::shift_right_assign)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::multiplies_assign)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::divides_assign)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::modulus_assign)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::plus_assign)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::minus_assign)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::bitwise_and_assign)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::bitwise_or_assign)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::bitwise_xor_assign)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::subscript)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::if_else_)
BOOST_TYPEOF_REGISTER_TYPE(boost::proto::tag::function)

BOOST_TYPEOF_REGISTER_TYPE(boost::proto::exprns_::is_proto_expr)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::exprns_::expr, (typename)(typename)(long))
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::utility::literal, (typename)(typename))

BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::term, 1)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 1)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 2)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 3)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 4)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 5)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 6)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 7)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 8)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 9)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 10)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 11)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 12)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 13)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 14)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 15)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 16)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 17)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 18)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 19)
BOOST_TYPEOF_REGISTER_TEMPLATE(boost::proto::argsns_::args, 20)

#define BOOST_PROTO_AUTO(Var, Expr) BOOST_AUTO(Var, boost::proto::deep_copy(Expr))
#define BOOST_PROTO_AUTO_TPL(Var, Expr) BOOST_AUTO_TPL(Var, boost::proto::deep_copy(Expr))

#endif
