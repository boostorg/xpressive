///////////////////////////////////////////////////////////////////////////////
/// \file when.hpp
/// Definition of when transform.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_TRANSFORM_WHEN_HPP_EAN_10_29_2007
#define BOOST_PROTO_TRANSFORM_WHEN_HPP_EAN_10_29_2007

#include <boost/mpl/if.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/traits.hpp>
#include <boost/xpressive/proto/transform/call.hpp>
#include <boost/xpressive/proto/transform/make.hpp>
#include <boost/function_types/is_function_pointer.hpp>

namespace boost { namespace proto { namespace transform
{
    namespace detail
    {
        // so that when< vararg<...>, ...> looks like a vararg grammar.
        template<typename Grammar, typename EnableIf = void>
        struct vararg_if
        {};

        template<typename Grammar>
        struct vararg_if<Grammar, typename Grammar::proto_is_vararg_>
        {
            typedef void proto_is_vararg_;
        };
    }

    // Simple transform, takes a raw transform and
    // applies it directly.
    template<typename Grammar, typename Fun>
    struct when
      : Fun, detail::vararg_if<Grammar>
    {
        typedef typename Grammar::proto_base_expr proto_base_expr;
    };

    #ifdef BOOST_HAS_VARIADIC_TMPL
    // Function-style transforms, handled below...
    template<typename Grammar, typename Return, typename... Args>
    struct when<Grammar, Return(*)(Args...)>
      : when<Grammar, Return(Args...)>
    {};

    // Lambda-style transform, takes a function-style
    // transform with arguments and calls it, OR takes a
    // (possibly lambda) type and constructor arguments.
    template<typename Grammar, typename Return, typename... Args>
    struct when<Grammar, Return(Args...)>
      : callable, detail::vararg_if<Grammar>
    {
        typedef typename Grammar::proto_base_expr proto_base_expr;

        template<typename Sig>
        struct result;

        template<typename This, typename Expr, typename State, typename Visitor>
        struct result<This(Expr, State, Visitor)>
          : boost::result_of<
                typename mpl::if_<
                    is_callable<Return>
                  , call<Return(Args...)> // "Return" is a function to call
                  , make<Return(Args...)> // "Return" is an object to construct
                >::type(Expr, State, Visitor)
            >
        {};

        template<typename Expr, typename State, typename Visitor>
        typename result<when(Expr const &, State const &, Visitor &)>::type
        operator ()(Expr const &expr, State const &state, Visitor &visitor) const
        {
            return typename mpl::if_<
                is_callable<Return>
              , call<Return(Args...)>
              , make<Return(Args...)>
            >::type()(expr, state, visitor);
        }
    };
    #else
    #include <boost/xpressive/proto/detail/when.hpp>
    #endif

    template<typename Fun>
    struct otherwise
      : when<_, Fun>
    {};

}}} // namespace boost::proto::transform

#endif
