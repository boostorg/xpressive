#ifndef BOOST_PP_IS_ITERATING
    ///////////////////////////////////////////////////////////////////////////////
    /// \file make.hpp
    /// Contains definition of the make<> transform.
    //
    //  Copyright 2007 Eric Niebler. Distributed under the Boost
    //  Software License, Version 1.0. (See accompanying file
    //  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    #define BOOST_PP_ITERATION_PARAMS_1 (3, (0, BOOST_PROTO_MAX_ARITY, <boost/xpressive/proto/detail/make.hpp>))
    #include BOOST_PP_ITERATE()

#else

    #define N BOOST_PP_ITERATION()

        namespace detail
        {
            #if N > 0
            template<typename T BOOST_PP_ENUM_TRAILING_PARAMS(N, typename A)>
            struct nested_type_if<
                T
              , typelist<BOOST_PP_ENUM_PARAMS(N, A)>
              , typename typelist<
                    BOOST_PP_ENUM_BINARY_PARAMS(N, typename A, ::not_applied_ BOOST_PP_INTERCEPT)
                >::type
            >
            {
                typedef T type;
                typedef void not_applied_;
            };

            template<
                template<BOOST_PP_ENUM_PARAMS(N, typename BOOST_PP_INTERCEPT)> class R
                BOOST_PP_ENUM_TRAILING_PARAMS(N, typename A)
              , typename Expr, typename State, typename Visitor
            >
            struct make_<R<BOOST_PP_ENUM_PARAMS(N, A)>, Expr, State, Visitor>
              : nested_type_if<
                    #define TMP0(Z, M, DATA) make_if_<BOOST_PP_CAT(A, M), Expr, State, Visitor>
                    #define TMP1(Z, M, DATA) typename TMP0(Z, M, DATA) ::type
                    R<BOOST_PP_ENUM(N, TMP1, ~)>
                  , typelist<BOOST_PP_ENUM(N, TMP0, ~) >
                    #undef TMP0
                    #undef TMP1
                >
            {};
            #endif

            template<
                typename R
                BOOST_PP_ENUM_TRAILING_PARAMS(N, typename A)
              , typename Expr, typename State, typename Visitor
            >
            struct make_if_<R(BOOST_PP_ENUM_PARAMS(N, A)), Expr, State, Visitor, false>
              : remove_const<typename remove_reference<
                    typename boost::result_of<when<_, R(BOOST_PP_ENUM_PARAMS(N, A))>(Expr, State, Visitor)>::type
                >::type>
            {};

            template<
                typename R
                BOOST_PP_ENUM_TRAILING_PARAMS(N, typename A)
              , typename Expr, typename State, typename Visitor
            >
            struct make_if_<R(*)(BOOST_PP_ENUM_PARAMS(N, A)), Expr, State, Visitor, false>
              : remove_const<typename remove_reference<
                    typename boost::result_of<when<_, R(BOOST_PP_ENUM_PARAMS(N, A))>(Expr, State, Visitor)>::type
                >::type>
            {};
        }

        template<typename Return BOOST_PP_ENUM_TRAILING_PARAMS(N, typename A)>
        struct make<Return(BOOST_PP_ENUM_PARAMS(N, A))> : callable
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
              : detail::make_<Return, Expr, State, Visitor>
            {};

            template<typename Expr, typename State, typename Visitor>
            typename result<make(Expr const &, State const &, Visitor &)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                typedef typename result<make(Expr const &, State const &, Visitor &)>::type result_type;
                return detail::construct<result_type>(
                    #define TMP(Z, M, DATA) detail::as_lvalue(when<_, BOOST_PP_CAT(A, M)>()(expr, state, visitor))
                    BOOST_PP_ENUM(N, TMP, DATA)
                    #undef TMP
                );
            }
        };

        #if BOOST_WORKAROUND(__GNUC__, == 3)
        // work around GCC bug
        template<typename Tag, typename Args, long Arity BOOST_PP_ENUM_TRAILING_PARAMS(N, typename A)>
        struct make<expr<Tag, Args, Arity>(BOOST_PP_ENUM_PARAMS(N, A))> : callable
        {
            template<typename Sig>
            struct result
            {
                typedef expr<Tag, Args, Arity> type;
            };

            template<typename Expr, typename State, typename Visitor>
            expr<Tag, Args, Arity> operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                return proto::construct<proto::expr<Tag, Args, Arity> >(
                    #define TMP(Z, M, DATA) detail::as_lvalue(when<_, BOOST_PP_CAT(A, M)>()(expr, state, visitor))
                    BOOST_PP_ENUM(N, TMP, DATA)
                    #undef TMP
                );
            }
        };
        #endif

    #undef N

#endif
