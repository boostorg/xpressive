///////////////////////////////////////////////////////////////////////////////
/// \file apply.hpp
/// Contains definition of the apply<> transform.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_TRANSFORM_APPLY_HPP_EAN_11_02_2007
#define BOOST_PROTO_TRANSFORM_APPLY_HPP_EAN_11_02_2007

#include <boost/type_traits.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/traits.hpp>

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
        struct apply<Trans>
          : Trans
        {};

        template<typename Trans, typename ExprTfx>
        struct apply<Trans, ExprTfx> : raw_transform
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
                : boost::result_of<Trans(
                    typename boost::result_of<when<_, ExprTfx>(Expr, State, Visitor)>::type
                  , State
                  , Visitor
                )>
            {};

            template<typename Expr, typename State, typename Visitor>
            typename result<apply(Expr, State, Visitor)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                return Trans()(
                    when<_, ExprTfx>()(expr, state, visitor)
                  , state
                  , visitor
                );
            }
        };

        template<typename Trans, typename ExprTfx, typename StateTfx>
        struct apply<Trans, ExprTfx, StateTfx> : raw_transform
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
              : boost::result_of<Trans(
                    typename boost::result_of<when<_, ExprTfx>(Expr, State, Visitor)>::type
                  , typename boost::result_of<when<_, StateTfx>(Expr, State, Visitor)>::type
                  , Visitor
                )>
            {};

            template<typename Expr, typename State, typename Visitor>
            typename result<apply(Expr, State, Visitor)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                return Trans()(
                    when<_, ExprTfx>()(expr, state, visitor)
                  , when<_, StateTfx>()(expr, state, visitor)
                  , visitor
                );
            }
        };

        template<typename Trans, typename ExprTfx, typename StateTfx, typename VisitorTfx>
        struct apply<Trans, ExprTfx, StateTfx, VisitorTfx> : raw_transform
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
              : boost::result_of<Trans(
                    typename boost::result_of<when<_, ExprTfx>(Expr, State, Visitor)>::type
                  , typename boost::result_of<when<_, StateTfx>(Expr, State, Visitor)>::type
                  , typename boost::result_of<when<_, VisitorTfx>(Expr, State, Visitor)>::type
                )>
            {};

            template<typename Expr, typename State, typename Visitor>
            typename result<apply(Expr, State, Visitor)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                typedef typename boost::result_of<when<_, VisitorTfx>(Expr, State, Visitor)>::type visitor_type;
                return Trans()(
                    when<_, ExprTfx>()(expr, state, visitor)
                  , when<_, StateTfx>()(expr, state, visitor)
                  , const_cast<visitor_type &>(as_lvalue(when<_, VisitorTfx>()(expr, state, visitor)))
                );
            }
        };

    }

    template<typename Trans, typename... Args>
    struct transform_category<transform::apply<Trans, Args...> >
    {
        typedef raw_transform type;
    };

}}

#endif
