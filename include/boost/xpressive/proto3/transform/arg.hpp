///////////////////////////////////////////////////////////////////////////////
/// \file arg.hpp
/// Contains definition of the argN transforms.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO3_TRANSFORM_ARG_HPP_EAN_11_01_2007
#define BOOST_PROTO3_TRANSFORM_ARG_HPP_EAN_11_01_2007

#include <boost/xpressive/proto3/proto_fwd.hpp>
#include <boost/xpressive/proto3/traits.hpp>

namespace boost { namespace proto
{

    namespace transform
    {

        struct _expr : raw_transform
        {
            template<typename Expr, typename, typename>
            struct apply
            {
                typedef Expr type;
            };

            template<typename Expr, typename State, typename Visitor>
            static Expr const &
            call(Expr const &expr, State const &, Visitor &)
            {
                return expr;
            }
        };

        struct _state : raw_transform
        {
            template<typename, typename State, typename>
            struct apply
            {
                typedef State type;
            };

            template<typename Expr, typename State, typename Visitor>
            static State const &
            call(Expr const &, State const &state, Visitor &)
            {
                return state;
            }
        };

        struct _visitor : raw_transform
        {
            template<typename, typename, typename Visitor>
            struct apply
            {
                typedef Visitor type;
            };

            template<typename Expr, typename State, typename Visitor>
            static Visitor &
            call(Expr const &, State const &, Visitor &visitor)
            {
                return visitor;
            }
        };

        template<int I>
        struct _arg_c : raw_transform
        {
            template<typename Expr, typename, typename>
            struct apply
              : proto::result_of::arg_c<Expr, I>
            {};

            template<typename Expr, typename State, typename Visitor>
            static typename boost::proto::result_of::arg_c<Expr, I>::const_reference
            call(Expr const &expr, State const &, Visitor &)
            {
                return proto::arg_c<I>(expr);
            }
        };

        struct _arg0 : _arg_c<0> {};
        struct _arg1 : _arg_c<1> {};
        struct _arg2 : _arg_c<2> {};
        struct _arg3 : _arg_c<3> {};
        struct _arg4 : _arg_c<4> {};
        struct _arg5 : _arg_c<5> {};
        struct _arg6 : _arg_c<6> {};
        struct _arg7 : _arg_c<7> {};
        struct _arg8 : _arg_c<8> {};
        struct _arg9 : _arg_c<9> {};

        typedef _arg0 _arg;
        typedef _arg0 _left;
        typedef _arg1 _right;

    }

}}

#endif
