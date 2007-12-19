#ifndef BOOST_PP_IS_ITERATING
    ///////////////////////////////////////////////////////////////////////////////
    /// \file make_expr_ex.hpp
    /// Exploded instances of the make_expr and make_arg functions
    //
    //  Copyright 2007 Eric Niebler. Distributed under the Boost
    //  Software License, Version 1.0. (See accompanying file
    //  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

	#define M1(Z, N, _) ((0)(1))

	#define M2(R, PRODUCT) M3(R, BOOST_PP_SEQ_SIZE(PRODUCT), PRODUCT)

    #define M3(R, SIZE, PRODUCT)                                                                    \
        template<BOOST_PP_ENUM_PARAMS(SIZE, typename A)>                                            \
        expr<tag::function, args<expr &, BOOST_PP_SEQ_FOR_EACH_I_R(R, M5, ~, PRODUCT)> >            \
        operator()(BOOST_PP_SEQ_FOR_EACH_I_R(R, M4, ~, PRODUCT))                                    \
        {                                                                                           \
            typedef args<expr &, BOOST_PP_SEQ_FOR_EACH_I_R(R, M5, ~, PRODUCT)> args_type;           \
            expr<tag::function, args_type> that = {                                                 \
                argsns_::make_cons_<typename args_type::cons_type>(                                 \
                    *this                                                                           \
                  , BOOST_PP_SEQ_FOR_EACH_I_R(R, M6, ~, PRODUCT)                                    \
                )                                                                                   \
            };                                                                                      \
            return that;                                                                            \
        }                                                                                           \
                                                                                                    \
        template<BOOST_PP_ENUM_PARAMS(SIZE, typename A)>                                            \
        expr<tag::function, args<expr const &, BOOST_PP_SEQ_FOR_EACH_I_R(R, M5, ~, PRODUCT)> >      \
        operator()(BOOST_PP_SEQ_FOR_EACH_I_R(R, M4, ~, PRODUCT)) const                              \
        {                                                                                           \
            typedef args<expr const &, BOOST_PP_SEQ_FOR_EACH_I_R(R, M5, ~, PRODUCT)> args_type;     \
            expr<tag::function, args_type> that = {                                                 \
                argsns_::make_cons_<typename args_type::cons_type>(                                 \
                    *this                                                                           \
                  , BOOST_PP_SEQ_FOR_EACH_I_R(R, M6, ~, PRODUCT)                                    \
                )                                                                                   \
            };                                                                                      \
            return that;                                                                            \
        }                                                                                           \
        /**/

	#define M4(R, _, I, ELEM)                                                                       \
		BOOST_PP_COMMA_IF(I)                                                                        \
        BOOST_PP_CAT(A, I) BOOST_PP_CAT(C, ELEM) &BOOST_PP_CAT(a, I)                                \
		/**/

	#define M5(R, _, I, ELEM)                                                                       \
        BOOST_PP_COMMA_IF(I)                                                                        \
        typename result_of::as_arg<BOOST_PP_CAT(A, I) BOOST_PP_CAT(C, ELEM)&>::type                 \
        /**/

	#define M6(R, _, I, ELEM)                                                                       \
        BOOST_PP_COMMA_IF(I)                                                                        \
        result_of::as_arg<BOOST_PP_CAT(A, I) BOOST_PP_CAT(C, ELEM)&>::call(BOOST_PP_CAT(a, I))      \
        /**/

	#define C0

	#define C1 const

	#define BOOST_PP_ITERATION_PARAMS_1 (3, (1, BOOST_PP_DEC(BOOST_PROTO_MAX_ARITY), <boost/xpressive/proto/detail/fun_call_ex.hpp>))
	#include BOOST_PP_ITERATE()

	#undef C0
	#undef C1
	#undef M1
	#undef M2
	#undef M3
	#undef M4
	#undef M5
	#undef M6

#else

	BOOST_PP_SEQ_FOR_EACH_PRODUCT(
		M2,
		BOOST_PP_REPEAT(BOOST_PP_ITERATION(), M1, ~)
	)

#endif
