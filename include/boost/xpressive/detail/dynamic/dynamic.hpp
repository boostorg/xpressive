///////////////////////////////////////////////////////////////////////////////
// dynamic.hpp
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_XPRESSIVE_DETAIL_DYNAMIC_DYNAMIC_HPP_EAN_10_04_2005
#define BOOST_XPRESSIVE_DETAIL_DYNAMIC_DYNAMIC_HPP_EAN_10_04_2005

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <list>
#include <utility>
#include <algorithm>
#include <boost/assert.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/xpressive/detail/detail_fwd.hpp>
#include <boost/xpressive/detail/core/quant_style.hpp>
#include <boost/xpressive/detail/dynamic/matchable.hpp>
#include <boost/xpressive/detail/core/icase.hpp>

namespace boost { namespace xpressive { namespace detail
{

///////////////////////////////////////////////////////////////////////////////
// invalid_xpression
template<typename BidiIter>
struct invalid_xpression
  : dynamic_xpression_base<BidiIter>
{
    invalid_xpression()
      : dynamic_xpression_base<BidiIter>()
    {
    }
    
    bool match(state_type<BidiIter> &) const
    {
        BOOST_ASSERT(false);
        return false;
    }

    static void noop(dynamic_xpression_base<BidiIter> const *)
    {
    }
};

///////////////////////////////////////////////////////////////////////////////
// get_invalid_xpression
template<typename BidiIter>
inline shared_ptr<dynamic_xpression_base<BidiIter> const> const &get_invalid_xpression()
{
    static invalid_xpression<BidiIter> const invalid_xpr;

    static shared_ptr<dynamic_xpression_base<BidiIter> const> const invalid_ptr
    (
        static_cast<dynamic_xpression_base<BidiIter> const *>(&invalid_xpr)
      , &invalid_xpression<BidiIter>::noop
    );

    return invalid_ptr;
}

///////////////////////////////////////////////////////////////////////////////
// dynamic_xpression
template<typename Matcher, typename BidiIter>
struct dynamic_xpression
  : Matcher
  , dynamic_xpression_base<BidiIter>
{
    typedef typename iterator_value<BidiIter>::type char_type;

    shared_ptr<dynamic_xpression_base<BidiIter> const> next_;

    dynamic_xpression(Matcher const &matcher = Matcher())
      : Matcher(matcher)
      , next_(get_invalid_xpression<BidiIter>())
    {
    }

    bool match(state_type<BidiIter> &state) const
    {
        return this->Matcher::match(state, *this->next_);
    }

    void link(xpression_linker<char_type> &linker) const
    {
        linker.accept(*static_cast<Matcher const *>(this), this->next_.get());
        this->next_->link(linker);
    }

    void peek(xpression_peeker<char_type> &peeker) const
    {
        this->peek_next_(peeker.accept(*static_cast<Matcher const *>(this)), peeker);
    }

    sequence<BidiIter> quantify
    (
        quant_spec const &spec
      , std::size_t &hidden_mark_count
      , sequence<BidiIter> seq
      , alternates_factory<BidiIter> const &factory
    ) const
    {
        return this->quantify_(spec, hidden_mark_count, seq, quant_type<Matcher>(), factory, this);
    }

    bool is_quantifiable() const
    {
        return quant_type<Matcher>::value != (int)quant_none;
    }

private:

    void peek_next_(mpl::true_, xpression_peeker<char_type> &peeker) const
    {
        this->next_->peek(peeker);
    }

    void peek_next_(mpl::false_, xpression_peeker<char_type> &) const
    {
        // no-op
    }

    sequence<BidiIter> quantify_
    (
        quant_spec const &
      , std::size_t &
      , sequence<BidiIter>
      , mpl::int_<quant_none>
      , alternates_factory<BidiIter> const &
      , void const *
    ) const;

    sequence<BidiIter> quantify_
    (
        quant_spec const &
      , std::size_t &
      , sequence<BidiIter>
      , mpl::int_<quant_fixed_width>
      , alternates_factory<BidiIter> const &
      , void const *
    ) const;

    sequence<BidiIter> quantify_
    (
        quant_spec const &
      , std::size_t &
      , sequence<BidiIter>
      , mpl::int_<quant_variable_width>
      , alternates_factory<BidiIter> const &
      , void const *
    ) const;

    sequence<BidiIter> quantify_
    (
        quant_spec const &
      , std::size_t &
      , sequence<BidiIter>
      , mpl::int_<quant_fixed_width>
      , alternates_factory<BidiIter> const &
      , mark_begin_matcher const *
    ) const;
};

///////////////////////////////////////////////////////////////////////////////
// make_dynamic_xpression
template<typename BidiIter, typename Matcher>
inline sequence<BidiIter> make_dynamic_xpression(Matcher const &matcher)
{
    typedef dynamic_xpression<Matcher, BidiIter> xpression_type;
    std::auto_ptr<xpression_type> xpr(new xpression_type(matcher));

    sequence<BidiIter> seq;
    seq.width_of = xpr->Matcher::get_width();
    seq.is_pure = Matcher::pure::value;
    seq.second = &xpr->next_;
    seq.first = xpr; // transfer ownership

    return seq;
}

///////////////////////////////////////////////////////////////////////////////
// alternates_vector
template<typename BidiIter>
struct alternates_vector
  : std::vector<shared_ptr<dynamic_xpression_base<BidiIter> const> >
{
};

///////////////////////////////////////////////////////////////////////////////
// alternates_factory
template<typename BidiIter>
struct alternates_factory
{
    virtual ~alternates_factory() {}
    virtual std::pair<sequence<BidiIter>, alternates_vector<BidiIter> *>
    operator ()() const = 0;
};

template<typename BidiIter, typename Traits>
struct alternates_factory_impl
  : alternates_factory<BidiIter>
{
    std::pair<sequence<BidiIter>, alternates_vector<BidiIter> *>
    operator ()() const
    {
        typedef alternate_matcher<alternates_vector<BidiIter>, Traits> alternate_matcher;
        typedef dynamic_xpression<alternate_matcher, BidiIter> alternate_xpression;
        shared_ptr<alternate_xpression> alt_xpr(new alternate_xpression);
        sequence<BidiIter> seq(alt_xpr, &alt_xpr->next_);
        return std::make_pair(seq, &alt_xpr->alternates_);
    }
};

///////////////////////////////////////////////////////////////////////////////
// alternates_to_matchable
template<typename BidiIter>
inline sequence<BidiIter> alternates_to_matchable
(
    std::list<sequence<BidiIter> > const &alternates
  , alternates_factory<BidiIter> const &factory
)
{
    BOOST_ASSERT(0 != alternates.size());

    // If there is only 1 alternate, just return it.
    if(1 == alternates.size())
    {
        return alternates.front();
    }

    std::pair<sequence<BidiIter>, alternates_vector<BidiIter> *> result = factory();

    // through the wonders of reference counting, all alternates can share an end_alternate
    typedef dynamic_xpression<alternate_end_matcher, BidiIter> alternate_end_xpression;
    shared_ptr<alternate_end_xpression> end_alt_xpr(new alternate_end_xpression);

    // terminate each alternate with an alternate_end_matcher
    result.second->reserve(alternates.size());
    typedef std::list<sequence<BidiIter> > alternates_list;
    typename alternates_list::const_iterator begin = alternates.begin(), end = alternates.end();
    result.first.width_of = begin->width_of;
    result.first.is_pure = begin->is_pure;
    for(; begin != end; ++begin)
    {
        if(!begin->is_empty())
        {
            result.second->push_back(begin->first);
            *begin->second = end_alt_xpr;
            if(result.first.width_of != begin->width_of)
                result.first.width_of = unknown_width();
            result.first.is_pure = result.first.is_pure && begin->is_pure;
        }
        else
        {
            result.second->push_back(end_alt_xpr);
            if(result.first.width_of != 0)
                result.first.width_of = unknown_width();
        }
    }

    //result.second->width_ = result.first.width_of;
    return result.first;
}

///////////////////////////////////////////////////////////////////////////////
// matcher_wrapper
template<typename Matcher>
struct matcher_wrapper
  : Matcher
{
    matcher_wrapper(Matcher const &matcher = Matcher())
      : Matcher(matcher)
    {
    }

    template<typename BidiIter>
    bool match(state_type<BidiIter> &state) const
    {
        return this->Matcher::match(state, matcher_wrapper<true_matcher>());
    }

    template<typename Char>
    void link(xpression_linker<Char> &linker) const
    {
        linker.accept(*static_cast<Matcher const *>(this), 0);
    }

    template<typename Char>
    void peek(xpression_peeker<Char> &peeker) const
    {
        peeker.accept(*static_cast<Matcher const *>(this));
    }
};

//////////////////////////////////////////////////////////////////////////
// dynamic_xpression::quantify_
//   unquantifiable
template<typename Matcher, typename BidiIter>
inline sequence<BidiIter> dynamic_xpression<Matcher, BidiIter>::quantify_
(
    quant_spec const &
  , std::size_t &
  , sequence<BidiIter>
  , mpl::int_<quant_none>
  , alternates_factory<BidiIter> const &
  , void const *
) const
{
    BOOST_ASSERT(false); // should never get here
    throw regex_error(regex_constants::error_badrepeat, "expression cannot be quantified");
}

//   fixed-width matchers
template<typename Matcher, typename BidiIter>
inline sequence<BidiIter> dynamic_xpression<Matcher, BidiIter>::quantify_
(
    quant_spec const &spec
  , std::size_t &hidden_mark_count
  , sequence<BidiIter> seq
  , mpl::int_<quant_fixed_width>
  , alternates_factory<BidiIter> const &factory
  , void const *
) const
{
    if(this->next_ != get_invalid_xpression<BidiIter>())
    {
        return this->quantify_(spec, hidden_mark_count, seq, mpl::int_<quant_variable_width>(), factory, this);
    }

    typedef matcher_wrapper<Matcher> xpr_type;
    BOOST_ASSERT(seq.width_of == this->Matcher::get_width());

    if(spec.greedy_)
    {
        simple_repeat_matcher<xpr_type, true> quant(*this, spec.min_, spec.max_, seq.width_of);
        return make_dynamic_xpression<BidiIter>(quant);
    }
    else
    {
        simple_repeat_matcher<xpr_type, false> quant(*this, spec.min_, spec.max_, seq.width_of);
        return make_dynamic_xpression<BidiIter>(quant);
    }
}

//   variable-width, no mark
template<typename Matcher, typename BidiIter>
inline sequence<BidiIter> dynamic_xpression<Matcher, BidiIter>::quantify_
(
    quant_spec const &spec
  , std::size_t &hidden_mark_count
  , sequence<BidiIter> seq
  , mpl::int_<quant_variable_width>
  , alternates_factory<BidiIter> const &factory
  , void const *
) const
{
    // create a hidden mark so this expression can be quantified
    int mark_nbr = -static_cast<int>(++hidden_mark_count);
    mark_begin_matcher mark_begin(mark_nbr);
    mark_end_matcher mark_end(mark_nbr);
    sequence<BidiIter> new_seq = make_dynamic_xpression<BidiIter>(mark_begin);
    new_seq += seq;
    new_seq += make_dynamic_xpression<BidiIter>(mark_end);
    return new_seq.first->quantify(spec, hidden_mark_count, new_seq, factory);
}

//   variable-width with mark
template<typename Matcher, typename BidiIter>
inline sequence<BidiIter> dynamic_xpression<Matcher, BidiIter>::quantify_
(
    quant_spec const &spec
  , std::size_t &
  , sequence<BidiIter> seq
  , mpl::int_<quant_fixed_width>
  , alternates_factory<BidiIter> const &factory
  , mark_begin_matcher const *
) const
{
    BOOST_ASSERT(spec.max_); // we should never get here if max is 0
    int mark_number = this->mark_number_;

    // only bother creating a quantifier if max is greater than one
    if(1 < spec.max_)
    {
        unsigned int min = spec.min_ ? spec.min_ : 1U;
        detail::sequence<BidiIter> seq_quant;
        // TODO: statically bind the repeat_begin_matcher to the mark_begin for better perf
        seq_quant += make_dynamic_xpression<BidiIter>(repeat_begin_matcher(mark_number));
        // TODO: statically bind the mark_end to the quantifier_end for better perf
        if(spec.greedy_)
        {
            repeat_end_matcher<true> end_quant(mark_number, min, spec.max_);
            seq += make_dynamic_xpression<BidiIter>(end_quant);
        }
        else
        {
            repeat_end_matcher<false> end_quant(mark_number, min, spec.max_);
            seq += make_dynamic_xpression<BidiIter>(end_quant);
        }
        seq_quant += seq;
        seq = seq_quant;
    }

    // if min is 0, the quant must be made alternate with an empty matcher.
    if(0 == spec.min_)
    {
        epsilon_mark_matcher mark(mark_number);
        std::list<sequence<BidiIter> > alts;
        alts.push_back(make_dynamic_xpression<BidiIter>(mark));
        alts.push_back(seq);
        if(spec.greedy_)
            alts.reverse();
        seq = alternates_to_matchable(alts, factory);
    }

    return seq;
}

}}} // namespace boost::xpressive::detail

#endif
