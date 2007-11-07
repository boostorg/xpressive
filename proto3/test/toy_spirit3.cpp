///////////////////////////////////////////////////////////////////////////////
// toy_spirit3.cpp
//
//  Copyright 2006 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cctype>
#include <string>
#include <iomanip>
#include <iostream>
#include <boost/assert.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/xpressive/proto3/proto.hpp>
#include <boost/xpressive/proto3/transform2.hpp>
#if BOOST_VERSION < 103500
# include <boost/spirit/fusion/algorithm/for_each.hpp>
# include <boost/spirit/fusion/algorithm/fold.hpp>
# include <boost/spirit/fusion/algorithm/any.hpp>
#else
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/fold.hpp>
#include <boost/fusion/include/any.hpp>
#endif
#include <boost/test/unit_test.hpp>

namespace boost
{
    // global tags
    struct char_tag {};
    struct space_tag {};

    // global primitives
    proto::terminal<char_tag>::type const char_ = {{}};
    proto::terminal<space_tag>::type const space = {{}};

    using proto::lit;
    using proto::literal;
}

namespace boost { namespace spirit2
{
    namespace utility
    {
        inline bool char_icmp(char ch, char lo, char hi)
        {
            return ch == lo || ch == hi;
        }

        template<typename FwdIter>
        inline bool string_cmp(char const *sz, FwdIter &begin, FwdIter end)
        {
            FwdIter tmp = begin;
            for(; *sz; ++tmp, ++sz)
                if(tmp == end || *tmp != *sz)
                    return false;
            begin = tmp;
            return true;
        }

        template<typename FwdIter>
        inline bool string_icmp(std::string const &str, FwdIter &begin, FwdIter end)
        {
            BOOST_ASSERT(0 == str.size() % 2);
            FwdIter tmp = begin;
            std::string::const_iterator istr = str.begin(), estr = str.end();
            for(; istr != estr; ++tmp, istr += 2)
                if(tmp == end || *tmp != *istr && *tmp != *(istr+1))
                    return false;
            begin = tmp;
            return true;
        }

        inline bool in_range(char ch, char lo, char hi)
        {
            return ch >= lo && ch <= hi;
        }

        inline bool in_irange(char ch, char lo, char hi)
        {
            return in_range(ch, lo, hi)
                || in_range(std::tolower(ch), lo, hi)
                || in_range(std::toupper(ch), lo, hi);
        }

        inline std::string to_istr(char const *sz)
        {
            std::string res;
            res.reserve(std::strlen(sz) * 2);
            for(; *sz; ++sz)
            {
                res.push_back(std::tolower(*sz));
                res.push_back(std::toupper(*sz));
            }
            return res;
        }
    } // namespace utility

    template<typename List>
    struct alternate
    {
        explicit alternate(List const &list)
          : elems(list)
        {}
        List elems;
    };

    template<typename List>
    struct sequence
    {
        explicit sequence(List const &list)
          : elems(list)
        {}
        List elems;
    };

    struct char_range
      : std::pair<char, char>
    {
        char_range(char from, char to)
          : std::pair<char, char>(from, to)
        {}
    };

    struct ichar
    {
        ichar(char ch)
          : lo_(std::tolower(ch))
          , hi_(std::toupper(ch))
        {}

        char lo_, hi_;
    };

    struct istr
    {
        istr(char const *sz)
          : str_(utility::to_istr(sz))
        {}

        std::string str_;
    };

    struct ichar_range
      : std::pair<char, char>
    {
        ichar_range(char from, char to)
          : std::pair<char, char>(from, to)
        {}
    };

    // The no-case directive
    struct no_case_tag {};

    // remove_case specializations for stripping case-sensitivity from parsers
    template<typename T, typename CaseSensitive>
    struct remove_case;

    template<typename T>
    struct remove_case<T, mpl::false_>
    {
        typedef T type;
    };

