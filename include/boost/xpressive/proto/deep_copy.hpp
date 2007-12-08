///////////////////////////////////////////////////////////////////////////////
/// \file deep_copy.hpp
/// Replace all nodes stored by reference by nodes stored by value.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_DEEP_COPY_HPP_EAN_11_21_2006
#define BOOST_PROTO_DEEP_COPY_HPP_EAN_11_21_2006

#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/args.hpp>
#include <boost/xpressive/proto/expr.hpp>
#include <boost/xpressive/proto/traits.hpp>
#include <boost/xpressive/proto/generate.hpp>

#define UNCV(X)                                                                 \
    typename remove_cv<X>::type

#define UNREF(X)                                                                \
    typename remove_reference<X>::type

#define UNCVREF(X)                                                              \
    UNCV(UNREF(X))

namespace boost { namespace proto
{
    namespace detail
    {
        template<typename Expr, typename Args = typename Expr::proto_args>
        struct deep_copy_impl;

        struct deep_copy_cons
        {
            template<typename Sig>
            struct result;

            template<typename This, typename... Args>
            struct result<This(Args...)>
            {
                typedef argsns_::cons<typename deep_copy_impl<UNCVREF(Args)>::type...> type;
            };

            template<typename... Args>
            typename result<deep_copy_cons(Args...)>::type
            operator()(Args const &... args) const
            {
                return result<deep_copy_cons(Args...)>::type::make(
                    deep_copy_impl<Args>::call(args)...
                );
            }
        };

        template<typename Expr, typename T>
        struct deep_copy_impl<Expr, term<T> >
        {
            typedef typename terminal<UNCVREF(T)>::type expr_type;
            typedef typename Expr::proto_domain::template apply<expr_type>::type type;

            static type call(Expr const &expr)
            {
                return Expr::proto_domain::make(expr_type::make(proto::arg(expr)));
            }
        };

        template<typename Expr, typename... Args>
        struct deep_copy_impl<Expr, args<Args...> >
        {
            typedef expr<typename Expr::proto_tag, args<
                typename deep_copy_impl<UNCVREF(Args)>::type...
            > > expr_type;
            typedef typename Expr::proto_domain::template apply<expr_type>::type type;

            static type call(Expr const &expr)
            {
                expr_type that = {
                    argsns_::fanout_args(
                        proto::detail::deep_copy_cons()
                      , expr.proto_base().proto_args_
                    )
                };
                return Expr::proto_domain::make(that);
            }
        };

    }

    namespace result_of
    {
        template<typename Expr>
        struct deep_copy
          : proto::detail::deep_copy_impl<Expr>
        {};
    }

    namespace functional
    {
        struct deep_copy
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Expr>
            struct result<This(Expr)>
              : result_of::deep_copy<UNCVREF(Expr)>
            {};

            template<typename Expr>
            typename result_of::deep_copy<Expr>::type
            operator()(Expr const &expr) const
            {
                return result_of::deep_copy<Expr>::call(expr);
            }
        };
    }

    functional::deep_copy const deep_copy = {};

}}

#undef UNCV
#undef UNREF
#undef UNCVREF

#endif // BOOST_PROTO_DEEP_COPY_HPP_EAN_11_21_2006
