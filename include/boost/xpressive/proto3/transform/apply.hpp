///////////////////////////////////////////////////////////////////////////////
/// \file apply.hpp
/// Contains definition of the apply_<> transform.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO3_TRANSFORM_APPLY_HPP_EAN_11_02_2007
#define BOOST_PROTO3_TRANSFORM_APPLY_HPP_EAN_11_02_2007

#include <boost/type_traits.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/xpressive/proto3/proto_fwd.hpp>
#include <boost/xpressive/proto3/traits.hpp>

namespace boost { namespace proto
{

    namespace transform
    {
        template<typename T>
        typename add_reference<T>::type as_lvalue(T &&t)
        {
            return t;
        }

        template<typename Trans>
        struct apply_<Trans>
          : Trans
        {};

        template<typename Trans, typename ExprTfx>
        struct apply_<Trans, ExprTfx> : raw_transform
        {
            template<typename Expr, typename State, typename Visitor>
            struct apply
              : Trans::template apply<
                    typename case_<_, ExprTfx>::template apply<Expr, State, Visitor>::type
                  , State
                  , Visitor
                >
            {};

            template<typename Expr, typename State, typename Visitor>
            static typename apply<Expr, State, Visitor>::type
            call(Expr const &expr, State const &state, Visitor &visitor)
            {
                return Trans::call(
                    case_<_, ExprTfx>::call(expr, state, visitor)
                  , state
                  , visitor
                );
            }
        };

        template<typename Trans, typename ExprTfx, typename StateTfx>
        struct apply_<Trans, ExprTfx, StateTfx> : raw_transform
        {
            template<typename Expr, typename State, typename Visitor>
            struct apply
              : Trans::template apply<
                    typename case_<_, ExprTfx>::template apply<Expr, State, Visitor>::type
                  , typename case_<_, StateTfx>::template apply<Expr, State, Visitor>::type
                  , Visitor
                >
            {};

            template<typename Expr, typename State, typename Visitor>
            static typename apply<Expr, State, Visitor>::type
            call(Expr const &expr, State const &state, Visitor &visitor)
            {
                return Trans::call(
                    case_<_, ExprTfx>::call(expr, state, visitor)
                  , case_<_, StateTfx>::call(expr, state, visitor)
                  , visitor
                );
            }
        };

        template<typename Trans, typename ExprTfx, typename StateTfx, typename VisitorTfx>
        struct apply_<Trans, ExprTfx, StateTfx, VisitorTfx> : raw_transform
        {
            template<typename Expr, typename State, typename Visitor>
            struct apply
              : Trans::template apply<
                    typename case_<_, ExprTfx>::template apply<Expr, State, Visitor>::type
                  , typename case_<_, StateTfx>::template apply<Expr, State, Visitor>::type
                  , typename case_<_, VisitorTfx>::template apply<Expr, State, Visitor>::type
                >
            {};

            template<typename Expr, typename State, typename Visitor>
            static typename apply<Expr, State, Visitor>::type
            call(Expr const &expr, State const &state, Visitor &visitor)
            {
                typedef typename case_<_, VisitorTfx>::template apply<Expr, State, Visitor>::type visitor_type;
                return Trans::call(
                    case_<_, ExprTfx>::call(expr, state, visitor)
                  , case_<_, StateTfx>::call(expr, state, visitor)
                  , const_cast<visitor_type &>(as_lvalue(case_<_, VisitorTfx>::call(expr, state, visitor)))
                );
            }
        };

    }

    template<typename Trans, typename... Args>
    struct transform_category<transform::apply_<Trans, Args...> >
    {
        typedef raw_transform type;
    };

}}

#endif
