///////////////////////////////////////////////////////////////////////////////
/// /file op_base.hpp
/// Contains definitions of unary_expr/</>, binary_expr/</> and nary_op/</>,
/// as well as the is_op/</> and the make_expr() helper function.
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PP_IS_ITERATING
    #ifndef BOOST_PROTO2_OP_BASE_HPP_EAN_04_01_2005
    #define BOOST_PROTO2_OP_BASE_HPP_EAN_04_01_2005

    #include <boost/preprocessor/inc.hpp>
    #include <boost/preprocessor/cat.hpp>
    #include <boost/preprocessor/iterate.hpp>
    #include <boost/preprocessor/punctuation/comma.hpp>
    #include <boost/preprocessor/facilities/intercept.hpp>
    #include <boost/preprocessor/repetition/repeat.hpp>
    #include <boost/preprocessor/repetition/repeat_from_to.hpp>
    #include <boost/preprocessor/repetition/enum_trailing.hpp>
    #include <boost/preprocessor/repetition/enum_params.hpp>
    #include <boost/preprocessor/repetition/enum_binary_params.hpp>
    #include <boost/preprocessor/repetition/enum_trailing_binary_params.hpp>

    #include <boost/mpl/at.hpp>
    #include <boost/mpl/size.hpp>
    #include <boost/mpl/vector.hpp>
    #include <boost/call_traits.hpp>
    #include <boost/static_assert.hpp>
    #include <boost/type_traits/add_const.hpp>
    #include <boost/type_traits/add_reference.hpp>
    #include <boost/xpressive/proto2/proto_fwd.hpp>
    #include <boost/xpressive/proto2/arg_traits.hpp>

    namespace boost { namespace proto2
    {
    #define BOOST_PROTO2_ARG(z, n, data)\
        typedef typename mpl::at_c<Args, n>::type BOOST_PP_CAT(BOOST_PP_CAT(arg, n), _type);\
        BOOST_PP_CAT(BOOST_PP_CAT(arg, n), _type) BOOST_PP_CAT(arg, n);\
        /**/

    #define BOOST_PROTO2_VOID(z, n, data)\
        typedef void BOOST_PP_CAT(BOOST_PP_CAT(arg, n), _type);\
        /**/

    #define BOOST_PROTO2_AS_OP(z, n, data)\
        proto2::as_expr(BOOST_PP_CAT(a,n))\
        /**/

    #define BOOST_PP_ITERATION_PARAMS_1 (3, (1, BOOST_PROTO2_MAX_ARITY, <boost/xpressive/proto2/op_base.hpp>))
    #include BOOST_PP_ITERATE()

    #undef BOOST_PROTO2_ARG
    #undef BOOST_PROTO2_VOID
    #undef BOOST_PROTO2_AS_OP
    }}
    #endif

#elif BOOST_PP_ITERATION_DEPTH() == 1

        template<typename Tag, typename Args>
        struct basic_expr<Tag, Args, BOOST_PP_ITERATION()>
        {
            typedef basic_expr expr_type;
            typedef Tag tag_type;
            typedef Args args_type;
            typedef mpl::long_<BOOST_PP_ITERATION()> arity;

            BOOST_PP_REPEAT(BOOST_PP_ITERATION(), BOOST_PROTO2_ARG, _)
            BOOST_PP_REPEAT_FROM_TO(BOOST_PP_ITERATION(), BOOST_PROTO2_MAX_ARITY, BOOST_PROTO2_VOID, _)

            basic_expr const &cast() const
            {
                return *this;
            }

            template<typename A>
            basic_expr<assign_tag, mpl::vector2<ref<basic_expr>, typename meta::as_expr<A>::type> > const
            operator =(A const &a) const
            {
                basic_expr<assign_tag, mpl::vector2<ref<basic_expr>, typename meta::as_expr<A>::type> > that = {{*this}, proto2::as_expr(a)};
                return that;
            }

            template<typename A>
            basic_expr<subscript_tag, mpl::vector2<ref<basic_expr>, typename meta::as_expr<A>::type> > const
            operator [](A const &a) const
            {
                basic_expr<subscript_tag, mpl::vector2<ref<basic_expr>, typename meta::as_expr<A>::type> > that = {{*this}, proto2::as_expr(a)};
                return that;
            }

            basic_expr<function_tag, mpl::vector1<ref<basic_expr> > > const
            operator ()() const
            {
                basic_expr<function_tag, mpl::vector1<ref<basic_expr> > > that = {{*this}};
                return that;
            }

    #define BOOST_PP_ITERATION_PARAMS_2 (3, (1, BOOST_PROTO2_MAX_ARITY, <boost/xpressive/proto2/op_base.hpp>))
    #include BOOST_PP_ITERATE()
        };

#elif BOOST_PP_ITERATION_DEPTH() == 2

    #define N BOOST_PP_ITERATION()
        template<BOOST_PP_ENUM_PARAMS(N, typename A)>
        basic_expr<function_tag, BOOST_PP_CAT(mpl::vector, BOOST_PP_INC(N))<ref<basic_expr> BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, typename meta::as_expr<A, >::type BOOST_PP_INTERCEPT)> > const
        operator ()(BOOST_PP_ENUM_BINARY_PARAMS(N, A, const &a)) const
        {
            basic_expr<function_tag, BOOST_PP_CAT(mpl::vector, BOOST_PP_INC(N))<ref<basic_expr> BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, typename meta::as_expr<A, >::type BOOST_PP_INTERCEPT)> > that = {{*this} BOOST_PP_ENUM_TRAILING(N, BOOST_PROTO2_AS_OP, _)};
            return that;
        }
    #undef N

#endif
