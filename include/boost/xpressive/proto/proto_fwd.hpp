///////////////////////////////////////////////////////////////////////////////
/// \file proto_fwd.hpp
/// Contains proto's forward declarations
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_PROTO_FWD_EAN_10_28_2007
#define BOOST_PROTO_PROTO_FWD_EAN_10_28_2007

#include <climits> // for INT_MAX
#include <boost/config.hpp>
#include <boost/preprocessor.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/long.hpp>
#include <boost/mpl/aux_/template_arity.hpp>
#include <boost/mpl/aux_/lambda_arity_param.hpp>

#ifndef BOOST_PROTO_MAX_ARITY
# define BOOST_PROTO_MAX_ARITY 5
#endif

#ifndef BOOST_PROTO_MAX_LOGICAL_ARITY
# define BOOST_PROTO_MAX_LOGICAL_ARITY 8
#endif

namespace boost { namespace proto
{
    namespace detail
    {
        #if defined(BOOST_HAS_VARIADIC_TMPL) && defined(BOOST_HAS_RVALUE_REFS)
        template<int... I>
        struct indices;

        template<int N, typename T = indices<> >
        struct make_indices;
        #endif
    }

    namespace wildns_
    {
        struct _;
    }

    using wildns_::_;

    namespace generatorns_
    {
        struct default_generator;

        template<template<typename> class Extends>
        struct generator;

        template<template<typename> class Extends>
        struct pod_generator;

        template<typename Generator = default_generator>
        struct by_value_generator;
    }

    using generatorns_::default_generator;
    using generatorns_::generator;
    using generatorns_::pod_generator;
    using generatorns_::by_value_generator;

    namespace domainns_
    {
        template<typename Generator = default_generator, typename Grammar = proto::_>
        struct domain;

        struct default_domain;

        struct deduce_domain;
    }

    using domainns_::domain;
    using domainns_::default_domain;
    using domainns_::deduce_domain;

    namespace argsns_
    {
        #if defined(BOOST_HAS_VARIADIC_TMPL) && defined(BOOST_HAS_RVALUE_REFS)
        template<typename... Args>
        struct cons;

        template<typename... Args>
        struct args;
        #else
        template<BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(BOOST_PROTO_MAX_ARITY, typename A, void), typename Dummy = void>
        struct cons;

        template<BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(BOOST_PROTO_MAX_ARITY, typename A, void), typename Dummy = void>
        struct args;
        #endif

        template<typename Arg>
        struct term;
    }

    using argsns_::args;
    using argsns_::term;

    namespace exprns_
    {
        template<typename Tag, typename Args, long Arity = Args::size>
        struct expr;

        template<typename Expr, typename Derived, typename Domain = default_domain>
        struct extends;

        struct is_proto_expr;
    }

    using exprns_::expr;
    using exprns_::extends;
    using exprns_::is_proto_expr;

    namespace tag
    {
        struct terminal;
        struct posit;
        struct negate;
        struct dereference;
        struct complement;
        struct address_of;
        struct logical_not;
        struct pre_inc;
        struct pre_dec;
        struct post_inc;
        struct post_dec;
        struct shift_left;
        struct shift_right;
        struct multiplies;
        struct divides;
        struct modulus;
        struct plus;
        struct minus;
        struct less;
        struct greater;
        struct less_equal;
        struct greater_equal;
        struct equal_to;
        struct not_equal_to;
        struct logical_or;
        struct logical_and;
        struct bitwise_and;
        struct bitwise_or;
        struct bitwise_xor;
        struct comma;
        struct mem_ptr;
        struct assign;
        struct shift_left_assign;
        struct shift_right_assign;
        struct multiplies_assign;
        struct divides_assign;
        struct modulus_assign;
        struct plus_assign;
        struct minus_assign;
        struct bitwise_and_assign;
        struct bitwise_or_assign;
        struct bitwise_xor_assign;
        struct subscript;
        struct if_else_;
        struct function;

        struct proto_expr;
        struct proto_expr_iterator;
        struct proto_flat_view;
    }

    namespace utility
    {
        template<typename T, typename Domain = default_domain>
        struct literal;
    }

    using utility::literal;

    namespace result_of
    {
        template<typename Expr>
        struct tag_of
        {
            typedef typename Expr::proto_tag type;
        };

        template<typename Expr, long N>
        struct value_at_c;

        template<typename Expr, long N>
        struct arg_c;

