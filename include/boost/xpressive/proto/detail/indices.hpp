///////////////////////////////////////////////////////////////////////////////
/// \file indices.hpp
/// Definintion of indices<>, a template for generating an
/// argument pack of integers 1,2,3...
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_DETAIL_INDICES_HPP_EAN_12_04_2007
#define BOOST_PROTO_DETAIL_INDICES_HPP_EAN_12_04_2007

namespace boost { namespace proto { namespace detail
{
    template<int... I>
    struct indices
    {};

    template<int N, int... I>
    struct make_indices<N, indices<I...> >
      : make_indices<N-4, indices<N-4, N-3, N-2, N-1, I...> >
    {};

    template<int... I>
    struct make_indices<3, indices<I...> >
    {
        typedef indices<0, 1, 2, I...> type;
    };

    template<int... I>
    struct make_indices<2, indices<I...> >
    {
        typedef indices<0, 1, I...> type;
    };

    template<int... I>
    struct make_indices<1, indices<I...> >
    {
        typedef indices<0, I...> type;
    };

    template<int... I>
    struct make_indices<0, indices<I...> >
    {
        typedef indices<I...> type;
    };

}}}

#endif
