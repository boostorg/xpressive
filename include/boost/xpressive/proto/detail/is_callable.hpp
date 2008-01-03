#ifndef BOOST_PP_IS_ITERATING
    ///////////////////////////////////////////////////////////////////////////////
    /// \file is_callable.hpp
    /// Definintion of detail::is_callable_
    //
    //  Copyright 2007 Eric Niebler. Distributed under the Boost
    //  Software License, Version 1.0. (See accompanying file
    //  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    #ifndef BOOST_PROTO_DETAIL_IS_CALLABLE_HPP_EAN_11_07_2007
    #define BOOST_PROTO_DETAIL_IS_CALLABLE_HPP_EAN_11_07_2007

    #include <boost/mpl/bool.hpp>
    #include <boost/type_traits.hpp>
    #include <boost/xpressive/proto/proto_fwd.hpp>

    namespace boost { namespace proto
    {
        namespace detail
        {

            template<typename T, typename EnableIf = void>
            struct is_callable2_
              : mpl::false_
            {};

            template<typename T>
            struct is_callable2_<T, typename T::proto_is_callable_>
              : mpl::true_
            {};

            template<typename T>
            struct is_callable_
              : is_callable2_<T>
            {};

        #ifdef BOOST_HAS_VARIADIC_TMPL
            template<typename... Args>
            struct back;

            template<BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, typename A), typename... Rest>
            struct back<BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, A), Rest...>
              : back<Rest...>
            {};

            template<
                template<BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, typename BOOST_PP_INTERCEPT), typename...> class T
              , BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, typename A)
              , typename... Rest
            >
            struct is_callable_<T<BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, A), Rest...> >
              : is_same<typename back<Rest...>::type, callable>
            {};
        #endif

        #define BOOST_PP_ITERATION_PARAMS_1 (3, (1, BOOST_PROTO_MAX_ARITY, <boost/xpressive/proto/detail/is_callable.hpp>))
        #include BOOST_PP_ITERATE()

        }
    }}

    #endif

#else

    #define N BOOST_PP_ITERATION()

        #ifdef BOOST_HAS_VARIADIC_TMPL
            template<BOOST_PP_ENUM_PARAMS(N, typename A)>
            struct back<BOOST_PP_ENUM_PARAMS(N, A)>
            {
                typedef BOOST_PP_CAT(A, BOOST_PP_DEC(N)) type;
            };
        #endif

            template<
                template<BOOST_PP_ENUM_PARAMS(N, typename BOOST_PP_INTERCEPT)> class T
              , BOOST_PP_ENUM_PARAMS(N, typename A)
            >
            struct is_callable_<T<BOOST_PP_ENUM_PARAMS(N, A)> >
              : is_same<BOOST_PP_CAT(A, BOOST_PP_DEC(N)), callable>
            {};

    #undef N

#endif