        template<typename Expr>
        struct left;

        template<typename Expr>
        struct right;

        template<typename T, typename Domain = default_domain, typename EnableIf = void>
        struct as_expr;

        template<typename T, typename Domain = default_domain, typename EnableIf = void>
        struct as_arg;

        template<typename T, typename EnableIf = void>
        struct is_expr;

        template<typename Expr, typename Grammar>
        struct matches;

        template<typename T, typename EnableIf = void>
        struct is_domain;

        template<typename T, typename EnableIf = void>
        struct domain_of;

        template<typename Tag, typename DomainOrSequence, typename SequenceOrVoid = void, typename EnableIf = void>
        struct unpack_arg;

        template<typename Tag, typename DomainOrSequence, typename SequenceOrVoid = void, typename EnableIf = void>
        struct unpack_expr;

        #if defined(BOOST_HAS_VARIADIC_TMPL) && defined(BOOST_HAS_RVALUE_REFS)
        template<typename Tag, typename... Args>
        struct make_arg;

        template<typename Tag, typename... Args>
        struct make_expr;
        #else
        template<
            typename Tag
            BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(
                BOOST_PROTO_MAX_ARITY
              , typename A
              , = void BOOST_PP_INTERCEPT
            )
        >
        struct make_arg;

        template<
            typename Tag
            BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(
                BOOST_PROTO_MAX_ARITY
              , typename A
              , = void BOOST_PP_INTERCEPT
            )
        >
        struct make_expr;
        #endif
    }

    using result_of::matches;
    using result_of::tag_of;
    using result_of::is_domain;
    using result_of::domain_of;
	using result_of::is_expr;

    template<long N, typename Expr>
    typename result_of::arg_c<Expr, N>::type
    arg_c(Expr &expr);

    template<long N, typename Expr>
    typename result_of::arg_c<Expr const, N>::type
    arg_c(Expr const &expr);

    #if defined(BOOST_HAS_VARIADIC_TMPL) && defined(BOOST_HAS_RVALUE_REFS)
    template<typename T>
    typename result_of::as_arg<T>::type
    as_arg(T &&t);
    #else
    template<typename T>
    typename result_of::as_arg<T &>::type
    as_arg(T &t);

    template<typename T>
    typename result_of::as_arg<T const &>::type
    as_arg(T const &t);
    #endif

    namespace op
    {
        template<typename T> struct terminal;
        template<typename T> struct posit;
        template<typename T> struct negate;
        template<typename T> struct dereference;
        template<typename T> struct complement;
        template<typename T> struct address_of;
        template<typename T> struct logical_not;
        template<typename T> struct pre_inc;
        template<typename T> struct pre_dec;
        template<typename T> struct post_inc;
        template<typename T> struct post_dec;
        template<typename T, typename U> struct shift_left;
        template<typename T, typename U> struct shift_right;
        template<typename T, typename U> struct multiplies;
        template<typename T, typename U> struct divides;
        template<typename T, typename U> struct modulus;
        template<typename T, typename U> struct plus;
        template<typename T, typename U> struct minus;
        template<typename T, typename U> struct less;
        template<typename T, typename U> struct greater;
        template<typename T, typename U> struct less_equal;
        template<typename T, typename U> struct greater_equal;
        template<typename T, typename U> struct equal_to;
        template<typename T, typename U> struct not_equal_to;
        template<typename T, typename U> struct logical_or;
        template<typename T, typename U> struct logical_and;
        template<typename T, typename U> struct bitwise_and;
        template<typename T, typename U> struct bitwise_or;
        template<typename T, typename U> struct bitwise_xor;
        template<typename T, typename U> struct comma;
        template<typename T, typename U> struct mem_ptr;
        template<typename T, typename U> struct assign;
        template<typename T, typename U> struct shift_left_assign;
        template<typename T, typename U> struct shift_right_assign;
        template<typename T, typename U> struct multiplies_assign;
        template<typename T, typename U> struct divides_assign;
        template<typename T, typename U> struct modulus_assign;
        template<typename T, typename U> struct plus_assign;
        template<typename T, typename U> struct minus_assign;
        template<typename T, typename U> struct bitwise_and_assign;
        template<typename T, typename U> struct bitwise_or_assign;
        template<typename T, typename U> struct bitwise_xor_assign;
        template<typename T, typename U> struct subscript;
        template<typename T, typename U, typename V> struct if_else_;
        template<typename Tag, typename T> struct unary_expr;
        template<typename Tag, typename T, typename U> struct binary_expr;

