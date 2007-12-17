#ifndef BOOST_PP_IS_ITERATING
    ///////////////////////////////////////////////////////////////////////////////
    /// \file apply_args.hpp
    /// Definintion of the pass-through transforms
    //
    //  Copyright 2007 Eric Niebler. Distributed under the Boost
    //  Software License, Version 1.0. (See accompanying file
    //  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    #ifndef BOOST_PROTO_DETAIL_APPLY_ARGS_HPP_EAN_11_07_2007
    #define BOOST_PROTO_DETAIL_APPLY_ARGS_HPP_EAN_11_07_2007

    #include <boost/xpressive/proto/proto_fwd.hpp>
    #include <boost/xpressive/proto/detail/define.hpp>

    namespace boost { namespace proto { namespace op
    {
        namespace detail
        {

            template<typename E, typename S, typename V, typename G>
            struct apply_args;

        #if defined(BOOST_HAS_VARIADIC_TMPL) && defined(BOOST_HAS_RVALUE_REFS)
            template<typename A0, typename A1>
            struct cat_args;

            template<typename... A0, typename... A1>
            struct cat_args<args<A0...>, args<A1...> >
            {
                typedef args<A0..., A1...> type;
            };

            template<typename... G>
            struct pad_args
            {
                typedef args<G...> type;
            };

            template<
                BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, typename E)
              , typename... ERest
              , typename S
              , typename V 
                BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_PROTO_MAX_ARITY, typename G)
              , typename... GRest
            >
            struct apply_args<
                args<BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, E), ERest...>
              , S
              , V 
              , args<
                    BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, G)
                  , GRest...
                >
            >
            {
                #define TMP(Z, N, DATA) typename boost::result_of<G##N(UNCVREF(E##N), S, V)>::type
                typedef typename cat_args<
                    args<BOOST_PP_ENUM(BOOST_PROTO_MAX_ARITY, TMP, ~) >
                  , typename apply_args<args<ERest...>, S, V, typename pad_args<GRest...>::type>::type
                >::type type;
                #undef TMP

                template<typename Cons>
                static typename type::cons_type call(Cons const &a, S const &s, V &v)
                {
                    #define TMP0(Z, N, DATA) .cdr
                    #define TMP1(Z, N, DATA) {G##N()(a BOOST_PP_REPEAT_ ## Z(N, TMP0, ~) .car, s, v)
                    #define TMP2(Z, N, DATA) }
                    typename type::cons_type that =
                        BOOST_PP_ENUM(BOOST_PROTO_MAX_ARITY, TMP1, ~)
                      , apply_args<args<ERest...>, S, V, typename pad_args<GRest...>::type>
                            ::call(a BOOST_PP_REPEAT(BOOST_PROTO_MAX_ARITY, TMP0, ~), s, v)
                        BOOST_PP_REPEAT(BOOST_PROTO_MAX_ARITY, TMP2, ~);
                    #undef TMP0
                    #undef TMP1
                    #undef TMP2
                    return that;
                }
            };

        #endif

        #define BOOST_PP_ITERATION_PARAMS_1 (3, (1, BOOST_PROTO_MAX_ARITY, <boost/xpressive/proto/detail/apply_args.hpp>))
        #include BOOST_PP_ITERATE()

        }
    }}}

    #include <boost/xpressive/proto/detail/undef.hpp>
    #endif

#else

        #if defined(BOOST_HAS_VARIADIC_TMPL) && defined(BOOST_HAS_RVALUE_REFS)
        template<BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), typename G)>
        struct pad_args<BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), G)>
        {
            #define TMP(Z, N, DATA) , BOOST_PP_CAT(G, BOOST_PP_DEC(BOOST_PP_ITERATION()))
            typedef args<
                BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), G)
                BOOST_PP_REPEAT_FROM_TO(BOOST_PP_ITERATION(), BOOST_PROTO_MAX_ARITY, TMP, ~)
            > type;
            #undef TMP
        };
        #endif

        template<
            BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), typename E)
          , typename S
          , typename V 
            BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_PROTO_MAX_ARITY, typename G)
            #if defined(BOOST_HAS_VARIADIC_TMPL) && defined(BOOST_HAS_RVALUE_REFS)
          , typename... GRest
            #endif
        >
        struct apply_args<
            args<BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), E)>
          , S
          , V
          , args<
                BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, G)
                #if defined(BOOST_HAS_VARIADIC_TMPL) && defined(BOOST_HAS_RVALUE_REFS)
              , GRest...
                #endif
            >
        >
        {
            #define TMP(Z, N, DATA) typename boost::result_of<G##N(UNCVREF(E##N), S, V)>::type
            typedef args<BOOST_PP_ENUM(BOOST_PP_ITERATION(), TMP, ~)> type;
            #undef TMP

            template<typename Cons>
            static typename type::cons_type call(Cons const &a, S const &s, V &v)
            {
                #define TMP0(Z, N, DATA) .cdr
                #define TMP1(Z, N, DATA) {G##N()(a BOOST_PP_REPEAT_ ## Z(N, TMP0, ~) .car, s, v)
                #define TMP2(Z, N, DATA) }
                typename type::cons_type that = 
                    BOOST_PP_ENUM(BOOST_PP_ITERATION(), TMP1, ~)
                    BOOST_PP_REPEAT(BOOST_PP_ITERATION(), TMP2, ~);
                return that;
                #undef TMP0
                #undef TMP1
                #undef TMP2
            }
        };

#endif
