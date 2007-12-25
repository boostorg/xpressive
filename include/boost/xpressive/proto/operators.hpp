///////////////////////////////////////////////////////////////////////////////
/// \file operators.hpp
/// Contains the definition of proto's operator overloads
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_OPERATORS_HPP_EAN_10_28_2007
#define BOOST_PROTO_OPERATORS_HPP_EAN_10_28_2007

#include <boost/mpl/assert.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/make_expr.hpp>
#include <boost/xpressive/proto/detail/define.hpp>

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

            template<typename Arg, typename Trait, typename Enable = void>
            struct arg_weight
            {
                BOOST_STATIC_CONSTANT(int, value = 1 + Trait::value);
            };

            template<typename Arg, typename Trait>
            struct arg_weight<Arg, Trait, typename Arg::proto_is_expr_>
            {
                BOOST_STATIC_CONSTANT(int, value = 0);
            };

            template<typename Arg, typename Trait>
            struct arg_weight<Arg &, Trait, typename Arg::proto_is_expr_>
            {
                BOOST_STATIC_CONSTANT(int, value = 0);
            };

            template<typename Domain, typename Trait, typename Arg, typename Expr>
            struct enable_unary
              : boost::enable_if<
                    boost::mpl::and_<Trait, boost::proto::matches<Expr, typename Domain::grammar> >
                  , Expr
                >
            {};

            template<typename Trait, typename Arg, typename Expr>
            struct enable_unary<deduce_domain, Trait, Arg, Expr>
              : boost::enable_if<
                    boost::mpl::and_<
                        Trait
                      , boost::proto::matches<Expr, typename domain_of<Arg>::type::grammar>
                    >
                  , Expr
                >
            {};

            template<typename Trait, typename Arg, typename Expr>
            struct enable_unary<default_domain, Trait, Arg, Expr>
              : boost::enable_if<Trait, Expr>
            {};

            template<typename Domain, typename Trait1, typename Arg1, typename Trait2, typename Arg2, typename Expr>
            struct enable_binary
              : boost::enable_if<
                    boost::mpl::and_<
                        mpl::bool_<(3 <= (arg_weight<Arg1, Trait1>::value + arg_weight<Arg2, Trait2>::value))>
                      , boost::proto::matches<Expr, typename Domain::grammar>
                    >
                  , Expr
                >
            {};

            template<typename Trait1, typename Arg1, typename Trait2, typename Arg2, typename Expr>
            struct enable_binary<deduce_domain, Trait1, Arg1, Trait2, Arg2, Expr>
              : boost::enable_if<
                    boost::mpl::and_<
                        mpl::bool_<(3 <= (arg_weight<Arg1, Trait1>::value + arg_weight<Arg2, Trait2>::value))>
                      , boost::proto::matches<Expr, typename unify_domain<Arg1, Arg2>::type::grammar>
                    >
                  , Expr
                >
            {};

            template<typename Trait1, typename Arg1, typename Trait2, typename Arg2, typename Expr>
            struct enable_binary<default_domain, Trait1, Arg1, Trait2, Arg2, Expr>
              : boost::enable_if_c<
                    (3 <= (arg_weight<Arg1, Trait1>::value + arg_weight<Arg2, Trait2>::value))
                  , Expr
                >
            {};

        }

    #define BOOST_PROTO_UNARY_OP_IS_POSTFIX_0
    #define BOOST_PROTO_UNARY_OP_IS_POSTFIX_1 , int

