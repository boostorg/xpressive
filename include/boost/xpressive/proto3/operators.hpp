///////////////////////////////////////////////////////////////////////////////
/// \file operators.hpp
/// Contains the definition of proto's operator overloads
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO3_OPERATORS_HPP_EAN_10_28_2007
#define BOOST_PROTO3_OPERATORS_HPP_EAN_10_28_2007

#include <boost/mpl/logical.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/preprocessor/punctuation/comma.hpp>
#include <boost/xpressive/proto3/proto_fwd.hpp>

#define UNREF(X) typename remove_reference<X>::type

namespace boost { namespace proto
{

    namespace exprns_
    {
        namespace detail
        {
            template<bool Pred, typename Domain, typename Expr>
            struct generate_if
            {};

            template<typename Domain, typename Expr>
            struct generate_if<true, Domain, Expr>
              : lazy_enable_if<
                    matches<Expr, typename Domain::grammar>
                  , typename Domain::template apply<Expr>
                >
            {};

            template<typename Expr>
            struct generate_if<true, default_domain, Expr>
            {
                typedef Expr type;
            };

            template<
                typename A
              , typename B
              , typename DA = typename domain_of<A>::type
              , typename DB = typename domain_of<B>::type
            >
            struct unify_domain
            {};

            template<typename A, typename B, typename D>
            struct unify_domain<A, B, D, D>
            {
                typedef D type;
            };

            template<typename A, typename B, typename D>
            struct unify_domain<A, B, default_domain, D>
            {
                typedef D type;
            };

            template<typename A, typename B, typename D>
            struct unify_domain<A, B, D, default_domain>
            {
                typedef D type;
            };

            template<typename A, typename B>
            struct unify_domain<A, B, default_domain, default_domain>
            {
                typedef default_domain type;
            };

        }

    #define BOOST_PROTO_UNARY_OP_IS_POSTFIX_0
    #define BOOST_PROTO_UNARY_OP_IS_POSTFIX_1 , int

        // BUGBUG these are borken because of gcc bug wrt forwarding
        // of built-in temporaries
    #define BOOST_PROTO_DEFINE_UNARY_OPERATOR(OP, TAG, POST)                        \
        template<typename A>                                                        \
        typename detail::generate_if<                                               \
            true                                                                    \
          , UNREF(A)::proto_domain                                                  \
          , expr<TAG, args<                                                         \
                typename result_of::as_arg<A, UNREF(A)::proto_domain>::type         \
            > >                                                                     \
        >::type                                                                     \
        operator OP(A &&a BOOST_PROTO_UNARY_OP_IS_POSTFIX_ ## POST)                 \
        {                                                                           \
            typedef UNREF(A)::proto_domain D;                                       \
            expr<TAG, args<                                                         \
                typename result_of::as_arg<A, D>::type                              \
            > > that = {{a /*proto::as_arg<D>(std::forward<A>(a))*/}};              \
            return D::make(that);                                                   \
        }                                                                           \
        /**/

    #define BOOST_PROTO_DEFINE_BINARY_OPERATOR(OP, TAG)                             \
        template<typename A, typename B>                                            \
        typename detail::generate_if<                                               \
            result_of::is_expr<A>::value || result_of::is_expr<B>::value            \
          , typename detail::unify_domain<A, B>::type                               \
          , expr<TAG, args<                                                         \
                typename result_of::as_arg<A, typename detail::unify_domain<A, B>::type>::type\
              , typename result_of::as_arg<B, typename detail::unify_domain<A, B>::type>::type\
            > >                                                                     \
        >::type                                                                     \
        operator OP(A &&a, B &&b)                                                   \
        {                                                                           \
            typedef typename detail::unify_domain<A, B>::type D;                    \
            expr<TAG, args<                                                         \
                typename result_of::as_arg<A, D>::type                              \
              , typename result_of::as_arg<B, D>::type                              \
            > > that = {{                                                           \
                a /*proto::as_arg<D>(std::forward<A>(a))*/                                \
              , {b /*proto::as_arg<D>(std::forward<B>(b))*/ }                              \
            }};                                                                     \
            return D::make(that);                                                   \
        }                                                                           \
        /**/

        BOOST_PROTO_DEFINE_UNARY_OPERATOR(+, tag::posit, 0)
        BOOST_PROTO_DEFINE_UNARY_OPERATOR(-, tag::negate, 0)
        BOOST_PROTO_DEFINE_UNARY_OPERATOR(*, tag::dereference, 0)
        BOOST_PROTO_DEFINE_UNARY_OPERATOR(~, tag::complement, 0)
        BOOST_PROTO_DEFINE_UNARY_OPERATOR(&, tag::address_of, 0)
        BOOST_PROTO_DEFINE_UNARY_OPERATOR(!, tag::logical_not, 0)
        BOOST_PROTO_DEFINE_UNARY_OPERATOR(++, tag::pre_inc, 0)
        BOOST_PROTO_DEFINE_UNARY_OPERATOR(--, tag::pre_dec, 0)
        BOOST_PROTO_DEFINE_UNARY_OPERATOR(++, tag::post_inc, 1)
        BOOST_PROTO_DEFINE_UNARY_OPERATOR(--, tag::post_dec, 1)

        BOOST_PROTO_DEFINE_BINARY_OPERATOR(<<, tag::shift_left)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(>>, tag::shift_right)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(*, tag::multiplies)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(/, tag::divides)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(%, tag::modulus)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(+, tag::plus)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(-, tag::minus)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(<, tag::less)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(>, tag::greater)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(<=, tag::less_equal)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(>=, tag::greater_equal)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(==, tag::equal_to)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(!=, tag::not_equal_to)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(||, tag::logical_or)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(&&, tag::logical_and)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(&, tag::bitwise_and)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(|, tag::bitwise_or)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(^, tag::bitwise_xor)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(BOOST_PP_COMMA(), tag::comma)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(->*, tag::mem_ptr)

        BOOST_PROTO_DEFINE_BINARY_OPERATOR(<<=, tag::shift_left_assign)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(>>=, tag::shift_right_assign)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(*=, tag::multiplies_assign)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(/=, tag::divides_assign)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(%=, tag::modulus_assign)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(+=, tag::plus_assign)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(-=, tag::minus_assign)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(&=, tag::bitwise_and_assign)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(|=, tag::bitwise_or_assign)
        BOOST_PROTO_DEFINE_BINARY_OPERATOR(^=, tag::bitwise_xor_assign)

    #undef BOOST_PROTO_UNARY_OP_IS_POSTFIX_0
    #undef BOOST_PROTO_UNARY_OP_IS_POSTFIX_1

    #undef BOOST_PROTO_DEFINE_UNARY_OPERATOR
    #undef BOOST_PROTO_DEFINE_BINARY_OPERATOR

    }

}}

#undef UNREF

#endif
