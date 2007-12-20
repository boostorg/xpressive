#ifndef BOOST_PP_IS_ITERATING
    ///////////////////////////////////////////////////////////////////////////////
    /// \file make_expr_ex.hpp
    /// Exploded instances of the make_expr and make_expr_ref functions
    //
    //  Copyright 2007 Eric Niebler. Distributed under the Boost
    //  Software License, Version 1.0. (See accompanying file
    //  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

	#define M1(Z, N, _) ((0)(1))

	#define M2(R, PRODUCT) M3(R, BOOST_PP_SEQ_SIZE(PRODUCT), PRODUCT)

    #define M3(R, SIZE, PRODUCT)                                                                    \
        template<typename Tag, BOOST_PP_ENUM_PARAMS(SIZE, typename A)>                              \
        typename lazy_disable_if<                                                                   \
            is_domain<A0>                                                                           \
          , result_of::make_expr_ref<Tag, BOOST_PP_SEQ_FOR_EACH_I_R(R, M5, ~, PRODUCT)>             \
        >::type                                                                                     \
        make_expr_ref(BOOST_PP_SEQ_FOR_EACH_I_R(R, M4, ~, PRODUCT))                                 \
        {                                                                                           \
            return result_of::make_expr_ref<Tag, BOOST_PP_SEQ_FOR_EACH_I_R(R, M5, ~, PRODUCT)>      \
                ::call(BOOST_PP_ENUM_PARAMS(SIZE, a));                                              \
        }                                                                                           \
                                                                                                    \
        template<typename Tag, typename Domain, BOOST_PP_ENUM_PARAMS(SIZE, typename A)>             \
        typename result_of::make_expr_ref<Tag, Domain, BOOST_PP_SEQ_FOR_EACH_I_R(R, M5, ~, PRODUCT)>::type\
        make_expr_ref(BOOST_PP_SEQ_FOR_EACH_I_R(R, M4, ~, PRODUCT))                                 \
        {                                                                                           \
            return result_of::make_expr_ref<Tag, Domain, BOOST_PP_SEQ_FOR_EACH_I_R(R, M5, ~, PRODUCT)>\
                ::call(BOOST_PP_ENUM_PARAMS(SIZE, a));                                              \
        }                                                                                           \
                                                                                                    \
        template<typename Tag, BOOST_PP_ENUM_PARAMS(SIZE, typename A)>                              \
        typename lazy_disable_if<                                                                   \
            is_domain<A0>                                                                           \
          , result_of::make_expr<Tag, BOOST_PP_SEQ_FOR_EACH_I_R(R, M5, ~, PRODUCT)>                 \
        >::type                                                                                     \
        make_expr(BOOST_PP_SEQ_FOR_EACH_I_R(R, M4, ~, PRODUCT))                                     \
        {                                                                                           \
            return result_of::make_expr<Tag, BOOST_PP_SEQ_FOR_EACH_I_R(R, M5, ~, PRODUCT)>          \
                ::call(BOOST_PP_ENUM_PARAMS(SIZE, a));                                              \
        }                                                                                           \
                                                                                                    \
        template<typename Tag, typename Domain, BOOST_PP_ENUM_PARAMS(SIZE, typename A)>             \
        typename result_of::make_expr<Tag, Domain, BOOST_PP_SEQ_FOR_EACH_I_R(R, M5, ~, PRODUCT)>::type\
        make_expr(BOOST_PP_SEQ_FOR_EACH_I_R(R, M4, ~, PRODUCT))                                     \
        {                                                                                           \
            return result_of::make_expr<Tag, Domain, BOOST_PP_SEQ_FOR_EACH_I_R(R, M5, ~, PRODUCT)>  \
                ::call(BOOST_PP_ENUM_PARAMS(SIZE, a));                                              \
        }                                                                                           \
        /**/

	#define M4(R, _, I, ELEM)                                                                       \
		BOOST_PP_COMMA_IF(I) BOOST_PP_CAT(A, I) BOOST_PP_CAT(C, ELEM) &BOOST_PP_CAT(a, I)           \
		/**/

	#define M5(R, _, I, ELEM)                                                                       \
		BOOST_PP_COMMA_IF(I) BOOST_PP_CAT(A, I) BOOST_PP_CAT(C, ELEM)&                              \
        /**/

	#define C0

	#define C1 const

	#define BOOST_PP_ITERATION_PARAMS_1 (3, (1, BOOST_PROTO_MAX_ARITY, <boost/xpressive/proto/detail/make_expr_ex.hpp>))
	#include BOOST_PP_ITERATE()

	#undef C0
	#undef C1
	#undef M1
	#undef M2
	#undef M3
	#undef M4
	#undef M5

#else

	BOOST_PP_SEQ_FOR_EACH_PRODUCT(
		M2,
		BOOST_PP_REPEAT(BOOST_PP_ITERATION(), M1, ~)
	)

#endif