#ifdef BOOST_HAS_RVALUE_REFS
    #define BOOST_PROTO_DEFINE_UNARY_OPERATOR(OP, TAG, POST)                                        \
        template<typename A>                                                                        \
        typename detail::generate_if<                                                               \
            true                                                                                    \
          , UNREF(A)::proto_domain                                                                  \
          , expr<TAG, args<                                                                         \
                typename result_of::as_expr_ref<A, UNREF(A)::proto_domain>::type                    \
            > >                                                                                     \
        >::type const                                                                               \
        operator OP(A &&a BOOST_PROTO_UNARY_OP_IS_POSTFIX_ ## POST)                                 \
        {                                                                                           \
            typedef UNREF(A)::proto_domain D;                                                       \
            expr<TAG, args<                                                                         \
                typename result_of::as_expr_ref<A, D>::type                                         \
            > > that = {{result_of::as_expr_ref<A, D>::call(a)}};                                   \
            return D::make(that);                                                                   \
        }

    #define BOOST_PROTO_DEFINE_BINARY_OPERATOR(OP, TAG)                                             \
        template<typename A, typename B>                                                            \
        typename detail::generate_if<                                                               \
            result_of::is_expr<A>::value || result_of::is_expr<B>::value                            \
          , typename detail::unify_domain<A, B>::type                                               \
          , expr<TAG, args<                                                                         \
                typename result_of::as_expr_ref<A, typename detail::unify_domain<A, B>::type>::type \
              , typename result_of::as_expr_ref<B, typename detail::unify_domain<A, B>::type>::type \
            > >                                                                                     \
        >::type const                                                                               \
        operator OP(A &&a, B &&b)                                                                   \
        {                                                                                           \
            typedef typename detail::unify_domain<A, B>::type D;                                    \
            expr<TAG, args<                                                                         \
                typename result_of::as_expr_ref<A, D>::type                                         \
              , typename result_of::as_expr_ref<B, D>::type                                         \
            > > that = {                                                                            \
                {result_of::as_expr_ref<A, D>::call(a)                                              \
              , {result_of::as_expr_ref<B, D>::call(b)}}                                            \
            };                                                                                      \
            return D::make(that);                                                                   \
        }
#else
    #define BOOST_PROTO_DEFINE_UNARY_OPERATOR(OP, TAG, POST)                                        \
        template<typename A>                                                                        \
        typename detail::generate_if<                                                               \
            true                                                                                    \
          , typename A::proto_domain                                                                \
          , expr<TAG, args<                                                                         \
                typename result_of::as_expr_ref<A &, typename A::proto_domain>::type                \
            > >                                                                                     \
        >::type const                                                                               \
        operator OP(A &a BOOST_PROTO_UNARY_OP_IS_POSTFIX_ ## POST)                                  \
        {                                                                                           \
            typedef typename A::proto_domain D;                                                     \
            expr<TAG, args<                                                                         \
                typename result_of::as_expr_ref<A &, D>::type                                       \
            > > that = {{result_of::as_expr_ref<A &, D>::call(a)}};                                 \
            return D::make(that);                                                                   \
        }                                                                                           \
        template<typename A>                                                                        \
        typename detail::generate_if<                                                               \
            true                                                                                    \
          , typename A::proto_domain                                                                \
          , expr<TAG, args<                                                                         \
                typename result_of::as_expr_ref<A const &, typename A::proto_domain>::type          \
            > >                                                                                     \
        >::type const                                                                               \
        operator OP(A const &a BOOST_PROTO_UNARY_OP_IS_POSTFIX_ ## POST)                            \
        {                                                                                           \
            typedef typename A::proto_domain D;                                                     \
            expr<TAG, args<                                                                         \
                typename result_of::as_expr_ref<A const &, D>::type                                 \
            > > that = {{result_of::as_expr_ref<A const &, D>::call(a)}};                           \
            return D::make(that);                                                                   \
        }

    #define BOOST_PROTO_DEFINE_BINARY_OPERATOR(OP, TAG)                                             \
        template<typename A, typename B>                                                            \
        typename detail::generate_if<                                                               \
            result_of::is_expr<A>::value || result_of::is_expr<B>::value                            \
          , typename detail::unify_domain<A, B>::type                                               \
          , expr<TAG, args<                                                                         \
                typename result_of::as_expr_ref<A &, typename detail::unify_domain<A, B>::type>::type\
              , typename result_of::as_expr_ref<B &, typename detail::unify_domain<A, B>::type>::type\
            > >                                                                                     \
        >::type const                                                                               \
        operator OP(A &a, B &b)                                                                     \
        {                                                                                           \
            typedef typename detail::unify_domain<A, B>::type D;                                    \
            expr<TAG, args<                                                                         \
                typename result_of::as_expr_ref<A &, D>::type                                       \
              , typename result_of::as_expr_ref<B &, D>::type                                       \
            > > that = {                                                                            \
                {result_of::as_expr_ref<A &, D>::call(a)                                            \
              , {result_of::as_expr_ref<B &, D>::call(b)}}                                          \
            };                                                                                      \
            return D::make(that);                                                                   \
        }                                                                                           \
        template<typename A, typename B>                                                            \
        typename detail::generate_if<                                                               \
            result_of::is_expr<A>::value || result_of::is_expr<B>::value                            \
          , typename detail::unify_domain<A, B>::type                                               \
          , expr<TAG, args<                                                                         \
                typename result_of::as_expr_ref<A &, typename detail::unify_domain<A, B>::type>::type\
              , typename result_of::as_expr_ref<B const &, typename detail::unify_domain<A, B>::type>::type\
            > >                                                                                     \
        >::type const                                                                               \
        operator OP(A &a, B const &b)                                                               \
        {                                                                                           \
            typedef typename detail::unify_domain<A, B>::type D;                                    \
            expr<TAG, args<                                                                         \
                typename result_of::as_expr_ref<A &, D>::type                                       \
              , typename result_of::as_expr_ref<B const &, D>::type                                 \
            > > that = {                                                                            \
                {result_of::as_expr_ref<A &, D>::call(a)                                            \
              , {result_of::as_expr_ref<B const &, D>::call(b)}}                                    \
            };                                                                                      \
            return D::make(that);                                                                   \
        }                                                                                           \
        template<typename A, typename B>                                                            \
        typename detail::generate_if<                                                               \
            result_of::is_expr<A>::value || result_of::is_expr<B>::value                            \
          , typename detail::unify_domain<A, B>::type                                               \
          , expr<TAG, args<                                                                         \
                typename result_of::as_expr_ref<A const &, typename detail::unify_domain<A, B>::type>::type\
              , typename result_of::as_expr_ref<B &, typename detail::unify_domain<A, B>::type>::type\
            > >                                                                                     \
        >::type const                                                                               \
        operator OP(A const &a, B &b)                                                               \
        {                                                                                           \
            typedef typename detail::unify_domain<A, B>::type D;                                    \
            expr<TAG, args<                                                                         \
                typename result_of::as_expr_ref<A const &, D>::type                                 \
              , typename result_of::as_expr_ref<B &, D>::type                                       \
            > > that = {                                                                            \
                {result_of::as_expr_ref<A const &, D>::call(a)                                      \
              , {result_of::as_expr_ref<B &, D>::call(b)}}                                          \
            };                                                                                      \
            return D::make(that);                                                                   \
        }                                                                                           \
        template<typename A, typename B>                                                            \
        typename detail::generate_if<                                                               \
            result_of::is_expr<A>::value || result_of::is_expr<B>::value                            \
          , typename detail::unify_domain<A, B>::type                                               \
          , expr<TAG, args<                                                                         \
                typename result_of::as_expr_ref<A const &, typename detail::unify_domain<A, B>::type>::type\
              , typename result_of::as_expr_ref<B const &, typename detail::unify_domain<A, B>::type>::type\
            > >                                                                                     \
        >::type const                                                                               \
        operator OP(A const &a, B const &b)                                                         \
        {                                                                                           \
            typedef typename detail::unify_domain<A, B>::type D;                                    \
            expr<TAG, args<                                                                         \
                typename result_of::as_expr_ref<A const &, D>::type                                 \
              , typename result_of::as_expr_ref<B const &, D>::type                                 \
            > > that = {                                                                            \
                {result_of::as_expr_ref<A const &, D>::call(a)                                      \
              , {result_of::as_expr_ref<B const &, D>::call(b)}}                                    \
            };                                                                                      \
            return D::make(that);                                                                   \
        }
#endif

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

    #undef BOOST_PROTO_DEFINE_UNARY_OPERATOR
    #undef BOOST_PROTO_DEFINE_BINARY_OPERATOR

    #ifdef BOOST_HAS_RVALUE_REFS
        template<typename A, typename B, typename C>
        typename result_of::make_expr_ref<tag::if_else_, A, B, C>::type const
        if_else(A &&a, B &&b, C &&c)
        {
            return result_of::make_expr_ref<tag::if_else_, A, B, C>::call(a, b, c);
        }
    #else
        template<typename A, typename B, typename C>
        typename result_of::make_expr_ref<tag::if_else_, A &, B &, C &>::type const
        if_else(A &a, B &b, C &c)
        {
            return result_of::make_expr_ref<tag::if_else_, A &, B &, C &>::call(a, b, c);
        }
        template<typename A, typename B, typename C>
        typename result_of::make_expr_ref<tag::if_else_, A &, B &, C const &>::type const
        if_else(A &a, B &b, C const &c)
        {
            return result_of::make_expr_ref<tag::if_else_, A &, B &, C const &>::call(a, b, c);
        }
        template<typename A, typename B, typename C>
        typename result_of::make_expr_ref<tag::if_else_, A &, B const &, C &>::type const
        if_else(A &a, B const &b, C &c)
        {
            return result_of::make_expr_ref<tag::if_else_, A &, B const &, C &>::call(a, b, c);
        }
        template<typename A, typename B, typename C>
        typename result_of::make_expr_ref<tag::if_else_, A &, B const &, C const &>::type const
        if_else(A &a, B const &b, C const &c)
        {
            return result_of::make_expr_ref<tag::if_else_, A &, B const &, C const &>::call(a, b, c);
        }
        template<typename A, typename B, typename C>
        typename result_of::make_expr_ref<tag::if_else_, A const &, B &, C &>::type const
        if_else(A const &a, B &b, C &c)
        {
            return result_of::make_expr_ref<tag::if_else_, A const &, B &, C &>::call(a, b, c);
        }
        template<typename A, typename B, typename C>
        typename result_of::make_expr_ref<tag::if_else_, A const &, B &, C const &>::type const
        if_else(A const &a, B &b, C const &c)
        {
            return result_of::make_expr_ref<tag::if_else_, A const &, B &, C const &>::call(a, b, c);
        }
        template<typename A, typename B, typename C>
        typename result_of::make_expr_ref<tag::if_else_, A const &, B const &, C &>::type const
        if_else(A const &a, B const &b, C &c)
        {
            return result_of::make_expr_ref<tag::if_else_, A const &, B const &, C &>::call(a, b, c);
        }
        template<typename A, typename B, typename C>
        typename result_of::make_expr_ref<tag::if_else_, A const &, B const &, C const &>::type const
        if_else(A const &a, B const &b, C const &c)
        {
            return result_of::make_expr_ref<tag::if_else_, A const &, B const &, C const &>::call(a, b, c);
        }
    #endif

    }

    using exprns_::if_else;

#ifdef BOOST_HAS_RVALUE_REFS
#define BOOST_PROTO_DEFINE_UNARY_OPERATOR(OP, TAG, TRAIT, DOMAIN, POST)                             \
    template<typename Arg>                                                                          \
    typename boost::proto::exprns_::detail::enable_unary<                                           \
        DOMAIN                                                                                      \
      , TRAIT<BOOST_PROTO_UNCVREF(Arg)>, Arg                                                        \
      , typename boost::proto::result_of::make_expr_ref<TAG, DOMAIN, Arg>::type                     \
    >::type const                                                                                   \
    operator OP(Arg &&arg BOOST_PROTO_UNARY_OP_IS_POSTFIX_ ## POST)                                 \
    {                                                                                               \
        return boost::proto::result_of::make_expr_ref<TAG, DOMAIN, Arg>::call(arg);                 \
    }

#define BOOST_PROTO_DEFINE_BINARY_OPERATOR(OP, TAG, TRAIT, DOMAIN)                                  \
    template<typename Left, typename Right>                                                         \
    typename boost::proto::exprns_::detail::enable_binary<                                          \
        DOMAIN                                                                                      \
      , TRAIT<BOOST_PROTO_UNCVREF(Left)>, Left                                                      \
      , TRAIT<BOOST_PROTO_UNCVREF(Right)>, Right                                                    \
      , typename boost::proto::result_of::make_expr_ref<TAG, DOMAIN, Left, Right>::type             \
    >::type const                                                                                   \
    operator OP(Left &&left, Right &&right)                                                         \
    {                                                                                               \
        return boost::proto::result_of::make_expr_ref<TAG, DOMAIN, Left, Right>                     \
            ::call(left, right);                                                                    \
    }
#else
#define BOOST_PROTO_DEFINE_UNARY_OPERATOR(OP, TAG, TRAIT, DOMAIN, POST)                             \
    template<typename Arg>                                                                          \
    typename boost::proto::exprns_::detail::enable_unary<                                           \
        DOMAIN                                                                                      \
      , TRAIT<Arg>, Arg &                                                                           \
      , typename boost::proto::result_of::make_expr_ref<TAG, DOMAIN, Arg &>::type                   \
    >::type const                                                                                   \
    operator OP(Arg &arg BOOST_PROTO_UNARY_OP_IS_POSTFIX_ ## POST)                                  \
    {                                                                                               \
        return boost::proto::result_of::make_expr_ref<TAG, DOMAIN, Arg &>::call(arg);               \
    }                                                                                               \
    template<typename Arg>                                                                          \
    typename boost::proto::exprns_::detail::enable_unary<                                           \
        DOMAIN                                                                                      \
      , TRAIT<Arg>, Arg const &                                                                     \
      , typename boost::proto::result_of::make_expr_ref<TAG, DOMAIN, Arg const &>::type             \
    >::type const                                                                                   \
    operator OP(Arg const &arg BOOST_PROTO_UNARY_OP_IS_POSTFIX_ ## POST)                            \
    {                                                                                               \
        return boost::proto::result_of::make_expr_ref<TAG, DOMAIN, Arg const &>::call(arg);         \
    }

#define BOOST_PROTO_DEFINE_BINARY_OPERATOR(OP, TAG, TRAIT, DOMAIN)                                  \
    template<typename Left, typename Right>                                                         \
    typename boost::proto::exprns_::detail::enable_binary<                                          \
        DOMAIN                                                                                      \
      , TRAIT<Left>, Left &                                                                         \
      , TRAIT<Right>, Right &                                                                       \
      , typename boost::proto::result_of::make_expr_ref<TAG, DOMAIN, Left &, Right &>::type         \
    >::type const                                                                                   \
    operator OP(Left &left, Right &right)                                                           \
    {                                                                                               \
        return boost::proto::result_of::make_expr_ref<TAG, DOMAIN, Left &, Right &>                 \
            ::call(left, right);                                                                    \
    }                                                                                               \
    template<typename Left, typename Right>                                                         \
    typename boost::proto::exprns_::detail::enable_binary<                                          \
        DOMAIN                                                                                      \
      , TRAIT<Left>, Left &                                                                         \
      , TRAIT<Right>, Right const &                                                                 \
      , typename boost::proto::result_of::make_expr_ref<TAG, DOMAIN, Left &, Right const &>::type   \
    >::type const                                                                                   \
    operator OP(Left &left, Right const &right)                                                     \
    {                                                                                               \
        return boost::proto::result_of::make_expr_ref<TAG, DOMAIN, Left &, Right const &>           \
            ::call(left, right);                                                                    \
    }                                                                                               \
    template<typename Left, typename Right>                                                         \
    typename boost::proto::exprns_::detail::enable_binary<                                          \
        DOMAIN                                                                                      \
      , TRAIT<Left>, Left const &                                                                   \
      , TRAIT<Right>, Right &                                                                       \
      , typename boost::proto::result_of::make_expr_ref<TAG, DOMAIN, Left const &, Right &>::type   \
    >::type const                                                                                   \
    operator OP(Left const &left, Right &right)                                                     \
    {                                                                                               \
        return boost::proto::result_of::make_expr_ref<TAG, DOMAIN, Left const &, Right &>           \
            ::call(left, right);                                                                    \
    }                                                                                               \
    template<typename Left, typename Right>                                                         \
    typename boost::proto::exprns_::detail::enable_binary<                                          \
        DOMAIN                                                                                      \
      , TRAIT<Left>, Left const &                                                                   \
      , TRAIT<Right>, Right const &                                                                 \
      , typename boost::proto::result_of::make_expr_ref<TAG, DOMAIN, Left const &, Right const &>::type\
    >::type const                                                                                   \
    operator OP(Left const &left, Right const &right)                                               \
    {                                                                                               \
        return boost::proto::result_of::make_expr_ref<TAG, DOMAIN, Left const &, Right const &>     \
            ::call(left, right);                                                                    \
    }
#endif

#define BOOST_PROTO_DEFINE_OPERATORS(TRAIT, DOMAIN)                                                 \
    BOOST_PROTO_DEFINE_UNARY_OPERATOR(+, boost::proto::tag::posit, TRAIT, DOMAIN, 0)                \
    BOOST_PROTO_DEFINE_UNARY_OPERATOR(-, boost::proto::tag::negate, TRAIT, DOMAIN, 0)               \
    BOOST_PROTO_DEFINE_UNARY_OPERATOR(*, boost::proto::tag::dereference, TRAIT, DOMAIN, 0)          \
    BOOST_PROTO_DEFINE_UNARY_OPERATOR(~, boost::proto::tag::complement, TRAIT, DOMAIN, 0)           \
    BOOST_PROTO_DEFINE_UNARY_OPERATOR(&, boost::proto::tag::address_of, TRAIT, DOMAIN, 0)           \
    BOOST_PROTO_DEFINE_UNARY_OPERATOR(!, boost::proto::tag::logical_not, TRAIT, DOMAIN, 0)          \
    BOOST_PROTO_DEFINE_UNARY_OPERATOR(++, boost::proto::tag::pre_inc, TRAIT, DOMAIN, 0)             \
    BOOST_PROTO_DEFINE_UNARY_OPERATOR(--, boost::proto::tag::pre_dec, TRAIT, DOMAIN, 0)             \
    BOOST_PROTO_DEFINE_UNARY_OPERATOR(++, boost::proto::tag::post_inc, TRAIT, DOMAIN, 1)            \
    BOOST_PROTO_DEFINE_UNARY_OPERATOR(--, boost::proto::tag::post_dec, TRAIT, DOMAIN, 1)            \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(<<, boost::proto::tag::shift_left, TRAIT, DOMAIN)            \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(>>, boost::proto::tag::shift_right, TRAIT, DOMAIN)           \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(*, boost::proto::tag::multiplies, TRAIT, DOMAIN)             \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(/, boost::proto::tag::divides, TRAIT, DOMAIN)                \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(%, boost::proto::tag::modulus, TRAIT, DOMAIN)                \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(+, boost::proto::tag::plus, TRAIT, DOMAIN)                   \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(-, boost::proto::tag::minus, TRAIT, DOMAIN)                  \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(<, boost::proto::tag::less, TRAIT, DOMAIN)                   \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(>, boost::proto::tag::greater, TRAIT, DOMAIN)                \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(<=, boost::proto::tag::less_equal, TRAIT, DOMAIN)            \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(>=, boost::proto::tag::greater_equal, TRAIT, DOMAIN)         \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(==, boost::proto::tag::equal_to, TRAIT, DOMAIN)              \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(!=, boost::proto::tag::not_equal_to, TRAIT, DOMAIN)          \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(||, boost::proto::tag::logical_or, TRAIT, DOMAIN)            \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(&&, boost::proto::tag::logical_and, TRAIT, DOMAIN)           \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(&, boost::proto::tag::bitwise_and, TRAIT, DOMAIN)            \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(|, boost::proto::tag::bitwise_or, TRAIT, DOMAIN)             \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(^, boost::proto::tag::bitwise_xor, TRAIT, DOMAIN)            \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(BOOST_PP_COMMA(), boost::proto::tag::comma, TRAIT, DOMAIN)   \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(->*, boost::proto::tag::mem_ptr, TRAIT, DOMAIN)              \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(<<=, boost::proto::tag::shift_left_assign, TRAIT, DOMAIN)    \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(>>=, boost::proto::tag::shift_right_assign, TRAIT, DOMAIN)   \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(*=, boost::proto::tag::multiplies_assign, TRAIT, DOMAIN)     \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(/=, boost::proto::tag::divides_assign, TRAIT, DOMAIN)        \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(%=, boost::proto::tag::modulus_assign, TRAIT, DOMAIN)        \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(+=, boost::proto::tag::plus_assign, TRAIT, DOMAIN)           \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(-=, boost::proto::tag::minus_assign, TRAIT, DOMAIN)          \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(&=, boost::proto::tag::bitwise_and_assign, TRAIT, DOMAIN)    \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(|=, boost::proto::tag::bitwise_or_assign, TRAIT, DOMAIN)     \
    BOOST_PROTO_DEFINE_BINARY_OPERATOR(^=, boost::proto::tag::bitwise_xor_assign, TRAIT, DOMAIN)    \
    /**/

    template<typename T>
    struct is_extension
      : mpl::false_
    {};

    namespace exops
    {
//        BOOST_PROTO_DEFINE_OPERATORS(is_extension, default_domain)
        using proto::if_else;
    }

}}

#include <boost/xpressive/proto/detail/undef.hpp>

#endif
