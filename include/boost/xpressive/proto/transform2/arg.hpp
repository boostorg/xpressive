///////////////////////////////////////////////////////////////////////////////
/// \file arg.hpp
/// Proto transforms for extracting arguments from expressions.
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_TRANSFORM2_ARG_HPP_EAN_12_16_2006
#define BOOST_PROTO_TRANSFORM2_ARG_HPP_EAN_12_16_2006

#include <boost/xpressive/proto/detail/prefix.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/traits.hpp>
#include <boost/xpressive/proto/detail/suffix.hpp>

namespace boost { namespace proto { namespace transform2
{
    // A transform that simply extracts the arg from an expression
    template<typename N, typename Transform>
    struct arg
      : transform_base
    {
        template<typename Expr, typename State, typename Visitor>
        struct apply
          : proto::result_of::arg<typename Transform::template apply<Expr, State, Visitor>::type, N>
        {};

        template<typename Expr, typename State, typename Visitor>
        static typename apply<Expr, State, Visitor>::type
        call(Expr const &expr, State const &state, Visitor &visitor)
        {
            // NOTE Grammar::call could return a temporary!
            // Don't return a dangling reference
            return proto::arg<N>(Transform::call(expr, state, visitor));
        }
    };

    // A transform that simply extracts the arg from an expression
    template<long N, typename Transform>
    struct arg_c
      : transform_base
    {
        template<typename Expr, typename State, typename Visitor>
        struct apply
          : proto::result_of::arg_c<typename Transform::template apply<Expr, State, Visitor>::type, N>
        {};

        template<typename Expr, typename State, typename Visitor>
        static typename apply<Expr, State, Visitor>::type
        call(Expr const &expr, State const &state, Visitor &visitor)
        {
            return proto::arg_c<N>(Transform::call(expr, state, visitor));
        }
    };

    // A transform that simply extracts the arg from an expression
    template<long N, typename Transform>
    struct _arg_c
      : arg_c<N, Transform>
    {};

    // A transform that simply extracts the left arg from an expression
    template<typename Transform>
    struct left
      : transform_base
    {
        template<typename Expr, typename State, typename Visitor>
        struct apply
          : proto::result_of::left<typename Transform::template apply<Expr, State, Visitor>::type>
        {};

        template<typename Expr, typename State, typename Visitor>
        static typename apply<Expr, State, Visitor>::type
        call(Expr const &expr, State const &state, Visitor &visitor)
        {
            return proto::left(Transform::call(expr, state, visitor));
        }
    };

    // A transform that simply extracts the right arg from an expression
    template<typename Transform>
    struct right
      : transform_base
    {
        template<typename Expr, typename State, typename Visitor>
        struct apply
          : proto::result_of::right<typename Transform::template apply<Expr, State, Visitor>::type>
        {};

        template<typename Expr, typename State, typename Visitor>
        static typename apply<Expr, State, Visitor>::type
        call(Expr const &expr, State const &state, Visitor &visitor)
        {
            return proto::right(Transform::call(expr, state, visitor));
        }
    };

    // Just return the state
    struct _state
      : transform_base
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

    // Just return the visitor
    struct _visitor
      : transform_base
    {
        template<typename, typename, typename Visitor>
        struct apply
        {
//            BOOST_MPL_ASSERT_RELATION(0,==,sizeof(Visitor));
            typedef Visitor type;
        };

        template<typename Expr, typename State, typename Visitor>
        static Visitor &
        call(Expr const &, State const &, Visitor &visitor)
        {
            return visitor;
        }
    };

}}}

namespace boost { namespace proto
{
    template<typename N, typename Transform>
    struct is_transform<transform2::arg<N, Transform> >
      : mpl::true_
    {};

    template<long N, typename Transform>
    struct is_transform<transform2::arg_c<N, Transform> >
      : mpl::true_
    {};

    template<long N, typename Transform>
    struct is_transform<transform2::_arg_c<N, Transform> >
      : mpl::true_
    {};

    template<typename Transform>
    struct is_transform<transform2::left<Transform> >
      : mpl::true_
    {};

    template<typename Transform>
    struct is_transform<transform2::right<Transform> >
      : mpl::true_
    {};

    template<>
    struct is_transform<transform2::_state>
      : mpl::true_
    {};

    template<>
    struct is_transform<transform2::_visitor>
      : mpl::true_
    {};
}}

#endif
