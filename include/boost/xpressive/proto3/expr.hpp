///////////////////////////////////////////////////////////////////////////////
/// \file expr.hpp
/// Contains the definition of the expr\<\> class template
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO3_EXPR_HPP_EAN_10_28_2007
#define BOOST_PROTO3_EXPR_HPP_EAN_10_28_2007

#include <boost/xpressive/proto3/proto_fwd.hpp>

namespace boost { namespace proto
{

    namespace exprns_
    {
        template<typename T, typename U>
        void is_same_type(U const &)
        {
            BOOST_MPL_ASSERT((is_same<T,U>));
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

            template<typename Expr, typename, typename>
            struct apply
            {
                typedef Expr type;
            };

            template<typename Expr, typename State, typename Visitor>
            static Expr const &call(Expr const &expr, State const &, Visitor &)
            {
                return expr;
            }

            template<typename... A>
            static expr make(A &&... a)
            {

                expr that = {{a...}};
                return that;
            }

            expr &proto_base()
            {
                return *this;
            }

            expr const &proto_base() const
            {
                return *this;
            }

            template<typename A>
            expr<tag::assign, args<expr &, typename result_of::as_arg<A>::type> >
            operator=(A &&a)
            {
                expr<tag::assign, args<expr &, typename result_of::as_arg<A>::type> > that =
                    //{{*this, {proto::as_arg(std::forward<A>(a))}}};
                    {{*this, {a}}};
                return that;
            }

            template<typename A>
            expr<tag::assign, args<expr const &, typename result_of::as_arg<A>::type> >
            operator=(A &&a) const
            {
                expr<tag::assign, args<expr const &, typename result_of::as_arg<A>::type> > that =
                    //{{*this, {proto::as_arg(std::forward<A>(a))}}};
                    {{*this, {a}}};
                return that;
            }

            template<typename A>
            expr<tag::subscript, args<expr &, typename result_of::as_arg<A>::type> >
            operator[](A &&a)
            {
                expr<tag::subscript, args<expr &, typename result_of::as_arg<A>::type> > that =
                    //{{*this, {proto::as_arg(std::forward<A>(a))}}};
                    {{*this, {a}}};
                return that;
            }

            template<typename A>
            expr<tag::subscript, args<expr const &, typename result_of::as_arg<A>::type> >
            operator[](A &&a) const
            {
                expr<tag::subscript, args<expr const &, typename result_of::as_arg<A>::type> > that =
                    //{{*this, {proto::as_arg(std::forward<A>(a))}}};
                    {{*this, {a}}};
                return that;
            }

            template<typename... A>
            expr<tag::function, args<expr &, typename result_of::as_arg<A>::type...> >
            operator()(A &&... a)
            {
                expr<tag::function, args<expr &, typename result_of::as_arg<A>::type...> > that =
                    {{*this, a...}};
                    //{argsns_::make_cons(*this, proto::as_arg(std::forward<A>(a))...)};
                return that;
            }

            template<typename... A>
            expr<tag::function, args<expr const &, typename result_of::as_arg<A>::type...> >
            operator()(A &&... a) const
            {
                expr<tag::function, args<expr const &, typename result_of::as_arg<A>::type...> > that =
                    {{*this, a...}};
                    //{argsns_::make_cons(*this, proto::as_arg(std::forward<A>(a))...)};
                return that;
            }
        };

    }

}}

#endif
