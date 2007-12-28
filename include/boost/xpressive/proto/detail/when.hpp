#ifndef BOOST_PP_IS_ITERATING
    ///////////////////////////////////////////////////////////////////////////////
    /// \file when.hpp
    /// Contains definition of the when<> transform.
    //
    //  Copyright 2007 Eric Niebler. Distributed under the Boost
    //  Software License, Version 1.0. (See accompanying file
    //  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    #define BOOST_PP_ITERATION_PARAMS_1 (3, (0, BOOST_PROTO_MAX_ARITY, <boost/xpressive/proto/detail/when.hpp>))
    #include BOOST_PP_ITERATE()

#else

    #define N BOOST_PP_ITERATION()

    template<typename Grammar, typename Return BOOST_PP_ENUM_TRAILING_PARAMS(N, typename A)>
    struct when<Grammar, Return(*)(BOOST_PP_ENUM_PARAMS(N, A))>
      : when<Grammar, Return(BOOST_PP_ENUM_PARAMS(N, A))>
    {};

    template<typename Grammar, typename Return BOOST_PP_ENUM_TRAILING_PARAMS(N, typename A)>
    struct when<Grammar, Return(BOOST_PP_ENUM_PARAMS(N, A))>
      : callable, detail::vararg_if<Grammar>
    {
        typedef Return when_function_type_(BOOST_PP_ENUM_PARAMS(N, A));
        typedef typename Grammar::proto_base_expr proto_base_expr;

        template<typename Sig>
        struct result;

        template<typename This, typename Expr, typename State, typename Visitor>
        struct result<This(Expr, State, Visitor)>
          : boost::result_of<
                typename mpl::if_<
                    is_callable<Return>
                  , call<when_function_type_> // "Return" is a function to call
                  , make<when_function_type_> // "Return" is an object to construct
                >::type(Expr, State, Visitor)
            >
        {};

        template<typename Expr, typename State, typename Visitor>
        typename result<when(Expr const &, State const &, Visitor &)>::type
        operator()(Expr const &expr, State const &state, Visitor &visitor) const
        {
            return typename mpl::if_<
                is_callable<Return>
              , call<when_function_type_>
              , make<when_function_type_>
            >::type()(expr, state, visitor);
        }
    };

    #undef N

#endif
