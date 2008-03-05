///////////////////////////////////////////////////////////////////////////////
// range_matcher.hpp
//
//  Copyright 2008 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_XPRESSIVE_DETAIL_CORE_MATCHER_RANGE_MATCHER_HPP_EAN_10_04_2005
#define BOOST_XPRESSIVE_DETAIL_CORE_MATCHER_RANGE_MATCHER_HPP_EAN_10_04_2005

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
# pragma warning(push)
# pragma warning(disable : 4100) // unreferenced formal parameter
#endif

#include <boost/mpl/bool.hpp>
#include <boost/xpressive/detail/detail_fwd.hpp>
#include <boost/xpressive/detail/core/quant_style.hpp>
#include <boost/xpressive/detail/core/state.hpp>
#include <boost/xpressive/detail/static/placeholders.hpp>

namespace boost { namespace xpressive { namespace detail
{

    ///////////////////////////////////////////////////////////////////////////////
    // range_matcher
    //
    template<typename Traits, typename ICase>
    struct range_matcher
      : quant_style_fixed_width<1>
    {
        typedef typename Traits::char_type char_type;
        typedef ICase icase_type;
        char_type ch_min_;
        char_type ch_max_;
        bool not_;

        range_matcher(range_placeholder<char_type> const &rng, bool no)
          : ch_min_(rng.ch_min_)
          , ch_max_(rng.ch_max_)
          , not_(no)
        {
        }

        void inverse()
        {
            this->not_ = !this->not_;
        }

        bool in_range(Traits const &traits, char_type ch, mpl::false_) const // case-sensitive
        {
            return traits.in_range(this->ch_min_, this->ch_max_, ch);
        }

        bool in_range(Traits const &traits, char_type ch, mpl::true_) const // case-insensitive
        {
            return traits.in_range_nocase(this->ch_min_, this->ch_max_, ch);
        }

        template<typename BidiIter, typename Next>
        bool match(match_state<BidiIter> &state, Next const &next) const
        {
            if(state.eos() || this->not_ ==
                this->in_range(traits_cast<Traits>(state), *state.cur_, icase_type()))
            {
                return false;
            }

            ++state.cur_;
            if(next.match(state))
            {
                return true;
            }

            --state.cur_;
            return false;
        }
    };

}}}

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma warning(pop)
#endif

#endif
