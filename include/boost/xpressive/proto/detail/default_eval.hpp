#ifndef BOOST_PP_IS_ITERATING
    ///////////////////////////////////////////////////////////////////////////////
    /// \file default_eval.hpp
    /// Contains definition of the default_eval_function_<> evaluator.
    //
    //  Copyright 2007 Eric Niebler. Distributed under the Boost
    //  Software License, Version 1.0. (See accompanying file
    //  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    #define EVAL_TYPE(Z, N, DATA)                                                                   \
        typename proto::result_of::eval<                                                            \
            typename remove_reference<typename proto::result_of::arg_c<DATA, N>::type>::type        \
          , Context                                                                                 \
        >::type

    #define EVAL(Z, N, DATA)                                                                        \
        proto::eval(proto::arg_c<N>(DATA), context)

    #define BOOST_PP_ITERATION_PARAMS_1 (3, (0, BOOST_PROTO_MAX_ARITY, <boost/xpressive/proto/detail/default_eval.hpp>))
    #include BOOST_PP_ITERATE()

    #undef EVAL_TYPE
    #undef EVAL

#else

    #define N BOOST_PP_ITERATION()

        template<typename Expr, typename Context>
        struct default_eval_function_<Expr, Context, N>
        {
            typedef
                typename proto::detail::result_of_fixup<EVAL_TYPE(~, 0, Expr)>::type
            function_type;

            typedef
                typename boost::result_of<
                    function_type(BOOST_PP_ENUM_SHIFTED(BOOST_PP_MAX(N, 1), EVAL_TYPE, Expr))
                >::type
            result_type;

            result_type operator ()(Expr &expr, Context &context) const
            {
                return EVAL(~, 0, expr)(BOOST_PP_ENUM_SHIFTED(BOOST_PP_MAX(N, 1), EVAL, expr));
            }
        };

    #undef N

#endif
