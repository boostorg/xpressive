///////////////////////////////////////////////////////////////////////////////
/// \file bind.hpp
/// Contains definition of the bind<> transform.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO3_TRANSFORM_BIND_HPP_EAN_11_02_2007
#define BOOST_PROTO3_TRANSFORM_BIND_HPP_EAN_11_02_2007

#include <boost/utility/result_of.hpp>
#include <boost/xpressive/proto3/proto_fwd.hpp>
#include <boost/xpressive/proto3/traits.hpp>

namespace boost { namespace proto
{

    namespace transform
    {

        template<typename Fun, typename... Args>
        struct bind : raw_transform
        {
            template<typename Expr, typename State, typename Visitor>
            struct apply
              : boost::result_of<
                    Fun(typename when<_, Args>::template apply<Expr, State, Visitor>::type...)
                >
            {};

            template<typename Expr, typename State, typename Visitor>
            static typename apply<Expr, State, Visitor>::type
            call(Expr const &expr, State const &state, Visitor &visitor)
            {
                Fun f;
                return f(when<_, Args>::call(expr, state, visitor)...);
            }
        };

        template<typename Fun>
        struct bind<Fun> : Fun, function_transform
        {};

    }

    template<typename Fun>
    struct transform_category<transform::bind<Fun> >
    {
        typedef function_transform type;
    };

    template<typename Fun, typename... Args>
    struct transform_category<transform::bind<Fun, Args...> >
    {
        typedef raw_transform type;
    };

}}

#endif
