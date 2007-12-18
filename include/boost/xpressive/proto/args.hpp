#ifndef BOOST_PP_IS_ITERATING
    ///////////////////////////////////////////////////////////////////////////////
    /// \file args.hpp
    /// Contains definition of args\<\> and term\<\> class templates.
    //
    //  Copyright 2007 Eric Niebler. Distributed under the Boost
    //  Software License, Version 1.0. (See accompanying file
    //  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    #ifndef BOOST_PROTO_ARGS_HPP_EAN_10_28_2007
    #define BOOST_PROTO_ARGS_HPP_EAN_10_28_2007

    #include <utility>
    #include <boost/mpl/assert.hpp>
    #include <boost/type_traits.hpp>
    #include <boost/utility/result_of.hpp>
    #include <boost/xpressive/proto/proto_fwd.hpp>
    #include <boost/xpressive/proto/detail/define.hpp>

    namespace boost { namespace proto
    {

        namespace argsns_
        {

            template<>
            struct args<>
            {
                BOOST_STATIC_CONSTANT(long, size = 0);
                typedef cons<> cons_type;
            };

            template<typename A>
            struct term
            {
                BOOST_STATIC_CONSTANT(long, size = 0);
                typedef cons<A> cons_type;

                /// INTERNAL ONLY
                ///
                typedef A back_;
            };

            template<>
            struct cons<>
            {
                BOOST_STATIC_CONSTANT(long, size = 0);
                typedef void car_type;
                typedef void cdr_type;
            };

        #define LBRACE(Z, N, DATA) {
        #define RBRACE(Z, N, DATA) }
        #define CDR_TYPE(Z, N, DATA) ::cdr_type
        #define CAR(Z, N, DATA) typedef BOOST_PP_CAT(A, N) car_type; car_type car; struct BOOST_PP_CAT(S, N) {
        #define CDR(Z, N, DATA) } cdr; typedef BOOST_PP_CAT(S, BOOST_PP_DEC(BOOST_PP_SUB(DATA, N))) cdr_type;

        #define BOOST_PP_ITERATION_PARAMS_1 (3, (1, BOOST_PROTO_MAX_ARITY, <boost/xpressive/proto/args.hpp>))
        #include BOOST_PP_ITERATE()

        #if defined(BOOST_HAS_VARIADIC_TMPL) && defined(BOOST_HAS_RVALUE_REFS)
            template<BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, typename A), typename... Args>
            struct args< BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, A), Args... >
            {
                BOOST_STATIC_CONSTANT(long, size = BOOST_PROTO_MAX_ARITY + sizeof...(Args));
                typedef cons<BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, A), Args... > cons_type;

                /// INTERNAL ONLY
                ///
                typedef typename args<Args...>::back_ back_;
            };

            template< BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, typename A), typename... Args>
            struct cons< BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, A), Args... >
            {
                BOOST_STATIC_CONSTANT(long, size = BOOST_PROTO_MAX_ARITY + sizeof...(Args));
                BOOST_PP_REPEAT(BOOST_PP_DEC(BOOST_PROTO_MAX_ARITY), CAR, BOOST_PP_DEC(N))
                typedef BOOST_PP_CAT(A, BOOST_PP_DEC(BOOST_PROTO_MAX_ARITY)) car_type;
                car_type car;
                cons<Args...> cdr;
                typedef cons<Args...> cdr_type;
                BOOST_PP_REPEAT(BOOST_PP_DEC(BOOST_PROTO_MAX_ARITY), CDR, BOOST_PP_DEC(BOOST_PROTO_MAX_ARITY))
            };

            template<typename Cons BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_PROTO_MAX_ARITY, typename A), typename... Rest >
            inline Cons make_cons_(BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PROTO_MAX_ARITY, A, &a), Rest &... rest)
            {
                Cons that = 
                    BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, {a)
                  , argsns_::make_cons_<typename Cons BOOST_PP_REPEAT(BOOST_PROTO_MAX_ARITY, CDR_TYPE, ~)>(rest...)
                    BOOST_PP_REPEAT(BOOST_PROTO_MAX_ARITY, RBRACE, ~);
                return that;
            }
        #endif

        #undef LBRACE
        #undef RBRACE
        #undef CDR_TYPE
        #undef CAR
        #undef CDR

        }

    }}

    #include <boost/xpressive/proto/detail/undef.hpp>

    #endif

#else

    #define N BOOST_PP_ITERATION()

        /// \brief A type sequence, for use as the 2nd parameter to the \c expr\<\> class template.
        ///
        /// A type sequence, for use as the 2nd parameter to the \c expr\<\> class template.
        /// The types in the sequence correspond to the children of a node in an expression tree.
        template< BOOST_PP_ENUM_PARAMS(N, typename A) >
        struct args< BOOST_PP_ENUM_PARAMS(N, A) >
        {
            BOOST_STATIC_CONSTANT(long, size = N);
            typedef cons< BOOST_PP_ENUM_PARAMS(N, A) > cons_type;

            /// INTERNAL ONLY
            ///
            typedef BOOST_PP_CAT(A, BOOST_PP_DEC(N)) back_;
        };

        template< BOOST_PP_ENUM_PARAMS(N, typename A) >
        struct cons< BOOST_PP_ENUM_PARAMS(N, A) >
        {
            BOOST_STATIC_CONSTANT(long, size = N);
            BOOST_PP_REPEAT(BOOST_PP_DEC(N), CAR, BOOST_PP_DEC(N))
            typedef BOOST_PP_CAT(A, BOOST_PP_DEC(N)) car_type;
            car_type car;
            static cons<> const cdr;
            typedef cons<> cdr_type;
            BOOST_PP_REPEAT(BOOST_PP_DEC(N), CDR, BOOST_PP_DEC(N))
        };

        template< BOOST_PP_ENUM_PARAMS(N, typename A) >
        cons<> const cons< BOOST_PP_ENUM_PARAMS(N, A) > BOOST_PP_REPEAT(BOOST_PP_DEC(N), CDR_TYPE, ~) ::cdr = {};

        template<typename Cons BOOST_PP_ENUM_TRAILING_PARAMS(N, typename A) >
        inline Cons make_cons_(BOOST_PP_ENUM_BINARY_PARAMS(N, A, &a))
        {
            Cons that = BOOST_PP_ENUM_PARAMS(N, {a) BOOST_PP_REPEAT(N, RBRACE, ~);
            return that;
        }

    #undef N

#endif
