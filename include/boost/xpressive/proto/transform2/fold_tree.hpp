///////////////////////////////////////////////////////////////////////////////
/// \file fold_tree.hpp
/// A higher-level transform that uses the fold, and branch transforms
/// to recursively fold a tree.
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_TRANSFORM2_FOLD_TREE_HPP_EAN_06_18_2007
#define BOOST_PROTO_TRANSFORM2_FOLD_TREE_HPP_EAN_06_18_2007

#include <boost/xpressive/proto/detail/prefix.hpp>
#include <boost/xpressive/proto/transform2/construct.hpp>
#include <boost/xpressive/proto/transform2/fold.hpp>
#include <boost/xpressive/proto/detail/suffix.hpp>

namespace boost { namespace proto { namespace transform2
{
    namespace detail_
    {
        template<typename Grammar, typename Fun>
        struct fold_tree_
          : or_<
                case_< Grammar, fold<_children, _state, fold_tree_<Grammar, Fun> > >
              , typename as_transform<Fun>::type
            >
        {};

        template<typename Grammar, typename Fun>
        struct reverse_fold_tree_
          : or_<
                case_< Grammar, reverse_fold<_children, _state, reverse_fold_tree_<Grammar, Fun> > >
              , typename as_transform<Fun>::type
            >
        {};
    }

    template<typename Grammar, typename State, typename Fun>
    struct fold_tree
      : case_< Grammar, fold<_children, State, detail_::fold_tree_<Grammar, Fun> > >
    {};

    template<typename Grammar, typename State, typename Fun>
    struct reverse_fold_tree
      : case_< Grammar, reverse_fold<_children, State, detail_::reverse_fold_tree_<Grammar, Fun> > >
    {};

}}}

namespace boost { namespace proto
{
    template<typename Grammar, typename State, typename Fun>
    struct is_transform<transform2::fold_tree<Grammar, Fun, State> >
      : mpl::true_
    {};

    template<typename Grammar, typename State, typename Fun>
    struct is_transform<transform2::reverse_fold_tree<Grammar, Fun, State> >
      : mpl::true_
    {};
}}

#endif
