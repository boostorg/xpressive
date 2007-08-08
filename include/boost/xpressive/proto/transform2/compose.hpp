///////////////////////////////////////////////////////////////////////////////
/// \file compose.hpp
/// A special-purpose proto transform for composing two transfomations. Given
/// two Transforms, expressions are transformed according to the first transform,
/// and the result is forwarded to the second for further transformation.
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_TRANSFORM2_COMPOSE_HPP_EAN_04_01_2007
#define BOOST_PROTO_TRANSFORM2_COMPOSE_HPP_EAN_04_01_2007

#include <boost/xpressive/proto/detail/prefix.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/detail/suffix.hpp>

namespace boost { namespace proto { namespace transform2
{

    // Composes two transforms
    template<typename Transform1, typename Transform2>
    struct compose
      : transform_base
    {
        template<typename Expr, typename State, typename Visitor>
        struct apply
        {
            typedef typename Transform2::template apply<
                typename Transform1::template apply<Expr, State, Visitor>::type
              , State
              , Visitor
            >::type type;
        };

        template<typename Expr, typename State, typename Visitor>
        static typename apply<Expr, State, Visitor>::type
        call(Expr const &expr, State const &state, Visitor &visitor)
        {
            return Transform2::call(Transform1::call(expr, state, visitor), state, visitor);
        }
    };

}}}

namespace boost { namespace proto
{
    template<typename Transform1, typename Transform2>
    struct is_transform<transform2::compose<Transform1, Transform2> >
      : mpl::true_
    {};
}}

#endif
