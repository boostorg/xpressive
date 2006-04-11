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
#include <boost/type_traits/is_same.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/xpressive/detail/detail_fwd.hpp>
#include <boost/xpressive/detail/core/quant_style.hpp>
#include <boost/xpressive/detail/dynamic/matchable.hpp>
#include <boost/xpressive/detail/dynamic/sequence.hpp>
#include <boost/xpressive/detail/core/icase.hpp>

namespace boost { namespace xpressive { namespace detail
{

///////////////////////////////////////////////////////////////////////////////
// invalid_xpression
template<typename BidiIter>
struct invalid_xpression
  : matchable_ex<BidiIter>
{
    invalid_xpression()
      : matchable_ex<BidiIter>()
    {
    }

    bool match(state_type<BidiIter> &) const
    {
        BOOST_ASSERT(false);
        return false;
    }

    static void noop(matchable_ex<BidiIter> const *)
    {
    }
};

///////////////////////////////////////////////////////////////////////////////
// get_invalid_xpression
template<typename BidiIter>
inline shared_matchable<BidiIter> const &get_invalid_xpression()
{
    static invalid_xpression<BidiIter> const invalid_xpr;
    static shared_ptr<matchable_ex<BidiIter> const> const invalid_ptr
    (
        static_cast<matchable_ex<BidiIter> const *>(&invalid_xpr)
      , &invalid_xpression<BidiIter>::noop
    );
    static shared_matchable<BidiIter> const invalid_matchable(invalid_ptr);
    return invalid_matchable;
}

///////////////////////////////////////////////////////////////////////////////
// dynamic_xpression
template<typename Matcher, typename BidiIter>
struct dynamic_xpression
  : Matcher
  , matchable_ex<BidiIter>
{
    typedef typename iterator_value<BidiIter>::type char_type;

    dynamic_xpression(Matcher const &matcher = Matcher())
      : Matcher(matcher)
      , next_(get_invalid_xpression<BidiIter>())
    {
    }

    bool match(state_type<BidiIter> &state) const
    {
        return this->Matcher::match(state, *this->next_.matchable());
    }

    void link(xpression_linker<char_type> &linker) const
    {
        linker.accept(*static_cast<Matcher const *>(this), this->next_.matchable().get());
        this->next_.link(linker);
    }

    void peek(xpression_peeker<char_type> &peeker) const
    {
        this->peek_next_(peeker.accept(*static_cast<Matcher const *>(this)), peeker);
    }

    sequence<BidiIter> repeat(quant_spec const &spec, sequence<BidiIter> const &seq) const
    {
        return this->repeat_(spec, seq, quant_type<Matcher>(), is_same<Matcher, mark_begin_matcher>());
    }

private:
    friend struct sequence<BidiIter>;

    void peek_next_(mpl::true_, xpression_peeker<char_type> &peeker) const
    {
        this->next_.peek(peeker);
    }

    void peek_next_(mpl::false_, xpression_peeker<char_type> &) const
    {
        // no-op
    }

    sequence<BidiIter> repeat_
    (
        quant_spec const &
      , sequence<BidiIter> const &
      , mpl::int_<quant_none>
      , mpl::false_
    ) const
    {
        BOOST_ASSERT(false); // should never get here
        throw regex_error(regex_constants::error_badrepeat, "expression cannot be quantified");
    }

    sequence<BidiIter> repeat_
    (
        quant_spec const &spec
      , sequence<BidiIter> const &seq
      , mpl::int_<quant_fixed_width>
      , mpl::false_
    ) const
    {
        if(this->next_ == get_invalid_xpression<BidiIter>())
        {
            return make_simple_repeat(spec, seq, matcher_wrapper<Matcher>(*this));
        }
        else if(!is_unknown(seq.width()) && seq.pure())
        {
            return make_simple_repeat(spec, seq);
        }
        else
        {
            return make_repeat(spec, seq);
        }
    }

    sequence<BidiIter> repeat_
    (
        quant_spec const &spec
      , sequence<BidiIter> const &seq
      , mpl::int_<quant_variable_width>
      , mpl::false_
    ) const
    {
        if(!is_unknown(seq.width()) && seq.pure())
        {
            return make_simple_repeat(spec, seq);
        }
        else
        {
            return make_repeat(spec, seq);
        }
    }

    sequence<BidiIter> repeat_
    (
        quant_spec const &spec
      , sequence<BidiIter> const &seq
      , mpl::int_<quant_fixed_width>
      , mpl::true_
    ) const
    {
        return make_repeat(spec, seq, this->mark_number_);
    }

    shared_matchable<BidiIter> next_;
};

///////////////////////////////////////////////////////////////////////////////
// make_dynamic_xpression
template<typename BidiIter, typename Matcher>
inline sequence<BidiIter> make_dynamic_xpression(Matcher const &matcher)
{
    typedef dynamic_xpression<Matcher, BidiIter> xpression_type;
    shared_ptr<xpression_type> xpr(new xpression_type(matcher));
    return sequence<BidiIter>(xpr);
}

///////////////////////////////////////////////////////////////////////////////
// alternates_vector
template<typename BidiIter>
struct alternates_vector
{
    BOOST_STATIC_CONSTANT(std::size_t, width = unknown_width::value);
    BOOST_STATIC_CONSTANT(bool, pure = false);

