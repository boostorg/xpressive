///////////////////////////////////////////////////////////////////////////////
/// \file arg_traits.hpp
/// Contains definitions for arg_type\<\>, left_type\<\>,
/// right_type\<\>, tag_type\<\>, and the helper functions arg(), left(),
/// and right().
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO2_ARG_TRAITS_HPP_EAN_04_01_2005
#define BOOST_PROTO2_ARG_TRAITS_HPP_EAN_04_01_2005

#include <boost/ref.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/call_traits.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/xpressive/proto2/proto_fwd.hpp>
#include <boost/xpressive/proto2/op_tags.hpp>

namespace boost { namespace proto2
{

    template<typename T>
    struct is_op
      : mpl::false_
    {};

    template<typename Tag, typename Args, long Arity>
    struct is_op<basic_expr<Tag, Args, Arity> >
      : mpl::true_
    {};

    template<typename Expr>
    struct is_op<ref<Expr> >
      : mpl::true_
    {};

#define BOOST_PROTO2_ARG(z, n, data)\
    typedef typename Expr::BOOST_PP_CAT(BOOST_PP_CAT(arg, n), _type) BOOST_PP_CAT(BOOST_PP_CAT(arg, n), _type);

    template<typename Expr>
    struct ref
    {
        typedef Expr expr_type;
        typedef typename Expr::tag_type tag_type;
        typedef typename Expr::args_type args_type;
        typedef typename Expr::arity arity;

        BOOST_PP_REPEAT(BOOST_PROTO2_MAX_ARITY, BOOST_PROTO2_ARG, _)

        Expr const &cast() const
        {
            return this->expr;
        }

        Expr const &expr;
    };

#undef BOOST_PROTO2_ARG

    template<typename T>
    struct unref
    {
        typedef T type;
    };

    template<typename T>
    struct unref<ref<T> >
    {
        typedef T type;
    };

    namespace meta
    {
        template<typename T, bool IsOp>
        struct as_expr
        {
            typedef basic_expr<terminal_tag, mpl::vector1<typename call_traits<T>::value_type> > type;
        };

        template<typename T>
        struct as_expr<T, true>
        {
            typedef ref<T> type;
        };

        template<typename T>
        struct as_expr<ref<T>, true>
        {
            typedef ref<T> type;
        };
    }

    namespace op
    {
        struct as_expr
        {
            template<typename Sig>
            struct result;

            template<typename This, typename T>
            struct result<This(T)>
              : meta::as_expr<typename meta::value_type<T>::type>
            {};

            template<typename T>
            typename meta::as_expr<T>::type operator()(T const &t) const
            {
                typename meta::as_expr<T>::type that = {t};
                return that;
            }

            template<typename T>
            ref<T> const &operator()(ref<T> const &t) const
            {
                return t;
            }
        };
    }

    op::as_expr const as_expr = {};

    template<typename T>
    struct literal
    {
        typedef typename call_traits<T>::value_type value_type;
        typedef basic_expr<terminal_tag, mpl::vector1<value_type> > type;
    };

    template<typename T>
    typename literal<T>::type lit(T const &t)
    {
        typename literal<T>::type that = {t};
        return that;
    }

    template<typename Tag, typename T>
    struct unary_expr
    {
        typedef basic_expr<Tag, mpl::vector1<T> > type;
    };

    template<typename Tag, typename T, typename U>
    struct binary_expr
    {
        typedef basic_expr<Tag, mpl::vector2<T, U> > type;
    };

    template<typename Expr>
    struct tag_type
    {
        typedef typename Expr::tag_type type;
    };

    template<typename Expr>
    struct arg_type
    {
        BOOST_STATIC_ASSERT(1 == Expr::arity::value);
        typedef typename Expr::arg0_type type;
    };

    template<typename Expr>
    struct left_type
    {
        BOOST_STATIC_ASSERT(2 == Expr::arity::value);
        typedef typename Expr::arg0_type type;
    };

    template<typename Expr>
    struct right_type
    {
        BOOST_STATIC_ASSERT(2 == Expr::arity::value);
        typedef typename Expr::arg1_type type;
    };

    template<typename Expr>
    inline typename Expr::arg0_type const &arg(Expr const &expr)
    {
        return expr.cast().arg0;
    }

    template<typename Expr>
    inline typename Expr::arg0_type const &left(Expr const &expr)
    {
        return expr.cast().arg0;
    }

    template<typename Expr>
    inline typename Expr::arg1_type const &right(Expr const &expr)
    {
        return expr.cast().arg1;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // make_expr (unary)
    template<typename Tag, typename Arg>
    basic_expr<Tag, mpl::vector1<Arg> > const
    make_expr(Arg const &arg)
    {
        basic_expr<Tag, mpl::vector1<Arg> > that = {arg};
        return that;
    }

    template<typename Tag, typename Arg>
    basic_expr<Tag, mpl::vector1<ref<Arg> > > const
    make_expr(reference_wrapper<Arg> const &arg)
    {
        basic_expr<Tag, mpl::vector1<ref<Arg> > > that = {{arg.get()}};
        return that;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // make_expr (binary)
    template<typename Tag, typename Left, typename Right>
    basic_expr<Tag, mpl::vector2<Left, Right> > const
    make_expr(Left const &left, Right const &right)
    {
        basic_expr<Tag, mpl::vector2<Left, Right> > that = {left, right};
        return that;
    }

    template<typename Tag, typename Left, typename Right>
    basic_expr<Tag, mpl::vector2<ref<Left>, Right> > const
    make_expr(reference_wrapper<Left> const &left, Right const &right)
    {
        basic_expr<Tag, mpl::vector2<ref<Left>, Right> > that = {{left.get()}, right};
        return that;
    }

    template<typename Tag, typename Left, typename Right>
    basic_expr<Tag, mpl::vector2<Left, ref<Right> > > const
    make_expr(Left const &left, reference_wrapper<Right> const &right)
    {
        basic_expr<Tag, mpl::vector2<Left, ref<Right> > > that = {left, {right.get()}};
        return that;
    }

    template<typename Tag, typename Left, typename Right>
    basic_expr<Tag, mpl::vector2<ref<Left>, ref<Right> > > const
    make_expr(reference_wrapper<Left> const &left, reference_wrapper<Right> const &right)
    {
        basic_expr<Tag, mpl::vector2<ref<Left>, ref<Right> > > that = {{left.get()}, {right.get()}};
        return that;
    }
}}

#endif
