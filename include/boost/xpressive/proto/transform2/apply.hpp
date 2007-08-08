///////////////////////////////////////////////////////////////////////////////
/// \file apply.hpp
/// Proto transforms for applying other Proto transforms.
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_TRANSFORM2_APPLY_HPP_EAN_06_23_2007
#define BOOST_PROTO_TRANSFORM2_APPLY_HPP_EAN_06_23_2007

#include <boost/xpressive/proto/detail/prefix.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/detail/suffix.hpp>

namespace boost { namespace proto { namespace transform2
{
    namespace detail_
    {
        template<typename T>
        T &as_lvalue(T &t)
        {
            return t;
        }

        template<typename T>
        T const &as_lvalue(T const &t)
        {
            return t;
        }
    }

    template<typename Transform, typename Expr, typename State, typename Visitor>
    struct apply_
      : transform_base
    {
        typedef typename detail_::as_transform<Transform>::type transform_type;
        typedef typename detail_::as_transform<Expr>::type expr_type;
        typedef typename detail_::as_transform<State>::type state_type;
        typedef typename detail_::as_transform<Visitor>::type visitor_type;

        template<typename E, typename S, typename V>
        struct apply
          : transform_type::template apply<
                typename expr_type::template apply<E, S, V>::type
              , typename state_type::template apply<E, S, V>::type
              , typename visitor_type::template apply<E, S, V>::type
            >
        {};

        template<typename E, typename S, typename V>
        static typename apply<E, S, V>::type
        call(E const &e, S const &s, V &v)
        {
            typedef typename visitor_type::template apply<E, S, V>::type visitor;
            return transform_type::call(
                expr_type::call(e, s, v)
              , state_type::call(e, s, v)
              , const_cast<visitor &>(detail_::as_lvalue(visitor_type::call(e, s, v)))
            );
        }
    };

    struct ignore { template<typename T> ignore(T const &) {} };

    template<typename T>
    struct always : proto::transform_base
    {
        template<class,class,class> struct apply { typedef T type; };
        static T call(ignore,ignore,ignore) { return T(); }
    };

}}}

namespace boost { namespace proto
{
    template<typename Transform, typename Expr, typename State, typename Visitor>
    struct is_transform<transform2::apply_<Transform, Expr, State, Visitor> >
      : mpl::true_
    {};

    template<typename T>
    struct is_transform<transform2::always<T> >
      : mpl::true_
    {};
}}

#endif