    struct to_matchable
      : std::unary_function<shared_matchable<BidiIter>, matchable_ex<BidiIter> const &>
    {
        matchable_ex<BidiIter> const &operator ()(shared_matchable<BidiIter> const &that) const
        {
            return *that.matchable();
        }
    };

    typedef typename std::vector<shared_matchable<BidiIter> >::const_iterator vector_iterator;
    typedef transform_iterator<to_matchable, vector_iterator> const_iterator;

    const_iterator begin() const
    {
        return const_iterator(this->alternates_.begin(), to_matchable());
    }

    const_iterator end() const
    {
        return const_iterator(this->alternates_.end(), to_matchable());
    }

    bool empty() const
    {
        return this->alternates_.empty();
    }

    void push_back(shared_matchable<BidiIter> const &value)
    {
        this->alternates_.push_back(value);
    }

private:
    std::vector<shared_matchable<BidiIter> > alternates_;
};

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
// make_simple_repeat
template<typename BidiIter, typename Xpr>
inline sequence<BidiIter>
make_simple_repeat(quant_spec const &spec, sequence<BidiIter> const &seq, Xpr const &xpr)
{
    if(spec.greedy_)
    {
        simple_repeat_matcher<Xpr, true> quant(xpr, spec.min_, spec.max_, seq.width().value());
        return make_dynamic_xpression<BidiIter>(quant);
    }
    else
    {
        simple_repeat_matcher<Xpr, false> quant(xpr, spec.min_, spec.max_, seq.width().value());
        return make_dynamic_xpression<BidiIter>(quant);
    }
}

//////////////////////////////////////////////////////////////////////////
// make_simple_repeat
template<typename BidiIter>
inline sequence<BidiIter>
make_simple_repeat(quant_spec const &spec, sequence<BidiIter> seq)
{
    seq += make_dynamic_xpression<BidiIter>(true_matcher());
    return make_simple_repeat(spec, seq, seq.xpr());
}

//////////////////////////////////////////////////////////////////////////
// make_repeat
template<typename BidiIter>
inline sequence<BidiIter>
make_repeat(quant_spec const &spec, sequence<BidiIter> seq)
{
    // create a hidden mark so this expression can be quantified
    int mark_nbr = -static_cast<int>(++*spec.hidden_mark_count_);
    mark_begin_matcher mark_begin(mark_nbr);
    mark_end_matcher mark_end(mark_nbr);
    seq = (make_dynamic_xpression<BidiIter>(mark_begin) += seq);
    seq += make_dynamic_xpression<BidiIter>(mark_end);
    return make_repeat(spec, seq, mark_nbr);
}

//////////////////////////////////////////////////////////////////////////
// make_repeat
template<typename BidiIter>
inline sequence<BidiIter>
make_repeat(quant_spec const &spec, sequence<BidiIter> seq, int mark_nbr)
{
    BOOST_ASSERT(spec.max_); // we should never get here if max is 0

    // only bother creating a quantifier if max is greater than one
    if(1 < spec.max_)
    {
        unsigned int min = spec.min_ ? spec.min_ : 1U;
        detail::sequence<BidiIter> seq_quant;
        // TODO: statically bind the repeat_begin_matcher to the mark_begin_matcher for better perf
        seq_quant += make_dynamic_xpression<BidiIter>(repeat_begin_matcher(mark_nbr));
        // TODO: statically bind the mark_end_matcher to the repeat_end_matcher for better perf
        if(spec.greedy_)
        {
            repeat_end_matcher<true> end_quant(mark_nbr, min, spec.max_);
            seq += make_dynamic_xpression<BidiIter>(end_quant);
        }
        else
        {
            repeat_end_matcher<false> end_quant(mark_nbr, min, spec.max_);
            seq += make_dynamic_xpression<BidiIter>(end_quant);
        }
        seq_quant += seq;
        seq = seq_quant;
    }

    // if min is 0, the quant must be made alternate with an empty matcher.
    if(0 == spec.min_)
    {
        typedef shared_matchable<BidiIter> xpr_type;
        seq += make_dynamic_xpression<BidiIter>(alternate_end_matcher());
        if(spec.greedy_)
        {
            seq = make_dynamic_xpression<BidiIter>(optional_mark_matcher<xpr_type, true>(seq.xpr(), mark_nbr));
        }
        else
        {
            seq = make_dynamic_xpression<BidiIter>(optional_mark_matcher<xpr_type, false>(seq.xpr(), mark_nbr));
        }
    }

    return seq;
}

}}} // namespace boost::xpressive::detail

#endif
