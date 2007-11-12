///////////////////////////////////////////////////////////////////////////////
/// \file transform.hpp
/// Definition of case_ transform.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO3_TRANSFORM_CASE_HPP_EAN_10_29_2007
#define BOOST_PROTO3_TRANSFORM_CASE_HPP_EAN_10_29_2007

#include <boost/mpl/bool.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/mpl/aux_/has_type.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/xpressive/proto3/proto_fwd.hpp>
#include <boost/xpressive/proto3/transform/bind.hpp>
#include <boost/xpressive/proto3/transform/apply.hpp>

namespace boost { namespace proto
{
    namespace transform
    {
        namespace detail
        {
            template<typename... T>
            struct typelist
            {
                typedef void type;
            };

            template<typename T>
            struct is_aggregate
              : is_pod<T>
            {};

            template<typename Tag, typename Args, long N>
            struct is_aggregate<expr<Tag, Args, N> >
              : mpl::true_
            {};

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
                , typename Category = typename transform_category<R>::type
            >
            struct apply_lambda_; // function-style transforms cannot be part of lambdas

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
            struct apply_lambda_<R, Expr, State, Visitor, no_transform>
              : apply_lambda_aux_<R, Expr, State, Visitor>
            {};

            template<typename R, typename Expr, typename State, typename Visitor>
            struct apply_lambda_<R, Expr, State, Visitor, raw_transform>
              : R::template apply<Expr, State, Visitor>
            {};

            template<typename R, typename... Args, typename Expr, typename State, typename Visitor>
            struct apply_lambda_<R(Args...), Expr, State, Visitor, no_transform>
              : case_<_, R(Args...)>::template apply<Expr, State, Visitor>
            {};

            template<typename R, typename... Args, typename Expr, typename State, typename Visitor>
            struct apply_lambda_<R(*)(Args...), Expr, State, Visitor, no_transform>
              : case_<_, R(*)(Args...)>::template apply<Expr, State, Visitor>
            {};

            // work around GCC bug
            template<typename Tag, typename Args, long N, typename Expr, typename State, typename Visitor>
            struct apply_lambda_<expr<Tag, Args, N>, Expr, State, Visitor, no_transform>
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
              , typename TransformCategory
              , typename Return
              , typename... Args
            >
            struct apply_
            {
                typedef typename apply_lambda_<Return, Expr, State, Visitor>::type type;

                static type call(Expr const &expr, State const &state, Visitor &visitor)
                {
                    return detail::construct_<type>(
                        case_<_, Args>::call(expr, state, visitor)...
                    );
                }
            };

            template<
                typename Expr
              , typename State
              , typename Visitor
              , typename Return
              , typename... Args
            >
            struct apply_<Expr, State, Visitor, function_transform, Return, Args...>
            {
                typedef typename bind<Return, Args...>::template apply<Expr, State, Visitor>::type type;

                static type call(Expr const &expr, State const &state, Visitor &visitor)
                {
                    return bind<Return, Args...>::call(expr, state, visitor);
                }
            };

            template<
                typename Expr
              , typename State
              , typename Visitor
              , typename Return
              , typename... Args
            >
            struct apply_<Expr, State, Visitor, raw_transform, Return, Args...>
            {
                typedef typename transform::apply_<Return, Args...>::template apply<Expr, State, Visitor>::type type;

                static type call(Expr const &expr, State const &state, Visitor &visitor)
                {
                    return transform::apply_<Return, Args...>::call(expr, state, visitor);
                }
            };

        }

        // Simple transform, takes a raw transform and
        // applies it directly.
        template<typename Grammar, typename Fun>
        struct case_
          : Fun
        {
            typedef typename Grammar::proto_base_expr proto_base_expr;
        };

        // Lambda-style transform, takes a raw or function-style
        // transform with arguments and applies it, OR takes a
        // (possibly lambda) type and constructor arguments.
        template<typename Grammar, typename Return, typename... Args>
        struct case_<Grammar, Return(Args...)>
          : raw_transform
        {
            typedef typename Grammar::proto_base_expr proto_base_expr;

            template<typename Expr, typename State, typename Visitor>
            struct apply
              : detail::apply_<Expr, State, Visitor, typename transform_category<Return>::type, Return, Args...>
            {};

            // BUGBUG makes a temporary
            template<typename Expr, typename State, typename Visitor>
            static typename apply<Expr, State, Visitor>::type
            call(Expr const &expr, State const &state, Visitor &visitor)
            {
                return apply<Expr, State, Visitor>::call(expr, state, visitor);
            }
        };

        template<typename Grammar, typename Return, typename... Args>
        struct case_<Grammar, Return(*)(Args...)>
          : raw_transform
        {
            typedef typename Grammar::proto_base_expr proto_base_expr;

            template<typename Expr, typename State, typename Visitor>
            struct apply
              : detail::apply_<Expr, State, Visitor, typename transform_category<Return>::type, Return, Args...>
            {};

            template<typename Expr, typename State, typename Visitor>
            static typename apply<Expr, State, Visitor>::type
            call(Expr const &expr, State const &state, Visitor &visitor)
            {
                return apply<Expr, State, Visitor>::call(expr, state, visitor);
            }
        };

    }

    namespace detail
    {
        template<typename T, typename EnableIf = void>
        struct transform_category2_
        {
            typedef no_transform type;
        };

        template<typename T>
        struct transform_category2_<T, typename T::proto_raw_transform_>
        {
            typedef raw_transform type;
        };

        template<typename T>
        struct transform_category2_<T, typename T::proto_function_transform_>
        {
            typedef function_transform type;
        };

        template<typename T>
        struct transform_category_
          : transform_category2_<T>
        {};

        template<template<typename...> class T, typename... Args>
        struct transform_category_<T<Args...> >
        {
            typedef no_transform type;
        };
    }

    template<typename T>
    struct transform_category
      : proto::detail::transform_category_<T>
    {};

    // work around GCC bug
    template<typename Tag, typename Args, long N>
    struct transform_category<expr<Tag, Args, N> >
    {
        typedef no_transform type;
    };

}}

#endif
