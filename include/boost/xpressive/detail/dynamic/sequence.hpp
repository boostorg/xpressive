///////////////////////////////////////////////////////////////////////////////
// sequence.hpp
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_XPRESSIVE_DETAIL_DYNAMIC_SEQUENCE_HPP_EAN_04_10_2006
#define BOOST_XPRESSIVE_DETAIL_DYNAMIC_SEQUENCE_HPP_EAN_04_10_2006

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/assert.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/xpressive/detail/utility/width.hpp>
#include <boost/xpressive/detail/detail_fwd.hpp>

namespace boost { namespace xpressive { namespace detail
{

///////////////////////////////////////////////////////////////////////////////
// sequence
template<typename BidiIter>
struct sequence
{
    sequence()
      : pure_(true)
      , width_(0)
      , quant_(quant_none)
      , head_()
      , tail_(0)
      , alt_end_xpr_()
      , alternates_(0)
    {
    }

    template<typename Matcher>
    sequence(shared_ptr<dynamic_xpression<Matcher, BidiIter> > const &xpr)
      : pure_(Matcher::pure)
      , width_(xpr->Matcher::get_width())
      , quant_(Matcher::quant)
      , head_(xpr)
      , tail_(&xpr->next_)
      , alt_end_xpr_()
      , alternates_(0)
    {
    }

    template<typename Traits>
    sequence(shared_ptr<dynamic_xpression<alternate_matcher<alternates_vector<BidiIter>, Traits>, BidiIter> > const &xpr)
      : pure_(true)
      , width_(0)
      , quant_(quant_none)
      , head_(xpr)
      , tail_(&xpr->next_)
      , alt_end_xpr_()
      , alternates_(&xpr->alternates_)
    {
    }

    bool empty() const
    {
        return !this->head_;
    }

    sequence<BidiIter> &operator +=(sequence<BidiIter> const &that)
    {
        if(this->empty())
        {
            *this = that;
        }
        else if(!that.empty())
        {
            *this->tail_ = that.head_;
            this->tail_ = that.tail_;
            // keep track of sequence width and purity
            this->width_ += that.width_;
            this->pure_ = this->pure_ && that.pure_;
            this->set_quant_();
        }
        return *this;
    }

    sequence<BidiIter> &operator |=(sequence<BidiIter> const &that)
    {
        BOOST_ASSERT(!this->empty());
        BOOST_ASSERT(0 != this->alternates_);

        // through the wonders of reference counting, all alternates_ can share an end_alternate
        if(!this->alt_end_xpr_)
        {
            this->alt_end_xpr_ = make_dynamic_xpression<BidiIter>(alternate_end_matcher()).xpr();
        }

        // Keep track of width and purity
        if(this->alternates_->empty())
        {
            this->width_ = that.width_;
            this->pure_ = that.pure_;
        }
        else
        {
            this->width_ |= that.width_;
            this->pure_ = this->pure_ && that.pure_;
        }

        // terminate each alternate with an alternate_end_matcher
        if(that.empty())
        {
            BOOST_ASSERT(!that.width_ && that.pure_);
            this->alternates_->push_back(this->alt_end_xpr_);
        }
        else
        {
            this->alternates_->push_back(that.head_);
            *that.tail_ = this->alt_end_xpr_;
        }

        this->set_quant_();
        return *this;
    }

    sequence<BidiIter> quantify
    (
        quant_spec const &spec
      , alternates_factory<BidiIter> const &factory
    ) const
    {
        return this->xpr().matchable()->quantify(spec, *this, factory);
    }

    shared_matchable<BidiIter> const &xpr() const
    {
        return this->head_;
    }

    detail::width width() const
    {
        return this->width_;
    }

    bool pure() const
    {
        return this->pure_;
    }

    quant_enum quant() const
    {
        return this->quant_;
    }

private:

    void set_quant_()
    {
        this->quant_ = (!is_unknown(this->width_) && this->pure_)
          ? (!this->width_ ? quant_none : quant_fixed_width)
          : quant_variable_width;
    }

    bool pure_;
    detail::width width_;
    quant_enum quant_;
    shared_matchable<BidiIter> head_;
    shared_matchable<BidiIter> *tail_;
    shared_matchable<BidiIter> alt_end_xpr_;
    alternates_vector<BidiIter> *alternates_;
};

}}} // namespace boost::xpressive::detail

#endif
