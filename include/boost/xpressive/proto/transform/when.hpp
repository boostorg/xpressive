///////////////////////////////////////////////////////////////////////////////
/// \file transform.hpp
/// Definition of when transform.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_TRANSFORM_CASE_HPP_EAN_10_29_2007
#define BOOST_PROTO_TRANSFORM_CASE_HPP_EAN_10_29_2007

#include <boost/mpl/bool.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/aux_/has_type.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/transform/call.hpp>

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
            template<typename... T>
            struct typelist
            {
                typedef void type;
            };

            template<typename T, bool HasType = mpl::aux::has_type<T>::value>
            struct nested_type
            {
                typedef typename T::type type;
            };

            template<typename T>
            struct nested_type<T, false>
            {
                typedef T type;
            };

            template<typename T, typename Args, typename EnableIf = void>
            struct nested_type_if
              : nested_type<T>
            {};

            template<typename T, typename... Args>
            struct nested_type_if<T, typelist<Args...>, typename typelist<typename Args::not_applied_...>::type>
            {
                typedef T type;
                typedef void not_applied_;
            };

            template<typename R, typename Expr, typename State, typename Visitor
                , bool IsTransform = is_transform<R>::value
            >
            struct apply_lambda_;

            template<typename R, typename Expr, typename State, typename Visitor>
            struct apply_lambda_aux_
            {
                typedef R type;
                typedef void not_applied_;
            };

            template<template<typename...> class R, typename... Args, typename Expr, typename State, typename Visitor>
            struct apply_lambda_aux_<R<Args...>, Expr, State, Visitor>
              : nested_type_if<
                    R<typename apply_lambda_<Args, Expr, State, Visitor>::type...>
                  , typelist<apply_lambda_<Args, Expr, State, Visitor>...>
                >
            {};

            template<typename R, typename Expr, typename State, typename Visitor>
            struct apply_lambda_<R, Expr, State, Visitor, false>
              : apply_lambda_aux_<R, Expr, State, Visitor>
            {};

            template<typename R, typename Expr, typename State, typename Visitor>
            struct apply_lambda_<R, Expr, State, Visitor, true>
              : boost::result_of<R(Expr, State, Visitor)>
            {};

            template<typename R, typename... Args, typename Expr, typename State, typename Visitor>
            struct apply_lambda_<R(Args...), Expr, State, Visitor, false>
              : boost::result_of<when<_, R(Args...)>(Expr, State, Visitor)>
            {};

            template<typename R, typename... Args, typename Expr, typename State, typename Visitor>
            struct apply_lambda_<R(*)(Args...), Expr, State, Visitor, false>
              : boost::result_of<when<_, R(*)(Args...)>(Expr, State, Visitor)>
            {};

            // work around GCC bug
            template<typename Tag, typename Args, long N, typename Expr, typename State, typename Visitor>
            struct apply_lambda_<expr<Tag, Args, N>, Expr, State, Visitor, false>
            {
                typedef expr<Tag, Args, N> type;
                typedef void not_applied_;
            };

            template<typename Type, typename... Args>
            typename enable_if<is_aggregate<Type>, Type>::type
            construct_(Args &&... args)
            {
                Type that = { args... };
                return that;
            }

            template<typename Type, typename... Args>
            typename disable_if<is_aggregate<Type>, Type>::type
            construct_(Args &&... args)
            {
                return Type(args...);
            }

            template<typename Type>
            Type construct_()
            {
                return Type();
            }

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
                typedef typename apply_lambda_<Return, Expr, State, Visitor>::type lambda_type;

                // If the result of applying the lambda on the return type is a transform,
                // apply the transform rather than trying to construct it.
                typedef
                    typename mpl::eval_if<
                        proto::detail::is_transform2_<lambda_type>
                      , apply<Expr, State, Visitor, true, lambda_type, Args...>
                      , mpl::identity<lambda_type>
                    >::type
                type;

                static type call(Expr const &expr, State const &state, Visitor &visitor)
                {
                    return apply::call_(expr, state, visitor, proto::detail::is_transform2_<lambda_type>());
                }

            private:
                static type call_(Expr const &expr, State const &state, Visitor &visitor, mpl::false_)
                {
                    return detail::construct_<type>(
                        when<_, Args>()(expr, state, visitor)...
                    );
                }

                static type call_(Expr const &expr, State const &state, Visitor &visitor, mpl::true_)
                {
                    return apply<Expr, State, Visitor, true, lambda_type, Args...>::call(expr, state, visitor);
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

            // BUGBUG makes a temporary
            template<typename Expr, typename State, typename Visitor>
            typename result<when(Expr, State, Visitor)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                return result<when(Expr, State, Visitor)>::call(expr, state, visitor);
            }
        };

        template<typename Grammar, typename Return, typename... Args>
        struct when<Grammar, Return(*)(Args...)>
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
