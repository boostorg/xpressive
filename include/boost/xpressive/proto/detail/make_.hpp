#ifndef BOOST_PP_IS_ITERATING
    ///////////////////////////////////////////////////////////////////////////////
    /// \file make_.hpp
    /// Definintion of the make_ helper
    //
    //  Copyright 2007 Eric Niebler. Distributed under the Boost
    //  Software License, Version 1.0. (See accompanying file
    //  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    template<
        typename Tag
      , typename Domain
      , template<typename> class AsExpr
      , BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(BOOST_PROTO_MAX_ARITY, typename A, void)
      , typename Dummy = void
    >
    struct make_;

    template<typename Domain, template<typename> class AsExpr, typename A>
    struct make_<tag::terminal, Domain, AsExpr, A>
    {
        typedef typename boost::result_of<AsExpr<Domain>(A)>::type type;

        static type call(CVREF(A) a)
        {
            return AsExpr<Domain>()(a);
        }
    };

#define BOOST_PP_ITERATION_PARAMS_1 (3, (1, BOOST_PROTO_MAX_ARITY, <boost/xpressive/proto/detail/make_.hpp>))
#include BOOST_PP_ITERATE()

#else

    template<
        typename Tag
      , typename Domain
      , template<typename> class AsExpr
        BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_PP_ITERATION(), typename A)
    >
    struct make_<Tag, Domain, AsExpr BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_PP_ITERATION(), A)>
    {
        #define TMP(Z, N, DATA) typename boost::result_of<AsExpr<Domain>(A##N)>::type
        typedef
            expr<
                Tag
              , args<BOOST_PP_ENUM(BOOST_PP_ITERATION(), TMP, ~)>
            >
        expr_type;
        #undef TMP

        typedef typename Domain::template apply<expr_type>::type type;

        #define TMP0(Z, N, DATA) CVREF(A##N) a##N
        #define TMP1(Z, N, DATA) AsExpr<Domain>()(a##N)
        static type call(BOOST_PP_ENUM(BOOST_PP_ITERATION(), TMP0, ~))
        {
            return Domain::make(proto::construct<expr_type>(BOOST_PP_ENUM(BOOST_PP_ITERATION(), TMP1, ~)));
        }
        #undef TMP0
        #undef TMP1
    };

#endif
