#ifndef BOOST_PP_IS_ITERATING
    ///////////////////////////////////////////////////////////////////////////////
    /// \file call.hpp
    /// Contains definition of the call<> transform.
    //
    //  Copyright 2007 Eric Niebler. Distributed under the Boost
    //  Software License, Version 1.0. (See accompanying file
    //  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    #if BOOST_PROTO_MAX_ARITY > 3
    #define BOOST_PP_ITERATION_PARAMS_1 (3, (4, BOOST_PROTO_MAX_ARITY, <boost/xpressive/proto/detail/call.hpp>))
    #include BOOST_PP_ITERATE()
    #endif

#else

    #define N BOOST_PP_ITERATION()

        template<typename Fun BOOST_PP_ENUM_TRAILING_PARAMS(N, typename A)>
        struct call<Fun(BOOST_PP_ENUM_PARAMS(N, A))> : callable
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
              : boost::result_of<
                    #define TMP(Z, M, DATA) typename boost::result_of<when<_, BOOST_PP_CAT(A, M)>(Expr, State, Visitor)>::type
                    Fun(BOOST_PP_ENUM(N, TMP, ~))
                    #undef TMP
                >
            {};

            template<typename Expr, typename State, typename Visitor>
            typename result<call(Expr const &, State const &, Visitor &)>::type
            operator ()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                Fun f;
                #define TMP(Z, M, DATA) when<_, BOOST_PP_CAT(A, M)>()(expr, state, visitor)
                return f(BOOST_PP_ENUM(N, TMP, ~));
                #undef TMP
            }
        };

    #undef N

#endif