    template<>
    struct remove_case<char, mpl::true_>
    {
        typedef ichar type;
    };

    template<>
    struct remove_case<char const *, mpl::true_>
    {
        typedef istr type;
    };

    template<>
    struct remove_case<char_range, mpl::true_>
    {
        typedef ichar_range type;
    };

    ///////////////////////////////////////////////////////////////////////////////
    /// Begin Spirit grammar here
    ///////////////////////////////////////////////////////////////////////////////
    namespace grammar
    {
        using namespace proto;
        using namespace fusion;

        struct SpiritExpr;

        struct AnyChar
          : terminal<char_tag>
        {};

        struct CharLiteral
          : terminal<char>
        {};

        struct NTBSLiteral
          : terminal<char const *>
        {};

        struct CharParser
          : function<AnyChar, CharLiteral>
        {};

        struct CharRangeParser
          : function<AnyChar, CharLiteral, CharLiteral>
        {};

        struct NoCase
          : terminal<no_case_tag>
        {};

        // Extract the arg from terminals
        struct SpiritTerminal
          : or_<
                case_< AnyChar,          _arg >
              , case_< CharLiteral,      remove_case<char, _visitor>(_arg) >
              , case_< CharParser,       remove_case<char, _visitor>(_arg_c<0, _arg1>)> // char_('a')
              , case_< NTBSLiteral,      remove_case<char const *, _visitor>(_arg) >
              , case_< CharRangeParser,  remove_case<char_range, _visitor>(_arg_c<0, _arg1>, _arg_c<0, _arg2>)> // char_('a','z')
            >
        {};

        template<typename Grammar>
        struct FoldToList
          : reverse_fold_tree<Grammar, nil(), cons<SpiritExpr, _state>(SpiritExpr, _state)>
        {};

        struct AltList : FoldToList< bitwise_or<_, _> > {};
        struct SeqList : FoldToList< shift_right<_, _> > {};

        // sequence rule folds all >>'s together into a list
        // and wraps the result in a sequence<> wrapper
        struct SpiritSequence
          : case_< shift_right<SpiritExpr, SpiritExpr>,  sequence<SeqList>(SeqList)  >
        {};

        // alternate rule folds all |'s together into a list
        // and wraps the result in a alternate<> wrapper
        struct SpiritAlternate
          : case_< bitwise_or<SpiritExpr, SpiritExpr>,   alternate<AltList>(AltList) >
        {};

        // Directives such as no_case are handled here
        struct SpiritDirective
          : case_< subscript<NoCase, SpiritExpr>, apply_<SpiritExpr, _right, _state, mpl::true_()> >
        {};

        // A SpiritExpr is an alternate, a sequence, a directive or a terminal
        struct SpiritExpr
          : or_<
                SpiritSequence
              , SpiritAlternate
              , SpiritDirective
              , SpiritTerminal
            >
        {};

    } // namespace grammar

    using grammar::SpiritExpr;
    using grammar::NoCase;

    ///////////////////////////////////////////////////////////////////////////////
    /// End SpiritExpr
    ///////////////////////////////////////////////////////////////////////////////

    // Globals
    NoCase::type const no_case = {{}};

    // Parser
    template<typename Iterator, typename Derived>
    struct with_reset
    {
        with_reset(Iterator begin, Iterator end)
          : first(begin), second(end)
        {}

        template<typename T>
        bool operator()(T const &t) const
        {
            Iterator tmp = this->first;
            if((*static_cast<Derived const *>(this))(t))
                return true;
            this->first = tmp;
            return false;
        }

        bool done() const
        {
            return this->first == this->second;
        }

        mutable Iterator first;
        Iterator second;
    };

