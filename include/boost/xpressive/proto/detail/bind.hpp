#ifndef BOOST_PP_IS_ITERATING
    ///////////////////////////////////////////////////////////////////////////////
    /// \file bind.hpp
    /// Contains definition of the bind<> transform.
    //
    //  Copyright 2007 Eric Niebler. Distributed under the Boost
    //  Software License, Version 1.0. (See accompanying file
    //  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    #define BOOST_PP_ITERATION_PARAMS_1 (3, (0, BOOST_PROTO_MAX_ARITY, <boost/xpressive/proto/detail/bind.hpp>))
    #include BOOST_PP_ITERATE()

#else

    #define N BOOST_PP_ITERATION()

        template<typename Return BOOST_PP_ENUM_TRAILING_PARAMS(N, typename A)>
        struct bind<Return(BOOST_PP_ENUM_PARAMS(N, A))> : callable
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
              : boost::result_of<
                    call<
                        typename boost::result_of<make<Return>(Expr, State, Visitor)>::type(BOOST_PP_ENUM_PARAMS(N, A))
                    >(Expr, State, Visitor)
                >
            {};

            template<typename Expr, typename State, typename Visitor>
            typename result<bind(Expr const &, State const &, Visitor &)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                return call<
                    typename boost::result_of<make<Return>(Expr const &, State const &, Visitor &)>::type(BOOST_PP_ENUM_PARAMS(N, A))
                >()(expr, state, visitor);
            }
        };

    #undef N

#endif
