///////////////////////////////////////////////////////////////////////////////
// modifier.hpp
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_XPRESSIVE_DETAIL_STATIC_MODIFIER_HPP_EAN_10_04_2005
#define BOOST_XPRESSIVE_DETAIL_STATIC_MODIFIER_HPP_EAN_10_04_2005

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
# pragma warning(push)
# pragma warning(disable : 4510) // default constructor could not be generated
# pragma warning(disable : 4610) // user defined constructor required
#endif

#include <boost/xpressive/detail/detail_fwd.hpp>
#include <boost/xpressive/proto/traits.hpp>
#include <boost/xpressive/regex_constants.hpp>

namespace boost { namespace xpressive { namespace detail
{

    ///////////////////////////////////////////////////////////////////////////////
    // modifier
    template<typename Modifier>
    struct modifier_op
    {
        typedef regex_constants::syntax_option_type opt_type;

        template<typename Expr>
        struct apply
          : proto::result_of::make_expr<modifier_tag, Modifier, Expr const>
        {};

        template<typename Expr>
        typename proto::result_of::make_expr<modifier_tag, Modifier, Expr const>::type
        operator ()(Expr const &expr) const
        {
            return proto::make_expr<modifier_tag>(this->mod_, expr);
        }

        operator opt_type() const
        {
            return this->opt_;
        }

        Modifier mod_;
        opt_type opt_;
    };

}}}

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma warning(pop)
#endif

#endif