    template<typename Iterator>
    struct parser
      : with_reset<Iterator, parser<Iterator> >
    {
        typedef with_reset<Iterator, parser<Iterator> > with_reset;

        parser(Iterator begin, Iterator end)
          : with_reset(begin, end)
        {}

        #if BOOST_VERSION < 103500
        template<typename, typename> // used by fusion::fold
        struct apply
        {
            typedef bool type;
        };
        #else
        typedef bool result_type;   // used by fusion::fold
        #endif

        template<typename T>
        bool operator()(T const &t, bool success) const // used by fusion::fold
        {
            return success && (*this)(t);
        }

        template<typename List>
        bool operator()(alternate<List> const &alternates) const
        {
            return fusion::any(alternates.elems, *static_cast<with_reset const *>(this));
        }

        template<typename List>
        bool operator()(sequence<List> const &sequence) const
        {
            return fusion::fold(sequence.elems, true, *this);
        }

        bool operator()(char_tag ch) const
        {
            if(this->done())
                return false;
            ++this->first;
            return true;
        }

        bool operator()(char ch) const
        {
            if(this->done() || ch != *this->first)
                return false;
            ++this->first;
            return true;
        }

        bool operator()(ichar ich) const
        {
            if(this->done() || !utility::char_icmp(*this->first, ich.lo_, ich.hi_))
                return false;
            ++this->first;
            return true;
        }

        bool operator()(char const *sz) const
        {
            return utility::string_cmp(sz, this->first, this->second);
        }

        bool operator()(istr const &s) const
        {
            return utility::string_icmp(s.str_, this->first, this->second);
        }

        bool operator()(char_range rng) const
        {
            if(this->done() || !utility::in_range(*this->first, rng.first, rng.second))
                return false;
            ++this->first;
            return true;
        }

        bool operator()(ichar_range rng) const
        {
            if(this->done() || !utility::in_irange(*this->first, rng.first, rng.second))
                return false;
            ++this->first;
            return true;
        }
    };

    template<typename Rule, typename Iterator>
    typename enable_if<proto::matches< Rule, SpiritExpr >, bool >::type
    parse_impl(Rule const &rule, Iterator begin, Iterator end)
    {
        mpl::false_ is_case_sensitive;
        parser<Iterator> parse_fun(begin, end);
        return parse_fun(SpiritExpr::call(rule, 0, is_case_sensitive));
    }

    // 2nd overload provides a short error message for invalid rules
    template<typename Rule, typename Iterator>
    typename disable_if<proto::matches< Rule, SpiritExpr >, bool >::type
    parse_impl(Rule const &rule, Iterator begin, Iterator end)
    {
        BOOST_MPL_ASSERT((proto::matches<Rule, SpiritExpr>));
        return false;
    }

    // parse() converts rule literals to proto expressions if necessary
    // and dispatches to parse_impl
    template<typename Rule, typename Iterator>
    bool parse(Rule const &rule, Iterator begin, Iterator end)
    {
        return parse_impl(proto::as_expr(rule), begin, end);
    }

}}

void test_toy_spirit3()
{
    using boost::spirit2::no_case;
    using boost::char_;
    std::string hello("abcd");

    BOOST_CHECK(
        boost::spirit2::parse(
            "abcd"
          , hello.begin()
          , hello.end()
        )
    );

    BOOST_CHECK(
        boost::spirit2::parse(
            char_ >> char_('b') >> 'c' >> char_
          , hello.begin()
          , hello.end()
        )
    );

    BOOST_CHECK(
       !boost::spirit2::parse(
            char_ >> char_('b') >> 'c' >> 'D'
          , hello.begin()
          , hello.end()
        )
    );

    BOOST_CHECK(
        boost::spirit2::parse(
            char_ >> char_('b') >> 'c' >> 'e'
          | char_ >> no_case[char_('B') >> "C" >> char_('D','Z')]
          , hello.begin()
          , hello.end()
        )
    );
}

using namespace boost::unit_test;
///////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("test proto, grammars and tree transforms");

    test->add(BOOST_TEST_CASE(&test_toy_spirit3));

    return test;
}
