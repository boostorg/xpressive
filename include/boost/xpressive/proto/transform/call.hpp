///////////////////////////////////////////////////////////////////////////////
/// \file call.hpp
/// Contains definition of the call<> transform.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_TRANSFORM_CALL_HPP_EAN_11_02_2007
#define BOOST_PROTO_TRANSFORM_CALL_HPP_EAN_11_02_2007

#include <boost/utility/result_of.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/traits.hpp>

namespace boost { namespace proto
{

    namespace transform
    {

        template<typename Fun, typename... Args>
        struct call : raw_transform
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
              : boost::result_of<
                    Fun(typename boost::result_of<when<_, Args>(Expr, State, Visitor)>::type...)
                >
            {};

            template<typename Expr, typename State, typename Visitor>
            typename result<call(Expr, State, Visitor)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                Fun f;
                return f(when<_, Args>()(expr, state, visitor)...);
            }
        };

        template<typename Fun, typename... Args>
        struct call<Fun(Args...)>
          : call<Fun, Args...>
        {};

    }

    template<typename Fun, typename... Args>
    struct transform_category<transform::call<Fun, Args...> >
    {
        typedef raw_transform type;
    };

}}

#endif
