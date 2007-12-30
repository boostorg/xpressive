#ifndef BOOST_PP_IS_ITERATING
    ///////////////////////////////////////////////////////////////////////////////
    /// \file by_value_generator.hpp
    /// Definintion of the by_value_generator_ helper
    //
    //  Copyright 2007 Eric Niebler. Distributed under the Boost
    //  Software License, Version 1.0. (See accompanying file
    //  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    #ifndef BOOST_PROTO_DETAIL_BY_VALUE_GENERATOR_HPP_EAN_11_07_2007
    #define BOOST_PROTO_DETAIL_BY_VALUE_GENERATOR_HPP_EAN_11_07_2007

    #include <boost/xpressive/proto/proto_fwd.hpp>
    #include <boost/xpressive/proto/detail/cat_args.hpp>
    #include <boost/xpressive/proto/detail/define.hpp>

    namespace boost { namespace proto
    {
        namespace detail
        {

            template<typename Args>
            struct by_value_generator_;

            template<typename A>
            struct by_value_generator_<term<A> >
            {
                // TODO doesn't handle reference to function
                typedef term<UNCVREF(A)> type;

                template<typename Cons>
                static typename type::cons_type call(Cons const &a)
                {
                    return argsns_::make_cons_<typename type::cons_type>(a.car);
                }
            };

        #ifdef BOOST_HAS_VARIADIC_TMPL
            template<
                BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, typename A)
              , typename... Rest
            >
            struct by_value_generator_<args<BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, A), Rest...> >
            {
                #define TMP(Z, N, DATA) UNCVREF(A##N)
                typedef typename cat_args<
                    args<BOOST_PP_ENUM(BOOST_PROTO_MAX_ARITY, TMP, ~) >
                  , typename by_value_generator_<args<Rest...> >::type
                >::type type;
                #undef TMP

                template<typename Cons>
                static typename type::cons_type call(Cons const &a)
                {
                    #define TMP0(Z, N, DATA) .cdr
                    #define TMP1(Z, N, DATA) {a BOOST_PP_REPEAT_ ## Z(N, TMP0, ~) .car
                    #define TMP2(Z, N, DATA) }
                    typename type::cons_type that =
                        BOOST_PP_ENUM(BOOST_PROTO_MAX_ARITY, TMP1, ~)
                      , by_value_generator_<args<Rest...> >::call(a BOOST_PP_REPEAT(BOOST_PROTO_MAX_ARITY, TMP0, ~))
                        BOOST_PP_REPEAT(BOOST_PROTO_MAX_ARITY, TMP2, ~);
                    return that;
                    #undef TMP0
                    #undef TMP1
                    #undef TMP2
                }
            };

        #endif

        #define BOOST_PP_ITERATION_PARAMS_1 (3, (1, BOOST_PROTO_MAX_ARITY, <boost/xpressive/proto/detail/by_value_generator.hpp>))
        #include BOOST_PP_ITERATE()

        }
    }}

    #include <boost/xpressive/proto/detail/undef.hpp>
    #endif

#else

            template<BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), typename A)>
            struct by_value_generator_<args<BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), A)> >
            {
                #define TMP(Z, N, DATA) UNCVREF(A##N)
                typedef args<BOOST_PP_ENUM(BOOST_PP_ITERATION(), TMP, ~) > type;
                #undef TMP

                template<typename Cons>
                static typename type::cons_type call(Cons const &a)
                {
                    #define TMP0(Z, N, DATA) .cdr
                    #define TMP1(Z, N, DATA) {a BOOST_PP_REPEAT_ ## Z(N, TMP0, ~) .car
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