        #if defined(BOOST_HAS_VARIADIC_TMPL) && defined(BOOST_HAS_RVALUE_REFS)
        template<typename... Args> struct function;
        template<typename Tag, typename... Args> struct nary_expr;
        #else
        template<
            BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(BOOST_PROTO_MAX_ARITY, typename A, void)
          , typename Dummy = void
        >
        struct function;

        template<
            typename Tag
            BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(
                BOOST_PROTO_MAX_ARITY
              , typename A
              , = void BOOST_PP_INTERCEPT
            )
          , typename Dummy = void
        >
        struct nary_expr;
        #endif
    }

    using namespace op;

    namespace control
    {
        namespace detail
        {
            template<typename T, typename EnableIf = void>
            struct is_vararg;
        }

        #if defined(BOOST_HAS_VARIADIC_TMPL) && defined(BOOST_HAS_RVALUE_REFS)
        template<typename... Alts>
        struct or_;

        template<typename... Alts>
        struct and_;
        #else
        template<
            typename Grammar0
          , typename Grammar1
            BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(
                BOOST_PP_SUB(BOOST_PROTO_MAX_LOGICAL_ARITY,2)
              , typename G
              , = void BOOST_PP_INTERCEPT
            )
        >
        struct or_;

        template<
            typename Grammar0
          , typename Grammar1
            BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(
                BOOST_PP_SUB(BOOST_PROTO_MAX_LOGICAL_ARITY,2)
              , typename G
              , = void BOOST_PP_INTERCEPT
            )
        >
        struct and_;
        #endif

        template<typename Grammar>
        struct not_;

        template<typename If, typename Then = _, typename Else = not_<_> >
        struct if_;

        template<typename Cases>
        struct switch_;

        template<typename T>
        struct exact;

        template<typename T>
        struct convertible_to;

        template<typename Grammar>
        struct vararg;

        int const N = INT_MAX;
    }

    using control::or_;
    using control::and_;
    using control::if_;
    using control::not_;
    using control::switch_;
    using control::exact;
    using control::convertible_to;
    using control::vararg;
    using control::N;

    template<typename T>
    struct is_callable;

    template<typename T>
    struct is_aggregate;

    namespace transform
    {
        #define BOOST_PROTO_CALLABLE() typedef void proto_is_callable_;

        struct callable
        {
            BOOST_PROTO_CALLABLE()
        };

        template<typename Grammar, typename Fun = Grammar>
        struct when;

        template<typename Fun>
        struct otherwise;

        #if defined(BOOST_HAS_VARIADIC_TMPL) && defined(BOOST_HAS_RVALUE_REFS)
        template<typename Fun, typename... Args>
        struct call;

        template<typename Fun, typename... Args>
        struct make;

        template<typename Fun, typename... Args>
        struct bind;
        #else
        template<
            typename Fun
            BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(
                BOOST_PROTO_MAX_ARITY
              , typename A
              , = void BOOST_PP_INTERCEPT
            )
        >
        struct call;

        template<
            typename Fun
            BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(
                BOOST_PROTO_MAX_ARITY
              , typename A
              , = void BOOST_PP_INTERCEPT
            )
        >
        struct make;

        template<
            typename Fun
            BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(
                BOOST_PROTO_MAX_ARITY
              , typename A
              , = void BOOST_PP_INTERCEPT
            )
        >
        struct bind;
        #endif

        template<typename Sequence, typename State, typename Fun>
        struct fold;

        template<typename Sequence, typename State, typename Fun>
        struct reverse_fold;

        // BUGBUG can we replace fold_tree with fold<flatten(_), state, fun> ?
        template<typename Sequence, typename State, typename Fun>
        struct fold_tree;

        template<typename Sequence, typename State, typename Fun>
        struct reverse_fold_tree;

        struct _expr;
        struct _state;
        struct _visitor;

        template<int I>
        struct _arg_c;

        template<typename T>
        struct always;

        struct _arg0;
        struct _arg1;
        struct _arg2;
        struct _arg3;
        struct _arg4;
        struct _arg5;
        struct _arg6;
        struct _arg7;
        struct _arg8;
        struct _arg9;

        typedef _arg0 _arg;
        typedef _arg0 _left;
        typedef _arg1 _right;
    }

