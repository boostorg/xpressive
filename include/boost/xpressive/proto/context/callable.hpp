///////////////////////////////////////////////////////////////////////////////
/// \file callable.hpp
/// Definintion of callable_context\<\>, an evaluation context for
/// proto::eval() that explodes each node and calls the derived context
/// type with the expressions constituents. If the derived context doesn't
/// have an overload that handles this node, fall-back to the default_context.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_CONTEXT_CALLABLE_HPP_EAN_06_23_2007
#define BOOST_PROTO_CONTEXT_CALLABLE_HPP_EAN_06_23_2007

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/type_traits.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/traits.hpp> // for arg_c
#include <boost/xpressive/proto/detail/indices.hpp>
#include <boost/xpressive/proto/detail/dont_care.hpp>

namespace boost { namespace proto
{
    namespace detail
    {
        typedef char yes_type;
        typedef char (&no_type)[2];

        struct private_type_
        {
            private_type_ const &operator,(int) const;
        };

        template<typename T>
        yes_type check_is_expr_handled(T const &);

        no_type check_is_expr_handled(private_type_ const &);

        template<typename Expr, typename Context, long Arity = Expr::proto_arity>
        struct is_expr_handled;

        #ifdef BOOST_HAS_VARIADIC_TMPL
        // TODO use PP to loop-unroll this
        template<typename Context, typename Args>
        struct callable_context_wrapper;

        template<int N, typename Result = args<> >
        struct make_dont_care;

        template<int N, typename... Result>
        struct make_dont_care<N, args<Result...> >
          : make_dont_care<N-1, args<dont_care, Result...> >
        {};

        template<typename... Result>
        struct make_dont_care<0, args<Result...> >
        {
            typedef args<Result...> type;
        };

        template<typename Context, typename... DontCare>
        struct callable_context_wrapper<Context, args<DontCare...> >
          : remove_cv<Context>::type
        {
            callable_context_wrapper();
            typedef private_type_ const &(*pointer_to_function)(dont_care, DontCare...);
            operator pointer_to_function() const;
        };

        template<typename Expr, typename Context, typename Args, typename Indices>
        struct is_expr_handled_aux_;

        template<typename Expr, typename Context, typename... Args, int... Indices>
        struct is_expr_handled_aux_<Expr, Context, args<Args...>, proto::detail::indices<Indices...> >
        {
            typedef typename make_dont_care<sizeof...(Args)>::type DontCare;

            static callable_context_wrapper<Context, DontCare> &sctx_;
            static Expr &sexpr_;

            BOOST_STATIC_CONSTANT(bool, value =
            (
                sizeof(yes_type) ==
                sizeof(
                    proto::detail::check_is_expr_handled(
                        (sctx_(
                            typename Expr::proto_tag()
                          , proto::arg_c<Indices>(sexpr_)...
                        ), 0)
                    )
            )));

            typedef mpl::bool_<value> type;
        };

        template<typename Expr, typename Context, long Arity>
        struct is_expr_handled
          : is_expr_handled_aux_<
                Expr
              , Context
              , typename Expr::proto_args
              , typename proto::detail::make_indices<Arity>::type
            >
        {};

        template<typename Expr, typename Context, typename Indices>
        struct callable_eval_;

        template<typename Expr, typename Context, int... Indices>
        struct callable_eval_<Expr, Context, proto::detail::indices<Indices...> >
        {
            typedef
                typename boost::result_of<
                    Context(
                        typename Expr::proto_tag
                      , typename proto::result_of::arg_c<Expr, Indices>::type...
                    )
                >::type
            result_type;

            result_type operator ()(Expr &expr, Context &context) const
            {
                return context(
                    typename Expr::proto_tag()
                  , proto::arg_c<Indices>(expr)...
                );
            }
        };
        #endif
    }

    namespace context
    {
        /// callable_eval
        ///
        template<typename Expr, typename Context, long Arity>
        struct callable_eval
          #ifdef BOOST_HAS_VARIADIC_TMPL
          : proto::detail::callable_eval_<Expr, Context, typename proto::detail::make_indices<Arity>::type>
          #endif
        {};

        /// callable_context
        ///
        template<typename Context, typename DefaultCtx>
        struct callable_context
        {
            /// callable_context::eval
            ///
            template<typename Expr, typename ThisContext = Context>
            struct eval
              : mpl::if_<
                    proto::detail::is_expr_handled<Expr, Context>
                  , callable_eval<Expr, ThisContext>
                  , typename DefaultCtx::template eval<Expr, Context>
                >::type
            {};
        };
    }

    #include <boost/xpressive/proto/detail/callable_eval.hpp>

}}

#endif
