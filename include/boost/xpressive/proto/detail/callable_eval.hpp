#ifndef BOOST_PP_IS_ITERATING
    ///////////////////////////////////////////////////////////////////////////////
    /// \file callable_eval.hpp
    /// Contains definition of the callable_eval<> evaluator and helpers.
    //
    //  Copyright 2007 Eric Niebler. Distributed under the Boost
    //  Software License, Version 1.0. (See accompanying file
    //  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    #define ARG_C_TYPE(Z, N, DATA) typename proto::result_of::arg_c<DATA, N>::type
    #define ARG_C(Z, N, DATA) proto::arg_c<N>(DATA)

    #define BOOST_PP_ITERATION_PARAMS_1 (3, (0, BOOST_PROTO_MAX_ARITY, <boost/xpressive/proto/detail/callable_eval.hpp>))
    #include BOOST_PP_ITERATE()

    #undef ARG_C_TYPE
    #undef ARG_C

#else

    #define N BOOST_PP_ITERATION()

    namespace detail
    {
        template<typename Context>
        struct BOOST_PP_CAT(callable_context_wrapper, N)
          : remove_cv<Context>::type
        {
            BOOST_PP_CAT(callable_context_wrapper, N)();
            typedef private_type_ const &(*pointer_to_function)(
                dont_care
                BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_PP_MAX(N, 1), dont_care BOOST_PP_INTERCEPT)
            );
            operator pointer_to_function() const;
        };

        template<typename Expr, typename Context>
        struct is_expr_handled<Expr, Context, N>
        {
            static BOOST_PP_CAT(callable_context_wrapper, N)<Context> &sctx_;
            static Expr &sexpr_;

            BOOST_STATIC_CONSTANT(bool, value =
            (
                sizeof(yes_type) ==
                sizeof(
                    proto::detail::check_is_expr_handled(
                        (sctx_(
                            typename Expr::proto_tag()
                            BOOST_PP_ENUM_TRAILING(BOOST_PP_MAX(N, 1), ARG_C, sexpr_)
                        ), 0)
                    )
            )));

            typedef mpl::bool_<value> type;
        };
    }

    namespace context
    {
        template<typename Expr, typename Context>
        struct callable_eval<Expr, Context, N>
        {
            typedef
                typename boost::result_of<
                    Context(
                        typename Expr::proto_tag
                        BOOST_PP_ENUM_TRAILING(BOOST_PP_MAX(N, 1), ARG_C_TYPE, Expr)
                    )
                >::type
            result_type;

            result_type operator ()(Expr &expr, Context &context) const
            {
                return context(
                    typename Expr::proto_tag()
                    BOOST_PP_ENUM_TRAILING(BOOST_PP_MAX(N, 1), ARG_C, expr)
                );
            }
        };
    }

    #undef N

#endif
