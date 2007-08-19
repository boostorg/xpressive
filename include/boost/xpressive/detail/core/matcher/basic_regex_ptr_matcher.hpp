///////////////////////////////////////////////////////////////////////////////
// basic_regex_ptr_matcher
//
//  Copyright 2007 Andreas Pokorny. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_XPRESSIVE_DETAIL_CORE_MATCHER_BASIC_REGEX_PTR_MATCHER_HPP_INCLUDED

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/assert.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/xpressive/regex_error.hpp>
#include <boost/xpressive/regex_constants.hpp>
#include <boost/xpressive/detail/detail_fwd.hpp>
#include <boost/xpressive/detail/core/quant_style.hpp>
#include <boost/xpressive/detail/core/state.hpp>
#include <boost/xpressive/detail/core/regex_impl.hpp>
#include <boost/xpressive/detail/core/adaptor.hpp>
#include <boost/xpressive/detail/core/access.hpp>

namespace boost { namespace xpressive { namespace detail
{
    ///////////////////////////////////////////////////////////////////////////////
    // basic_regex_ptr_matcher 
    //
    template<typename BidiIter>
    struct basic_regex_ptr_matcher
      : quant_style<quant_variable_width, unknown_width::value, false>
    {
        basic_regex<BidiIter> const * pimpl_;

        basic_regex_ptr_matcher(basic_regex<BidiIter> const* ptr)
          :  pimpl_(ptr)
        {
            BOOST_ASSERT(this->pimpl_);
        }

        template<typename Next>
        bool match(match_state<BidiIter> &state, Next const &next) const
        {
            ensure(core_access<BidiIter>::get_regex_impl(*this->pimpl_)->xpr_, regex_constants::error_badref, "bad regex reference");

            return push_context_match(
                *core_access<BidiIter>::get_regex_impl(*this->pimpl_), 
                state, 
                this->wrap_(next, is_static_xpression<Next>())
                );
        }

    private:
        template<typename Next>
        static xpression_adaptor<reference_wrapper<Next const>, matchable<BidiIter> > wrap_(Next const &next, mpl::true_)
        {
            // wrap the static xpression in a matchable interface
            return xpression_adaptor<reference_wrapper<Next const>, matchable<BidiIter> >(boost::cref(next));
        }

        template<typename Next>
        static Next const &wrap_(Next const &next, mpl::false_)
        {
            return next;
        }
    };

}}}

#endif
