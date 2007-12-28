///////////////////////////////////////////////////////////////////////////////
/// \file make.hpp
/// Contains definition of the make<> transform.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_TRANSFORM_MAKE_HPP_EAN_12_02_2007
#define BOOST_PROTO_TRANSFORM_MAKE_HPP_EAN_12_02_2007

#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/aux_/has_type.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/traits.hpp>
#include <boost/xpressive/proto/args.hpp>
#include <boost/xpressive/proto/detail/as_lvalue.hpp>

namespace boost { namespace proto
{

    namespace transform
    {
        namespace detail
        {
            using proto::detail::as_lvalue;

            #ifdef BOOST_HAS_VARIADIC_TMPL
            template<typename... T>
            #else
            template<BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(BOOST_PROTO_MAX_ARITY, typename A, void)>
            #endif
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

            #ifdef BOOST_HAS_VARIADIC_TMPL
            template<typename T, typename... Args>
            struct nested_type_if<T, typelist<Args...>, typename typelist<typename Args::not_applied_...>::type>
            {
                typedef T type;
                typedef void not_applied_;
            };
            #endif

            template<typename R, typename Expr, typename State, typename Visitor
                , bool IsTransform = is_callable<R>::value
            >
            struct make_if_;

            template<typename R, typename Expr, typename State, typename Visitor>
            struct make_
            {
                typedef R type;
                typedef void not_applied_;
            };

            #ifdef BOOST_HAS_VARIADIC_TMPL
            template<template<typename...> class R, typename... Args, typename Expr, typename State, typename Visitor>
            struct make_<R<Args...>, Expr, State, Visitor>
              : nested_type_if<
                    R<typename make_if_<Args, Expr, State, Visitor>::type...>
                  , typelist<make_if_<Args, Expr, State, Visitor>...>
                >
            {};
            #endif
            
            template<typename R, typename Expr, typename State, typename Visitor>
            struct make_if_<R, Expr, State, Visitor, false>
              : make_<R, Expr, State, Visitor>
            {};

            template<typename R, typename Expr, typename State, typename Visitor>
            struct make_if_<R, Expr, State, Visitor, true>
              : remove_const<typename remove_reference<
                    typename boost::result_of<R(Expr, State, Visitor)>::type
                >::type>
            {};

            #ifdef BOOST_HAS_VARIADIC_TMPL
            template<typename R, typename... Args, typename Expr, typename State, typename Visitor>
            struct make_if_<R(Args...), Expr, State, Visitor, false>
              : remove_const<typename remove_reference<
                    typename boost::result_of<when<_, R(Args...)>(Expr, State, Visitor)>::type
                >::type>
            {};

            template<typename R, typename... Args, typename Expr, typename State, typename Visitor>
            struct make_if_<R(*)(Args...), Expr, State, Visitor, false>
              : remove_const<typename remove_reference<
                    typename boost::result_of<when<_, R(Args...)>(Expr, State, Visitor)>::type
                >::type>
            {};
            #endif

            template<typename Type, bool IsAggregate = is_aggregate<Type>::value>
            struct construct_
            {
                typedef Type result_type;

                #ifdef BOOST_HAS_VARIADIC_TMPL
                template<typename... Args>
                Type operator()(Args &... args) const
                {
                    return Type(args...);
                }
                #else
                Type operator()() const
                {
                    return Type();
                }

                #define TMP(Z, N, DATA)                                                             \
                template<BOOST_PP_ENUM_PARAMS_Z(Z, N, typename A)>                                  \
                Type operator()(BOOST_PP_ENUM_BINARY_PARAMS_Z(Z, N, A, &a)) const                   \
                {                                                                                   \
                    return Type(BOOST_PP_ENUM_PARAMS_Z(Z, N, a));                                   \
                }
                BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_INC(BOOST_PROTO_MAX_ARITY), TMP, ~)
                #undef TMP
                #endif
            };

            template<typename Type>
            struct construct_<Type, true>
            {
                typedef Type result_type;

                Type operator()() const
                {
                    return Type();
                }

                #ifdef BOOST_HAS_VARIADIC_TMPL
                template<typename... Args>
                Type operator()(Args &... args) const
                {
                    Type that = {args...};
                    return that;
                }
                #else
                #define TMP(Z, N, DATA)                                                             \
                template<BOOST_PP_ENUM_PARAMS_Z(Z, N, typename A)>                                  \
                Type operator()(BOOST_PP_ENUM_BINARY_PARAMS_Z(Z, N, A, &a)) const                   \
                {                                                                                   \
                    Type that = {BOOST_PP_ENUM_PARAMS_Z(Z, N, a)};                                  \
                    return that;                                                                    \
                }
                BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_INC(BOOST_PROTO_MAX_ARITY), TMP, ~)
                #undef TMP
                #endif
            };

            template<typename T, typename A, long N>
            struct construct_<expr<T, A, N>, true>
            {
                typedef expr<T, A, N> result_type;

                #ifdef BOOST_HAS_VARIADIC_TMPL
                template<typename... Args>
                result_type operator()(Args &... args) const
                {
                    return proto::construct<result_type>(args...);
                }
                #else
                #define TMP(Z, N, DATA)                                                             \
                template<BOOST_PP_ENUM_PARAMS_Z(Z, N, typename A)>                                  \
                result_type operator()(BOOST_PP_ENUM_BINARY_PARAMS_Z(Z, N, A, &a)) const            \
                {                                                                                   \
                    return proto::construct<result_type>(BOOST_PP_ENUM_PARAMS_Z(Z, N, a));          \
                }
                BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_INC(BOOST_PROTO_MAX_ARITY), TMP, ~)
                #undef TMP
                #endif
            };

            #ifdef BOOST_HAS_VARIADIC_TMPL
            template<typename Type, typename... Args>
            Type construct(Args &... args)
            {
                return construct_<Type>()(args...);
            }
            #else
            #define TMP(Z, N, DATA)                                                                 \
            template<typename Type BOOST_PP_ENUM_TRAILING_PARAMS_Z(Z, N, typename A)>               \
            Type construct(BOOST_PP_ENUM_BINARY_PARAMS_Z(Z, N, A, &a))                              \
            {                                                                                       \
                return construct_<Type>()(BOOST_PP_ENUM_PARAMS_Z(Z, N, a));                         \
            }
            BOOST_PP_REPEAT(BOOST_PROTO_MAX_ARITY, TMP, ~)
            #undef TMP
            #endif
        }

        template<typename Fun>
        struct make : callable
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
              : detail::make_<Fun, Expr, State, Visitor>
            {};

            template<typename Expr, typename State, typename Visitor>
            typename result<make(Expr const &, State const &, Visitor &)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                typedef typename result<make(Expr const &, State const &, Visitor &)>::type result_type;
                return result_type();
            }
        };

        #ifdef BOOST_HAS_VARIADIC_TMPL
        template<typename Return, typename... Args>
        struct make<Return(Args...)> : callable
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
              : detail::make_<Return, Expr, State, Visitor>
            {};

            template<typename Expr, typename State, typename Visitor>
            typename result<make(Expr const &, State const &, Visitor &)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                typedef typename result<make(Expr const &, State const &, Visitor &)>::type result_type;
                return detail::construct<result_type>(
                    detail::as_lvalue(when<_, Args>()(expr, state, visitor))...
                );
            }
        };
        #else
        #include <boost/xpressive/proto/detail/make.hpp>
        #endif
    }

    template<typename Fun>
    struct is_callable<transform::make<Fun> >
      : mpl::true_
    {};

}}

#endif