    using transform::when;
    using transform::otherwise;
    using transform::_arg0;
    using transform::_arg1;
    using transform::_arg2;
    using transform::_arg3;
    using transform::_arg4;
    using transform::_arg5;
    using transform::_arg6;
    using transform::_arg7;
    using transform::_arg8;
    using transform::_arg9;
    using transform::_arg;
    using transform::_left;
    using transform::_right;
    using transform::_expr;
    using transform::_state;
    using transform::_visitor;
    using transform::_arg_c;
    using transform::call;
    using transform::make;
    using transform::bind;
    using transform::fold;
    using transform::always;
    using transform::reverse_fold;
    using transform::fold_tree;
    using transform::reverse_fold_tree;
    using transform::callable;

    namespace context
    {
        struct null_context;

        template<typename Expr, typename Context, long Arity = Expr::proto_arity>
        struct null_eval;

        struct default_context;

        template<typename Expr, typename Context, typename Tag = typename Expr::proto_tag>
        struct default_eval;

        template<typename Derived, typename DefaultCtx = default_context>
        struct callable_context;

        template<typename Expr, typename Context
            #if defined(BOOST_HAS_VARIADIC_TMPL) && defined(BOOST_HAS_RVALUE_REFS)
          , typename Indices = typename proto::detail::make_indices<
                Expr::proto_arity == 0 ? 1 : Expr::proto_arity
            >::type
            #endif
        >
        struct callable_eval;
    }

    using context::null_context;
    using context::null_eval;
    using context::default_context;
    using context::default_eval;
    using context::callable_context;
    using context::callable_eval;

    namespace functional
    {
        struct left;
        struct right;
        struct deep_copy;

        template<typename Domain = default_domain>
        struct as_expr;

        template<typename Domain = default_domain>
        struct as_arg;

        template<typename N = mpl::long_<0> >
        struct arg;

        template<long N>
        struct arg_c;

        template<typename Tag, typename Domain = deduce_domain>
        struct make_arg;

        template<typename Tag, typename Domain = deduce_domain>
        struct unpack_arg;

        template<typename Tag, typename Domain = deduce_domain>
        struct unfused_arg_fun;

        template<typename Tag, typename Domain = deduce_domain>
        struct unfused_arg;

        template<typename Tag, typename Domain = deduce_domain>
        struct make_expr;

        template<typename Tag, typename Domain = deduce_domain>
        struct unpack_expr;

        template<typename Tag, typename Domain = deduce_domain>
        struct unfused_expr_fun;

        template<typename Tag, typename Domain = deduce_domain>
        struct unfused_expr;

        typedef make_expr<tag::terminal>            make_terminal;
        typedef make_expr<tag::posit>               make_posit;
        typedef make_expr<tag::negate>              make_negate;
        typedef make_expr<tag::dereference>         make_dereference;
        typedef make_expr<tag::complement>          make_complement;
        typedef make_expr<tag::address_of>          make_address_of;
        typedef make_expr<tag::logical_not>         make_logical_not;
        typedef make_expr<tag::pre_inc>             make_pre_inc;
        typedef make_expr<tag::pre_dec>             make_pre_dec;
        typedef make_expr<tag::post_inc>            make_post_inc;
        typedef make_expr<tag::post_dec>            make_post_dec;
        typedef make_expr<tag::shift_left>          make_shift_left;
        typedef make_expr<tag::shift_right>         make_shift_right;
        typedef make_expr<tag::multiplies>          make_multiplies;
        typedef make_expr<tag::divides>             make_divides;
        typedef make_expr<tag::modulus>             make_modulus;
        typedef make_expr<tag::plus>                make_plus;
        typedef make_expr<tag::minus>               make_minus;
        typedef make_expr<tag::less>                make_less;
        typedef make_expr<tag::greater>             make_greater;
        typedef make_expr<tag::less_equal>          make_less_equal;
        typedef make_expr<tag::greater_equal>       make_greater_equal;
        typedef make_expr<tag::equal_to>            make_equal_to;
        typedef make_expr<tag::not_equal_to>        make_not_equal_to;
        typedef make_expr<tag::logical_or>          make_logical_or;
        typedef make_expr<tag::logical_and>         make_logical_and;
        typedef make_expr<tag::bitwise_and>         make_bitwise_and;
        typedef make_expr<tag::bitwise_or>          make_bitwise_or;
        typedef make_expr<tag::bitwise_xor>         make_bitwise_xor;
        typedef make_expr<tag::comma>               make_comma;
        typedef make_expr<tag::mem_ptr>             make_mem_ptr;
        typedef make_expr<tag::assign>              make_assign;
        typedef make_expr<tag::shift_left_assign>   make_shift_left_assign;
        typedef make_expr<tag::shift_right_assign>  make_shift_right_assign;
        typedef make_expr<tag::multiplies_assign>   make_multiplies_assign;
        typedef make_expr<tag::divides_assign>      make_divides_assign;
        typedef make_expr<tag::modulus_assign>      make_modulus_assign;
        typedef make_expr<tag::plus_assign>         make_plus_assign;
        typedef make_expr<tag::minus_assign>        make_minus_assign;
        typedef make_expr<tag::bitwise_and_assign>  make_bitwise_and_assign;
        typedef make_expr<tag::bitwise_or_assign>   make_bitwise_or_assign;
        typedef make_expr<tag::bitwise_xor_assign>  make_bitwise_xor_assign;
        typedef make_expr<tag::subscript>           make_subscript;
        typedef make_expr<tag::if_else_>            make_if_else;
        typedef make_expr<tag::function>            make_function;

