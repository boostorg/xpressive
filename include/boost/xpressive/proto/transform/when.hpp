///////////////////////////////////////////////////////////////////////////////
/// \file transform.hpp
/// Definition of when transform.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_TRANSFORM_CASE_HPP_EAN_10_29_2007
#define BOOST_PROTO_TRANSFORM_CASE_HPP_EAN_10_29_2007

#include <boost/mpl/if.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/type_traits.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/traits.hpp>
#include <boost/xpressive/proto/transform/call.hpp>
#include <boost/xpressive/proto/transform/make.hpp>

namespace boost { namespace proto
{
    template<typename T>
    struct is_aggregate
      : is_pod<T>
    {};

    template<typename Tag, typename Args, long N>
    struct is_aggregate<expr<Tag, Args, N> >
      : mpl::true_
    {};

    namespace detail
    {
        template<typename T, typename EnableIf = void>
        struct is_transform2_
          : mpl::false_
        {};

        template<typename T>
        struct is_transform2_<T, typename T::proto_is_transform_>
          : mpl::true_
        {};

        template<typename T>
        struct is_transform_
          : is_transform2_<T>
        {};

        template<template<typename...> class T, typename... Args>
        struct is_transform_<T<Args...> >
          : mpl::false_
        {};
    }

    template<typename T>
    struct is_transform
      : proto::detail::is_transform_<T>
    {};

    // work around GCC bug
    template<typename Tag, typename Args, long N>
    struct is_transform<expr<Tag, Args, N> >
      : mpl::false_
    {};

    namespace transform
    {
        namespace detail
        {
            // can't this apply template be always used? What is the purpose of the one below?
            template<
                typename Expr
              , typename State
              , typename Visitor
              , bool IsTransform
              , typename Return
              , typename... Args
            >
            struct apply
            {
                typedef 
                    typename boost::result_of<
                        transform::make<Return, Args...>(Expr, State, Visitor)
                    >::type
                lambda_type;

                // If the result of applying the lambda on the return type is a transform,
                // apply the transform rather than trying to construct it.
                typedef
                    typename mpl::if_<
                        proto::detail::is_transform2_<lambda_type>
                      , transform::call<lambda_type, Args...>
                      , transform::make<Return, Args...>
                    >::type
                transform_type;

                typedef typename boost::result_of<transform_type(Expr, State, Visitor)>::type type;

                static type call(Expr const &expr, State const &state, Visitor &visitor)
                {
                    return transform_type()(expr, state, visitor);
                }
            };

            template<
                typename Expr
              , typename State
              , typename Visitor
              , typename Return
              , typename... Args
            >
            struct apply<Expr, State, Visitor, true, Return, Args...>
            {
                typedef typename boost::result_of<
                    transform::call<Return, Args...>(Expr, State, Visitor)
                >::type type;

                static type call(Expr const &expr, State const &state, Visitor &visitor)
                {
                    return transform::call<Return, Args...>()(expr, state, visitor);
                }
            };

        }

        // Simple transform, takes a raw transform and
        // applies it directly.
        template<typename Grammar, typename Fun>
        struct when
          : Fun
        {
            typedef typename Grammar::proto_base_expr proto_base_expr;
        };

        // Function-style transforms, handled below...
        template<typename Grammar, typename Return, typename... Args>
        struct when<Grammar, Return(*)(Args...)>
          : when<Grammar, Return(Args...)>
        {};

        // Lambda-style transform, takes a raw or function-style
        // transform with arguments and applies it, OR takes a
        // (possibly lambda) type and constructor arguments.
        template<typename Grammar, typename Return, typename... Args>
        struct when<Grammar, Return(Args...)>
          : transform_base
        {
            typedef typename Grammar::proto_base_expr proto_base_expr;

            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
              : detail::apply<Expr, State, Visitor, is_transform<Return>::value, Return, Args...>
            {};

            template<typename Expr, typename State, typename Visitor>
            typename result<when(Expr, State, Visitor)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                return result<when(Expr, State, Visitor)>::call(expr, state, visitor);
            }
        };

    }

}}

#endif
