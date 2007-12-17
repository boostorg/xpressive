///////////////////////////////////////////////////////////////////////////////
/// \file cat_args.hpp
/// Definintion of cat_args, for concatenating arg lists
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_DETAIL_CAT_ARGS_HPP_EAN_11_07_2007
#define BOOST_PROTO_DETAIL_CAT_ARGS_HPP_EAN_11_07_2007

#include <boost/xpressive/proto/proto_fwd.hpp>

namespace boost { namespace proto
{
    namespace detail
    {

    #if defined(BOOST_HAS_VARIADIC_TMPL) && defined(BOOST_HAS_RVALUE_REFS)
        template<typename A0, typename A1>
        struct cat_args;

        template<typename... A0, typename... A1>
        struct cat_args<args<A0...>, args<A1...> >
        {
            typedef args<A0..., A1...> type;
        };
    #endif
    }
}}

#endif
