///////////////////////////////////////////////////////////////////////////////
/// \file deep_copy.hpp
/// The proto2 expression template compiler and supporting utilities.
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO2_DEEP_COPY_HPP_EAN_05_08_2006
#define BOOST_PROTO2_DEEP_COPY_HPP_EAN_05_08_2006

#include <boost/mpl/if.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/xpressive/proto2/proto.hpp>
#include <boost/fusion/sequence/view/transform_view.hpp>

namespace boost { namespace proto2
{

    struct deep_copy_transform
    {
        // N-ary nodes handled here
        template<typename Node, typename State, typename Visitor>
        struct apply;

        template<typename Arg, typename OpTag, typename State, typename Visitor>
        struct apply<unary_op<Arg, OpTag>, State, Visitor>
        {
            typename unary_op<
                typename remove_cv<typename remove_reference<Arg>::type>::type
              , OpTag
            > type;
        };

        template<typename Left, typename Right, typename OpTag, typename State, typename Visitor>
        struct apply<binary_op<Left, Right, OpTag>, State, Visitor>
          : apply<binary_op<Left, Right, OpTag>, State, Visitor>
        {
            typename binary_op<
                typename remove_cv<typename remove_reference<Left>::type>::type
              , typename remove_cv<typename remove_reference<Right>::type>::type
              , OpTag
            > type;
        };

        template<typename Functor, typename Args, typename State, typename Visitor>
        struct apply<nary_op<Functor, Args>, State, Visitor>
        {
            typedef nary_op<
                typename remove_cv<typename remove_reference<Functor>::type>::type
              , typename fusion::result_of::as_vector<
                    fusion::transform_view<Args, remove_cv<remove_reference<mpl::_> > >
                >::type
            > type;
        };

        template<typename Node, typename State, typename Visitor>
        struct call_result
        {
            typedef typename apply<Node, State, Visitor>::type result_type;
            typedef typename mpl::if_<
                is_same<result_type, Node>
              , Node const &
              , result_type
            >::type type;
        };

        template<typename Node, typename State, typename Visitor>
        static typename call_result<Node, State, Visitor>::type
        call(Node const &node, State const &, Visitor &)
        {
            return node;
        }
    };

}}

#endif
