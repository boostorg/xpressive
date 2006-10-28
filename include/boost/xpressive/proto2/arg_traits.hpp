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
    struct is_op<basic_op<Tag, Args, Arity> >
      : mpl::true_
    {};

    template<typename Op>
    struct is_op<ref<Op> >
      : mpl::true_
    {};

#define BOOST_PROTO2_ARG(z, n, data)\
    typedef typename Op::BOOST_PP_CAT(BOOST_PP_CAT(arg, n), _type) BOOST_PP_CAT(BOOST_PP_CAT(arg, n), _type);

    template<typename Op>
    struct ref
    {
        typedef Op op_type;
        typedef typename Op::tag_type tag_type;
        typedef typename Op::args_type args_type;
        typedef typename Op::arity arity;

        BOOST_PP_REPEAT(BOOST_PROTO2_MAX_ARITY, BOOST_PROTO2_ARG, _)

        Op const &cast() const
        {
            return this->op;
        }

        //operator Op const &() const
        //{
        //    return this->op;
        //}

        Op const &op;
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

    template<typename T, bool IsOp>
    struct as_op_type
    {
        typedef basic_op<noop_tag, mpl::vector1<typename call_traits<T>::value_type> > type;
    };

    template<typename T>
    struct as_op_type<T, true>
    {
        typedef ref<T> type;
    };

    template<typename T>
    struct as_op_type<ref<T>, true>
    {
        typedef ref<T> type;
    };

    template<typename T>
    typename as_op_type<T>::type as_op(T const &t)
    {
        typename as_op_type<T>::type that = {t};
        return that;
    }

    template<typename T>
    ref<T> const &as_op(ref<T> const &t)
    {
        return t;
    }

    template<typename T>
    struct literal
    {
        typedef typename call_traits<T>::value_type value_type;
        typedef basic_op<noop_tag, mpl::vector1<value_type> > type;
    };

    template<typename T>
    typename literal<T>::type lit(T const &t)
    {
        typename literal<T>::type that = {t};
        return that;
    }

    template<typename Tag, typename T>
    struct unary_op
    {
        typedef basic_op<Tag, mpl::vector1<T> > type;
    };

    template<typename Tag, typename T, typename U>
    struct binary_op
    {
        typedef basic_op<Tag, mpl::vector2<T, U> > type;
    };

    template<typename Op>
    struct tag_type
    {
        typedef typename Op::tag_type type;
    };

    template<typename Op>
    struct arg_type
    {
        BOOST_STATIC_ASSERT(1 == Op::arity::value);
        typedef typename Op::arg0_type type;
    };

    template<typename Op>
    struct left_type
    {
        BOOST_STATIC_ASSERT(2 == Op::arity::value);
        typedef typename Op::arg0_type type;
    };

    template<typename Op>
    struct right_type
    {
        BOOST_STATIC_ASSERT(2 == Op::arity::value);
        typedef typename Op::arg1_type type;
    };

    template<typename Op>
    inline typename Op::arg0_type const &arg(Op const &op)
    {
        return op.cast().arg0;
    }

    template<typename Op>
    inline typename Op::arg0_type const &left(Op const &op)
    {
        return op.cast().arg0;
    }

    template<typename Op>
    inline typename Op::arg1_type const &right(Op const &op)
    {
        return op.cast().arg1;
    }

    /////////////////////////////////////////////////////////////////////////////////
    //// noop
    //template<typename Arg>
    //basic_op<noop_tag, mpl::vector1<Arg> > const
    //noop(Arg const &arg)
    //{
    //    basic_op<noop_tag, mpl::vector1<Arg> > that = {arg};
    //    return that;
    //}

    //template<typename Arg>
    //basic_op<noop_tag, mpl::vector1<ref<Arg> > > const
    //noop(reference_wrapper<Arg> const &arg)
    //{
    //    basic_op<noop_tag, mpl::vector1<ref<Arg> > > that = {{arg.get()}};
    //    return that;
    //}

    ///////////////////////////////////////////////////////////////////////////////
    // make_op (unary)
    template<typename Tag, typename Arg>
    basic_op<Tag, mpl::vector1<Arg> > const
    make_op(Arg const &arg)
    {
        basic_op<Tag, mpl::vector1<Arg> > that = {arg};
        return that;
    }

    template<typename Tag, typename Arg>
    basic_op<Tag, mpl::vector1<ref<Arg> > > const
    make_op(reference_wrapper<Arg> const &arg)
    {
        basic_op<Tag, mpl::vector1<ref<Arg> > > that = {{arg.get()}};
        return that;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // make_op (binary)
    template<typename Tag, typename Left, typename Right>
    basic_op<Tag, mpl::vector2<Left, Right> > const
    make_op(Left const &left, Right const &right)
    {
        basic_op<Tag, mpl::vector2<Left, Right> > that = {left, right};
        return that;
    }

    template<typename Tag, typename Left, typename Right>
    basic_op<Tag, mpl::vector2<ref<Left>, Right> > const
    make_op(reference_wrapper<Left> const &left, Right const &right)
    {
        basic_op<Tag, mpl::vector2<ref<Left>, Right> > that = {{left.get()}, right};
        return that;
    }

    template<typename Tag, typename Left, typename Right>
    basic_op<Tag, mpl::vector2<Left, ref<Right> > > const
    make_op(Left const &left, reference_wrapper<Right> const &right)
    {
        basic_op<Tag, mpl::vector2<Left, ref<Right> > > that = {left, {right.get()}};
        return that;
    }

    template<typename Tag, typename Left, typename Right>
    basic_op<Tag, mpl::vector2<ref<Left>, ref<Right> > > const
    make_op(reference_wrapper<Left> const &left, reference_wrapper<Right> const &right)
    {
        basic_op<Tag, mpl::vector2<ref<Left>, ref<Right> > > that = {{left.get()}, {right.get()}};
        return that;
    }
}}

#endif
