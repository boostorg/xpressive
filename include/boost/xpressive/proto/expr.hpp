///////////////////////////////////////////////////////////////////////////////
/// \file expr.hpp
/// Contains the definition of the expr\<\> class template
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_EXPR_HPP_EAN_10_28_2007
#define BOOST_PROTO_EXPR_HPP_EAN_10_28_2007

#include <cstddef>
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/utility/addressof.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/tags.hpp>
#include <boost/xpressive/proto/traits.hpp>

namespace boost { namespace proto
{

    namespace exprns_
    {
        namespace detail
        {
            template<typename Tag, typename Arg>
            struct address_of_hack
            {
                typedef address_of_hack type;
            };

            template<typename Expr>
            struct address_of_hack<tag::address_of, Expr &>
            {
                typedef Expr *type;
            };

            template<typename X, std::size_t N, typename Y>
            void checked_copy(X (&x)[N], Y (&y)[N])
            {
                for(std::size_t i = 0; i < N; ++i)
                {
                    y[i] = x[i];
                }
            }

            template<typename T, std::size_t N>
            struct if_is_array
            {};

            template<typename T, std::size_t N>
            struct if_is_array<T[N], N>
            {
                typedef void type;
            };
        }

        template<typename Tag, typename Args, long Arity>
        struct expr
        {
            typedef Tag proto_tag;
            typedef Args proto_args;
            typedef void proto_is_expr_;
            static long const proto_arity = Arity;
            typedef expr proto_base_expr;
            typedef expr proto_derived_expr;
            typedef default_domain proto_domain;
            typedef tag::proto_expr fusion_tag;

            typename Args::cons_type proto_args_;

            expr &proto_base()
            {
                return *this;
            }

            expr const &proto_base() const
            {
                return *this;
            }

            typedef
                typename
                    exprns_::detail::address_of_hack<Tag, typename Args::cons_type::car_type>
                ::type
            address_of_hack_type_;

            operator address_of_hack_type_() const
            {
                return boost::addressof(this->proto_args_.car);
            }

            // TODO make this a free function
            template<typename... A>
            static expr make(A &&... a)
            {
                expr that = {argsns_::make_cons_<typename Args::cons_type>(a...)};
                return that;
            }

            // necessary for terminals that store arrays by value
            template<typename A, std::size_t N>
            static expr make(
                A (&a)[N]
              , typename exprns_::detail::if_is_array<
                    typename Args::cons_type::car_type, N
                >::type * = 0
            )
            {
                typedef char arity_is_zero[0==Arity];
                expr that;
                exprns_::detail::checked_copy(a, that.proto_args_.car);
                return that;
            }

        #if defined(BOOST_HAS_VARIADIC_TMPL) && defined(BOOST_HAS_RVALUE_REFS)

            template<typename A>
            expr<tag::assign, args<expr &, typename result_of::as_arg<A>::type> >
            operator=(A &&a)
            {
                expr<tag::assign, args<expr &, typename result_of::as_arg<A>::type> > that =
                    {{*this, {result_of::as_arg<A>::call(a)}}};
                return that;
            }

            template<typename A>
            expr<tag::assign, args<expr const &, typename result_of::as_arg<A>::type> >
            operator=(A &&a) const
            {
                expr<tag::assign, args<expr const &, typename result_of::as_arg<A>::type> > that =
                    {{*this, {result_of::as_arg<A>::call(a)}}};
                return that;
            }

            template<typename A>
            expr<tag::subscript, args<expr &, typename result_of::as_arg<A>::type> >
            operator[](A &&a)
            {
                expr<tag::subscript, args<expr &, typename result_of::as_arg<A>::type> > that =
                    {{*this, {result_of::as_arg<A>::call(a)}}};
                return that;
            }

            template<typename A>
            expr<tag::subscript, args<expr const &, typename result_of::as_arg<A>::type> >
            operator[](A &&a) const
            {
                expr<tag::subscript, args<expr const &, typename result_of::as_arg<A>::type> > that =
                    {{*this, {result_of::as_arg<A>::call(a)}}};
                return that;
            }

