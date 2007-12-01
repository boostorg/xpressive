///////////////////////////////////////////////////////////////////////////////
/// \file fold.hpp
/// Contains definition of the fold_tree<> and reverse_fold_tree<> transforms.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_TRANSFORM_FOLD_TREE_HPP_EAN_11_05_2007
#define BOOST_PROTO_TRANSFORM_FOLD_TREE_HPP_EAN_11_05_2007

#include <boost/mpl/placeholders.hpp>
#include <boost/type_traits.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/traits.hpp>
#include <boost/xpressive/proto/transform/fold.hpp>

namespace boost { namespace proto
{

    namespace transform
    {

        namespace detail
        {

            template<typename Grammar, typename Fun>
            struct fold_tree_
              : or_<
                    when<Grammar, fold<_, _state, fold_tree_<Grammar, Fun> > >
                  , when<_, Fun>
                >
            {};

            template<typename Grammar, typename Fun>
            struct reverse_fold_tree_
              : or_<
                    when<Grammar, reverse_fold<_, _state, reverse_fold_tree_<Grammar, Fun> > >
                  , when<_, Fun>
                >
            {};

        }

        template<typename Sequence, typename State0, typename Fun>
        struct fold_tree
          : raw_transform
        {
            template<typename Expr, typename State, typename Visitor>
            struct apply
            {
                typedef fold<
                    Sequence
                  , State0
                  , detail::fold_tree_<
                        nary_expr<typename Expr::proto_tag, vararg<_> >
                      , Fun
                    >
                > impl;

                typedef typename impl::template apply<Expr, State, Visitor>::type type;
            };

            template<typename Expr, typename State, typename Visitor>
            static typename apply<Expr, State, Visitor>::type
            call(Expr const &expr, State const &state, Visitor &visitor)
            {
                return apply<Expr, State, Visitor>::impl::call(expr, state, visitor);
            }
        };

        template<typename Sequence, typename State0, typename Fun>
        struct reverse_fold_tree
          : raw_transform
        {
            template<typename Expr, typename State, typename Visitor>
            struct apply
            {
                typedef reverse_fold<
                    Sequence
                  , State0
                  , detail::reverse_fold_tree_<
                        nary_expr<typename Expr::proto_tag, vararg<_> >
                      , Fun
                    >
                > impl;

                typedef typename impl::template apply<Expr, State, Visitor>::type type;
            };

            template<typename Expr, typename State, typename Visitor>
            static typename apply<Expr, State, Visitor>::type
            call(Expr const &expr, State const &state, Visitor &visitor)
            {
                return apply<Expr, State, Visitor>::impl::call(expr, state, visitor);
            }
        };
    }

    template<typename Sequence, typename State0, typename Fun>
    struct transform_category<transform::fold_tree<Sequence, State0, Fun> >
    {
        typedef raw_transform type;
    };

    template<typename Grammar, typename Fun>
    struct transform_category<transform::detail::fold_tree_<Grammar, Fun> >
    {
        typedef raw_transform type;
    };

    template<typename Sequence, typename State0, typename Fun>
    struct transform_category<transform::reverse_fold_tree<Sequence, State0, Fun> >
    {
        typedef raw_transform type;
    };

    template<typename Grammar, typename Fun>
    struct transform_category<transform::detail::reverse_fold_tree_<Grammar, Fun> >
    {
        typedef raw_transform type;
    };

}}

#endif