        struct flatten;
        struct pop_front;
        struct reverse;
    }

    typedef functional::make_terminal               _make_terminal;
    typedef functional::make_posit                  _make_posit;
    typedef functional::make_negate                 _make_negate;
    typedef functional::make_dereference            _make_dereference;
    typedef functional::make_complement             _make_complement;
    typedef functional::make_address_of             _make_address_of;
    typedef functional::make_logical_not            _make_logical_not;
    typedef functional::make_pre_inc                _make_pre_inc;
    typedef functional::make_pre_dec                _make_pre_dec;
    typedef functional::make_post_inc               _make_post_inc;
    typedef functional::make_post_dec               _make_post_dec;
    typedef functional::make_shift_left             _make_shift_left;
    typedef functional::make_shift_right            _make_shift_right;
    typedef functional::make_multiplies             _make_multiplies;
    typedef functional::make_divides                _make_divides;
    typedef functional::make_modulus                _make_modulus;
    typedef functional::make_plus                   _make_plus;
    typedef functional::make_minus                  _make_minus;
    typedef functional::make_less                   _make_less;
    typedef functional::make_greater                _make_greater;
    typedef functional::make_less_equal             _make_less_equal;
    typedef functional::make_greater_equal          _make_greater_equal;
    typedef functional::make_equal_to               _make_equal_to;
    typedef functional::make_not_equal_to           _make_not_equal_to;
    typedef functional::make_logical_or             _make_logical_or;
    typedef functional::make_logical_and            _make_logical_and;
    typedef functional::make_bitwise_and            _make_bitwise_and;
    typedef functional::make_bitwise_or             _make_bitwise_or;
    typedef functional::make_bitwise_xor            _make_bitwise_xor;
    typedef functional::make_comma                  _make_comma;
    typedef functional::make_mem_ptr                _make_mem_ptr;
    typedef functional::make_assign                 _make_assign;
    typedef functional::make_shift_left_assign      _make_shift_left_assign;
    typedef functional::make_shift_right_assign     _make_shift_right_assign;
    typedef functional::make_multiplies_assign      _make_multiplies_assign;
    typedef functional::make_divides_assign         _make_divides_assign;
    typedef functional::make_modulus_assign         _make_modulus_assign;
    typedef functional::make_plus_assign            _make_plus_assign;
    typedef functional::make_minus_assign           _make_minus_assign;
    typedef functional::make_bitwise_and_assign     _make_bitwise_and_assign;
    typedef functional::make_bitwise_or_assign      _make_bitwise_or_assign;
    typedef functional::make_bitwise_xor_assign     _make_bitwise_xor_assign;
    typedef functional::make_subscript              _make_subscript;
    typedef functional::make_if_else                _make_if_else;
    typedef functional::make_function               _make_function;

    typedef functional::flatten     _flatten;
    typedef functional::pop_front   _pop_front;
    typedef functional::pop_front   _reverse;

    template<typename T>
    struct is_extension;

    namespace exops
    {}
}}

#define BOOST_PROTO_UNCVREF(x)\
    typename boost::remove_cv<typename boost::remove_reference<x>::type>::type

#endif
