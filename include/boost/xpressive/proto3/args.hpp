///////////////////////////////////////////////////////////////////////////////
/// \file args.hpp
/// Contains definition of args\<\> and term\<\> class templates.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO3_ARGS_HPP_EAN_10_28_2007
#define BOOST_PROTO3_ARGS_HPP_EAN_10_28_2007

#include <utility>
#include <boost/xpressive/proto3/proto_fwd.hpp>

namespace boost { namespace proto
{

    namespace argsns_
    {

        template<typename... Args>
        struct args
        {
            static long const size = sizeof...(Args);
            typedef cons<Args...> cons_type;
        };

        template<typename Arg>
        struct term
        {
            static long const size = 0;
            typedef cons<Arg> cons_type;
        };

        template<typename Args>
        struct back;

        template<typename Head, typename... Tail>
        struct back<args<Head, Tail...> >
          : back<args<Tail...> >
        {};

        template<typename Head>
        struct back<args<Head> >
        {
            typedef Head type;
        };

        template<typename Head>
        struct back<term<Head> >
        {
            typedef Head type;
        };

        template<typename Head, typename... Tail>
        struct cons<Head, Tail...>
        {
            static long const size = 1 + sizeof...(Tail);
            typedef Head car_type;
            typedef cons<Tail...> cdr_type;
            car_type car;
            cdr_type cdr;
        };

        template<>
        struct cons<>
        {
            static long const size = 0;
            typedef void car_type;
            typedef void cdr_type;
        };

        template<typename Head>
        struct cons<Head>
        {
            static long const size = 1;
            typedef Head car_type;
            typedef cons<> cdr_type;
            car_type car;
            static cdr_type const cdr;
        };

        template<typename Head>
        cons<> const cons<Head>::cdr = {};

        template<typename Head, typename... Tail>
        inline cons<Head, Tail...> make_cons(Head &&head, Tail &&... tail)
        {
            cons<Head, Tail...> that = {head, make_cons(std::forward<Tail>(tail)...)};
            return that;
        }

        template<typename Head>
        inline cons<Head> make_cons(Head &&head)
        {
            cons<Head> that = {head};
            return that;
        }

    }

}}

#endif
