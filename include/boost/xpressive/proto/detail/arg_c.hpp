#ifndef BOOST_PP_IS_ITERATING
    ///////////////////////////////////////////////////////////////////////////////
    /// \file arg_c.hpp
    /// Definintion of the arg_c trait, and friends
    //
    //  Copyright 2007 Eric Niebler. Distributed under the Boost
    //  Software License, Version 1.0. (See accompanying file
    //  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    #ifndef BOOST_PROTO_DETAIL_ARG_C_HPP_EAN_11_07_2007
    #define BOOST_PROTO_DETAIL_ARG_C_HPP_EAN_11_07_2007

    #include <boost/xpressive/proto/proto_fwd.hpp>
    #include <boost/xpressive/proto/detail/define.hpp>

    namespace boost { namespace proto { namespace result_of
    {
        namespace detail
        {

            template<typename Cons, long N>
            struct arg_c;

            template<typename Cons, long N>
            struct arg_cv;

            template<typename Cons, long N>
            struct value_at_c;

        #define CDR_TYPE(Z, N, DATA) ::cdr_type
        #define CDR(Z, N, DATA) .cdr

        #ifdef BOOST_HAS_VARIADIC_TMPL
            template<typename Cons, long N>
            struct arg_c
            {
                typedef arg_c<typename Cons BOOST_PP_REPEAT(BOOST_PROTO_MAX_ARITY, CDR_TYPE, ~), N-BOOST_PROTO_MAX_ARITY> base_type;
                typedef typename base_type::type type;

                static type call(Cons &args)
                {
                    return base_type::call(args BOOST_PP_REPEAT(BOOST_PROTO_MAX_ARITY, CDR, ~));
                }
            };

            template<typename Cons, long N>
            struct arg_cv
            {
                typedef arg_c<typename Cons BOOST_PP_REPEAT(BOOST_PROTO_MAX_ARITY, CDR_TYPE, ~), N-BOOST_PROTO_MAX_ARITY> base_type;
                typedef typename base_type::type type;

                static type call(Cons const &args)
                {
                    return base_type::call(args BOOST_PP_REPEAT(BOOST_PROTO_MAX_ARITY, CDR, ~));
                }
            };

            template<typename Cons, long N>
            struct value_at_c
              : value_at_c<typename Cons BOOST_PP_REPEAT(BOOST_PROTO_MAX_ARITY, CDR_TYPE, ~), N-BOOST_PROTO_MAX_ARITY>
            {};
        #endif

        #define BOOST_PP_ITERATION_PARAMS_1 (3, (0, BOOST_PROTO_MAX_ARITY, <boost/xpressive/proto/detail/arg_c.hpp>))
        #include BOOST_PP_ITERATE()

        #undef CDR_TYPE
        #undef CDR
        }
    }}}

    #include <boost/xpressive/proto/detail/undef.hpp>
    #endif

#else

    #define N BOOST_PP_ITERATION()

            template<typename Cons>
            struct arg_c<Cons, N>
            {
                typedef REF(typename Cons BOOST_PP_REPEAT(N, CDR_TYPE, ~) ::car_type) type;

                static type call(Cons &args)
                {
                    return args BOOST_PP_REPEAT(N, CDR, ~) .car;
                }
            };

            template<typename Cons>
            struct arg_cv<Cons, N>
            {
                typedef CVREF(typename Cons BOOST_PP_REPEAT(N, CDR_TYPE, ~) ::car_type) type;

                static type call(Cons const &args)
                {
                    return args BOOST_PP_REPEAT(N, CDR, ~) .car;
                }
            };

            template<typename Cons>
            struct value_at_c<Cons, N>
            {
                typedef typename Cons BOOST_PP_REPEAT(N, CDR_TYPE, ~) ::car_type type;
            };

    #undef N

#endif
