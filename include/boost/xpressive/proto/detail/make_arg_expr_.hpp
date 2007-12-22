#ifndef BOOST_PP_IS_ITERATING
    ///////////////////////////////////////////////////////////////////////////////
    /// \file make_arg_expr_.hpp
    /// Definintion of the result_of::make_expr_ref and result_of::make_expr
    //
    //  Copyright 2007 Eric Niebler. Distributed under the Boost
    //  Software License, Version 1.0. (See accompanying file
    //  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    template<typename Tag, typename A0>
    struct make_expr_ref<Tag, A0>
      : proto::detail::make_<Tag, typename domain_of<A0>::type, functional::as_expr_ref, A0>
    {};

    template<typename Tag, typename A0>
    struct make_expr_ref<Tag, deduce_domain, A0>
      : proto::detail::make_<Tag, typename domain_of<A0>::type, functional::as_expr_ref, A0>
    {};

    template<typename Tag, typename A0>
    struct make_expr<Tag, A0>
      : proto::detail::make_<Tag, typename domain_of<A0>::type, functional::as_expr, A0>
    {};

    template<typename Tag, typename A0>
    struct make_expr<Tag, deduce_domain, A0>
      : proto::detail::make_<Tag, typename domain_of<A0>::type, functional::as_expr, A0>
    {};

    #define BOOST_PP_ITERATION_PARAMS_1 (3, (2, BOOST_PP_INC(BOOST_PROTO_MAX_ARITY), <boost/xpressive/proto/detail/make_arg_expr_.hpp>))
    #include BOOST_PP_ITERATE()

#else

    #define N BOOST_PP_ITERATION()

    template<typename Tag BOOST_PP_ENUM_TRAILING_PARAMS(N, typename A)>
    struct make_expr_ref<Tag BOOST_PP_ENUM_TRAILING_PARAMS(N, A)>
      : mpl::if_<
            is_domain<A0>
          , proto::detail::make_<Tag, A0, functional::as_expr_ref, BOOST_PP_ENUM_SHIFTED_PARAMS(N, A)>
          , make_expr_ref<Tag, deduce_domain BOOST_PP_ENUM_TRAILING_PARAMS(N, A)>
        >::type
    {};

    template<typename Tag BOOST_PP_ENUM_TRAILING_PARAMS(N, typename A)>
    struct make_expr_ref<Tag, deduce_domain BOOST_PP_ENUM_TRAILING_PARAMS(N, A)>
      : proto::detail::make_<
            Tag
          , typename proto::detail::deduce_domain_<
                typename domain_of<A0>::type
              , BOOST_PP_ENUM_SHIFTED_PARAMS(N, A)
            >::type
          , functional::as_expr_ref
            BOOST_PP_ENUM_TRAILING_PARAMS(N, A)
        >
    {};

    template<typename Tag BOOST_PP_ENUM_TRAILING_PARAMS(N, typename A)>
    struct make_expr<Tag BOOST_PP_ENUM_TRAILING_PARAMS(N, A)>
      : mpl::if_<
            is_domain<A0>
          , proto::detail::make_<Tag, A0, functional::as_expr, BOOST_PP_ENUM_SHIFTED_PARAMS(N, A)>
          , make_expr<Tag, deduce_domain BOOST_PP_ENUM_TRAILING_PARAMS(N, A)>
        >::type
    {};

    template<typename Tag BOOST_PP_ENUM_TRAILING_PARAMS(N, typename A)>
    struct make_expr<Tag, deduce_domain BOOST_PP_ENUM_TRAILING_PARAMS(N, A)>
      : proto::detail::make_<
            Tag
          , typename proto::detail::deduce_domain_<
                typename domain_of<A0>::type
              , BOOST_PP_ENUM_SHIFTED_PARAMS(N, A)
            >::type
          , functional::as_expr
            BOOST_PP_ENUM_TRAILING_PARAMS(N, A)
        >
    {};

    #undef N

#endif