            template<typename... A>
            expr<tag::function, args<expr &, typename result_of::as_arg<A>::type...> >
            operator()(A &&... a)
            {
                typedef args<expr &, typename result_of::as_arg<A>::type...> args_type;
                expr<tag::function, args_type> that =
                    {argsns_::make_cons_<typename args_type::cons_type>(*this, result_of::as_arg<A>::call(a)...)};
                return that;
            }

            template<typename... A>
            expr<tag::function, args<expr const &, typename result_of::as_arg<A>::type...> >
            operator()(A &&... a) const
            {
                typedef args<expr const &, typename result_of::as_arg<A>::type...> args_type;
                expr<tag::function, args_type> that =
                    {argsns_::make_cons_<typename args_type::cons_type>(*this, result_of::as_arg<A>::call(a)...)};
                return that;
            }

        #else

            template<typename A>
            expr<tag::assign, args<expr &, typename result_of::as_arg<A &>::type> >
            operator=(A &a)
            {
                expr<tag::assign, args<expr &, typename result_of::as_arg<A &>::type> > that =
                    {{*this, {result_of::as_arg<A &>::call(a)}}};
                return that;
            }

            template<typename A>
            expr<tag::assign, args<expr const &, typename result_of::as_arg<A &>::type> >
            operator=(A &a) const
            {
                expr<tag::assign, args<expr const &, typename result_of::as_arg<A &>::type> > that =
                    {{*this, {result_of::as_arg<A &>::call(a)}}};
                return that;
            }

            template<typename A>
            expr<tag::assign, args<expr &, typename result_of::as_arg<A const &>::type> >
            operator=(A const &a)
            {
                expr<tag::assign, args<expr &, typename result_of::as_arg<A const &>::type> > that =
                    {{*this, {result_of::as_arg<A const &>::call(a)}}};
                return that;
            }

            template<typename A>
            expr<tag::assign, args<expr const &, typename result_of::as_arg<A const &>::type> >
            operator=(A const &a) const
            {
                expr<tag::assign, args<expr const &, typename result_of::as_arg<A const &>::type> > that =
                    {{*this, {result_of::as_arg<A const &>::call(a)}}};
                return that;
            }

            template<typename A>
            expr<tag::subscript, args<expr &, typename result_of::as_arg<A &>::type> >
            operator[](A &a)
            {
                expr<tag::subscript, args<expr &, typename result_of::as_arg<A &>::type> > that =
                    {{*this, {result_of::as_arg<A &>::call(a)}}};
                return that;
            }

            template<typename A>
            expr<tag::subscript, args<expr const &, typename result_of::as_arg<A &>::type> >
            operator[](A &a) const
            {
                expr<tag::subscript, args<expr const &, typename result_of::as_arg<A &>::type> > that =
                    {{*this, {result_of::as_arg<A &>::call(a)}}};
                return that;
            }

            template<typename A>
            expr<tag::subscript, args<expr &, typename result_of::as_arg<A const &>::type> >
            operator[](A const &a)
            {
                expr<tag::subscript, args<expr &, typename result_of::as_arg<A const &>::type> > that =
                    {{*this, {result_of::as_arg<A const &>::call(a)}}};
                return that;
            }

            template<typename A>
            expr<tag::subscript, args<expr const &, typename result_of::as_arg<A const &>::type> >
            operator[](A const &a) const
            {
                expr<tag::subscript, args<expr const &, typename result_of::as_arg<A const &>::type> > that =
                    {{*this, {result_of::as_arg<A const &>::call(a)}}};
                return that;
            }

            expr<tag::function, args<expr &> >
            operator()()
            {
                expr<tag::function, args<expr &> > that = {{*this}};
                return that;
            }

            expr<tag::function, args<expr const &> >
            operator()() const
            {
                expr<tag::function, args<expr const &> > that = {{*this}};
                return that;
            }

            #include <boost/xpressive/proto/detail/fun_call_ex.hpp>

        #endif

        };

    }

}}

#endif
