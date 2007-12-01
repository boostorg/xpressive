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
#include <boost/type_traits.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>

#define UNCV(X)                                                                 \
    typename remove_cv<X>::type

#define UNREF(X)                                                                \
    typename remove_reference<X>::type

#define UNCVREF(X)                                                              \
    UNCV(UNREF(X))

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

        struct make_cons_fun
        {
            template<typename Sig>
            struct result;

            template<typename This, typename... Args>
            struct result<This(Args...)>
            {
                typedef cons<UNCV(Args)...> type;
            };

            template<typename Head, typename... Tail>
            cons<UNCV(Head), UNCV(Tail)...> operator()(Head &&head, Tail &&... tail) const
            {
                cons<UNCV(Head), UNCV(Tail)...> that = {head, (*this)(std::forward<Tail>(tail)...)};
                return that;
            }

            template<typename Head>
            cons<UNCV(Head)> operator()(Head &&head) const
            {
                cons<UNCV(Head)> that = {head};
                return that;
            }
        };

        make_cons_fun const make_cons = {};

        struct fanout_args_fun
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Fun, typename... Args>
            struct result<This(Fun, cons<Args...>)>
              : boost::result_of<UNCVREF(Fun)(UNCVREF(Args)...)>
            {};

            template<typename Fun, typename... Args>
            typename boost::result_of<Fun(Args...)>::type
            operator()(Fun const &fun, cons<Args...> const &cons) const
            {
                typedef typename boost::result_of<Fun(Args...)>::type type;
                return this->call_<type>(fun, cons.cdr, cons.car);
            }

        private:

            template<typename Ret, typename Fun, typename... Tail, typename... Head>
            static Ret call_(Fun const &fun, cons<Tail...> const &cons, Head const &...head)
            {
                return call_<Ret>(fun, cons.cdr, head..., cons.car);
            }

            template<typename Ret, typename Fun, typename... Head>
            static Ret call_(Fun const &fun, cons<> const &, Head const &... head)
            {
                return fun(head...);
            }
        };

        fanout_args_fun const fanout_args = {};

    }

}}

#undef UNCV
#undef UNREF
#undef UNCVREF

#endif
