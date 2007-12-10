///////////////////////////////////////////////////////////////////////////////
/// \file bind.hpp
/// Contains definition of the bind<> transform.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_TRANSFORM_BIND_HPP_EAN_12_02_2007
#define BOOST_PROTO_TRANSFORM_BIND_HPP_EAN_12_02_2007

#include <boost/utility/result_of.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/transform/make.hpp>
#include <boost/xpressive/proto/transform/call.hpp>

namespace boost { namespace proto
{

    namespace transform
    {
        template<typename Return, typename... Args>
        struct bind : callable
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
              : boost::result_of<
                    call<
                        typename boost::result_of<make<Return>(Expr, State, Visitor)>::type
                      , Args...
                    >(Expr, State, Visitor)
                >
            {};

            template<typename Expr, typename State, typename Visitor>
            typename result<bind(Expr const &, State const &, Visitor &)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                return call<
                    typename boost::result_of<make<Return>(Expr const &, State const &, Visitor &)>::type
                  , Args...
                >()(expr, state, visitor);
            }
        };

        template<typename Fun, typename... Args>
        struct bind<Fun(Args...)>
          : bind<Fun, Args...>
        {};

    }

    template<typename Fun, typename... Args>
    struct is_callable<transform::bind<Fun, Args...> >
      : mpl::true_
    {};

}}

#endif
