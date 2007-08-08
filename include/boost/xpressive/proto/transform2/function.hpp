///////////////////////////////////////////////////////////////////////////////
/// \file function.hpp
/// Proto transforms for applying a function object.
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_TRANSFORM2_FUNCTION_HPP_EAN_06_23_2007
#define BOOST_PROTO_TRANSFORM2_FUNCTION_HPP_EAN_06_23_2007

#include <boost/xpressive/proto/detail/prefix.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/detail/suffix.hpp>

namespace boost { namespace proto { namespace transform2
{

    // Apply a function object, passing just Expr
    template<typename Grammar, typename Function1>
    struct function1
      : Grammar, virtual transform_base
    {
        function1() {}

        template<typename Expr, typename State, typename Visitor>
        struct apply
          : boost::result_of<
                Function1(
                    typename Grammar::template apply<Expr, State, Visitor>::type
                )
            >
        {};

        template<typename Expr, typename State, typename Visitor>
        static typename apply<Expr, State, Visitor>::type
        call(Expr const &expr, State const &state, Visitor &visitor)
        {
            return Function1()(Grammar::call(expr, state, visitor));
        }
    };

    // Apply a function object, passing Expr and State
    template<typename Grammar, typename Function2>
    struct function2
      : Grammar, virtual transform_base
    {
        function2() {}

        template<typename Expr, typename State, typename Visitor>
        struct apply
          : boost::result_of<
                Function2(
                    typename Grammar::template apply<Expr, State, Visitor>::type
                  , State const &
                )
            >
        {};

        template<typename Expr, typename State, typename Visitor>
        static typename apply<Expr, State, Visitor>::type
        call(Expr const &expr, State const &state, Visitor &visitor)
        {
            return Function2()(Grammar::call(expr, state, visitor), state);
        }
    };

    // Apply a function object, passing Expr, State and Visitor
    template<typename Grammar, typename Function3>
    struct function3
      : Grammar, virtual transform_base
    {
        function3() {}

        template<typename Expr, typename State, typename Visitor>
        struct apply
          : boost::result_of<
                Function3(
                    typename Grammar::template apply<Expr, State, Visitor>::type
                  , State const &
                  , Visitor &
                )
            >
        {};

        template<typename Expr, typename State, typename Visitor>
        static typename apply<Expr, State, Visitor>::type
        call(Expr const &expr, State const &state, Visitor &visitor)
        {
            return Function3()(Grammar::call(expr, state, visitor), state, visitor);
        }
    };

}}}

namespace boost { namespace proto
{
    template<typename Grammar, typename Function1>
    struct is_transform<transform2::function1<Grammar, Function1> >
      : mpl::true_
    {};

    template<typename Grammar, typename Function2>
    struct is_transform<transform2::function2<Grammar, Function2> >
      : mpl::true_
    {};

    template<typename Grammar, typename Function3>
    struct is_transform<transform2::function3<Grammar, Function3> >
      : mpl::true_
    {};
}}

#endif
