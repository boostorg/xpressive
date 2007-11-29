///////////////////////////////////////////////////////////////////////////////
// grammar.hpp
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_XPRESSIVE_DETAIL_STATIC_GRAMMAR2_HPP_EAN_11_12_2006
#define BOOST_XPRESSIVE_DETAIL_STATIC_GRAMMAR2_HPP_EAN_11_12_2006

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/mpl/if.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/apply_wrap.hpp>
#include <boost/mpl/next_prior.hpp>
#include <boost/fusion/include/cons.hpp>
#include <boost/xpressive/proto3/proto.hpp>
#include <boost/xpressive/proto3/transform.hpp>
#include <boost/xpressive/detail/detail_fwd.hpp>
#include <boost/xpressive/detail/static/static.hpp>
#include <boost/xpressive/detail/static/is_pure.hpp>

#define BOOST_XPRESSIVE_CHECK_REGEX(Expr, Char)\
    BOOST_MPL_ASSERT\
    ((\
        typename boost::mpl::if_<\
            boost::xpressive::is_valid_regex<Expr, Char>\
          , boost::mpl::true_\
          , boost::xpressive::INVALID_REGULAR_EXPRESSION\
        >::type\
    ));

//////////////////////////////////////////////////////////////////////////
//**********************************************************************//
//*                            << NOTE! >>                             *//
//*                                                                    *//
//* Whenever you change this grammar, you MUST also make corresponding *//
//* changes to width_of.hpp and is_pure.hpp.                           *//
//*                                                                    *//
//**********************************************************************//
//////////////////////////////////////////////////////////////////////////

namespace boost { namespace proto
{
    template<>
    struct is_aggregate<xpressive::detail::mark_placeholder>
      : mpl::true_
    {};
}}

namespace boost { namespace xpressive
{
    template<typename Char>
    struct Grammar;

    namespace detail
    {
        ///////////////////////////////////////////////////////////////////////////////
        // generic_quant_tag
        template<uint_t Min, uint_t Max>
        struct generic_quant_tag
        {
            typedef mpl::integral_c<uint_t, Min> min_type;
            typedef mpl::integral_c<uint_t, Max> max_type;
        };

        ///////////////////////////////////////////////////////////////////////////////
        // min_type / max_type
        template<typename Tag>
        struct min_type : Tag::min_type {};

        template<>
        struct min_type<proto::tag::posit> : mpl::integral_c<uint_t, 1> {};

        template<>
        struct min_type<proto::tag::dereference> : mpl::integral_c<uint_t, 0> {};

        template<>
        struct min_type<proto::tag::logical_not> : mpl::integral_c<uint_t, 0> {};

        template<typename Tag>
        struct max_type : Tag::max_type {};

        template<>
        struct max_type<proto::tag::posit> : mpl::integral_c<uint_t, UINT_MAX-1> {};

        template<>
        struct max_type<proto::tag::dereference> : mpl::integral_c<uint_t, UINT_MAX-1> {};

        template<>
        struct max_type<proto::tag::logical_not> : mpl::integral_c<uint_t, 1> {};

        ///////////////////////////////////////////////////////////////////////////////
        // alternates_list
        //   a fusion-compatible sequence of alternate expressions, that also keeps
        //   track of the list's width and purity.
        template<typename Head, typename Tail>
        struct alternates_list
          : fusion::cons<Head, Tail>
        {
            BOOST_STATIC_CONSTANT(std::size_t, width = Head::width == Tail::width ? Head::width : unknown_width::value);
            BOOST_STATIC_CONSTANT(bool, pure = Head::pure && Tail::pure);

            alternates_list(Head const &head, Tail const &tail)
              : fusion::cons<Head, Tail>(head, tail)
            {
            }
        };

        template<typename Head>
        struct alternates_list<Head, fusion::nil>
          : fusion::cons<Head, fusion::nil>
        {
            BOOST_STATIC_CONSTANT(std::size_t, width = Head::width);
            BOOST_STATIC_CONSTANT(bool, pure = Head::pure);

            alternates_list(Head const &head, fusion::nil const &tail)
              : fusion::cons<Head, fusion::nil>(head, tail)
            {
            }
        };
    }

    namespace grammar_detail
    {
        using namespace proto;
        using namespace transform;
        using namespace xpressive::detail;

        template<typename Tag>
        struct is_generic_quant_tag
          : mpl::false_
        {};

        template<uint_t Min, uint_t Max>
        struct is_generic_quant_tag<generic_quant_tag<Min,Max> >
          : mpl::true_
        {};

        struct as_matcher : function_transform
        {
            template<typename Sig>
            struct result;

            template<typename This, typename T, typename Visitor>
            struct result<This(T, Visitor)>
              : Visitor::template apply<T>
            {};

            template<typename T, typename Visitor>
            typename Visitor::template apply<T>::type
            operator()(T const &t, Visitor &visitor) const
            {
                return visitor.call(t);
            }
        };

        struct get_width : function_transform
        {
            typedef std::size_t result_type;

            template<typename Xpr>
            std::size_t operator()(Xpr const &xpr) const
            {
                return xpr.get_width().value();
            }
        };

        struct mark_number : function_transform
        {
            typedef int result_type;

            template<typename Expr>
            int operator()(Expr const &expr) const
            {
                return expr.mark_number_;
            }
        };

        struct get_hidden_mark : function_transform
        {
            typedef int result_type;

            template<typename Visitor>
            int operator()(Visitor &visitor) const
            {
                return visitor.get_hidden_mark();
            }
        };

        struct traits : function_transform
        {
            template<typename Sig>
            struct result;
            
            template<typename This, typename Visitor>
            struct result<This(Visitor)>
            {
                typedef typename Visitor::traits_type type;
            };
            
            template<typename Visitor>
            typename Visitor::traits_type const &
            operator()(Visitor &visitor) const
            {
                return visitor.traits();
            }
        };

        struct newline : function_transform
        {
            template<typename Sig>
            struct result;
            
            template<typename This, typename Traits>
            struct result<This(Traits)>
            {
                typedef typename Traits::char_class_type type;
            };
            
            template<typename Traits>
            typename Traits::char_class_type
            operator()(Traits const &traits) const
            {
                return lookup_classname(traits, "newline");
            }
        };

        struct as_posix_charset : function_transform
        {
            template<typename Sig>
            struct result;
            
            template<typename This, typename Posix, typename Visitor, typename YesNo>
            struct result<This(Posix, Visitor, YesNo)>
            {
                typedef posix_charset_matcher<typename Visitor::traits_type> type;
            };

            template<typename Posix, typename Visitor>
            posix_charset_matcher<typename Visitor::traits_type>
            operator()(Posix const &posix, Visitor const &visitor, bool yes_no) const
            {
                char const *name_end = posix.name_ + std::strlen(posix.name_);
                return posix_charset_matcher<typename Visitor::traits_type>(
                    visitor.traits().lookup_classname(posix.name_, name_end, Visitor::icase_type::value)
                  , yes_no == posix.not_
                );
            }
        };

        template<typename Visitor>
        struct icase
        {
            typedef typename Visitor::icase_type type;
        };

        // BUGBUG make_expr uses as_expr, not as_arg. Is that right?
        typedef functional::make_expr<tag::shift_right> _make_shift_right;
        typedef functional::make_expr<tag::terminal> _make_terminal;
        typedef functional::make_expr<tag::assign> _make_assign;
        typedef functional::make_expr<tag::logical_not> _make_logical_not;
        typedef functional::make_expr<tag::negate> _make_negate;

        // Place a head and a tail in sequence, if it's not
        // already in sequence.
        struct in_sequence : function_transform
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Matcher, typename Next>
            struct result<This(Matcher, Next)>
            {
                typedef static_xpression<Matcher, Next> type;
            };

            template<typename This, typename Matcher, typename Next, typename Next2>
            struct result<This(static_xpression<Matcher, Next>, Next2)>
            {
                typedef static_xpression<Matcher, Next> type;
            };

            template<typename Matcher, typename Next>
            static_xpression<Matcher, Next>
            operator()(Matcher const &matcher, Next const &next) const
            {
                return static_xpression<Matcher, Next>(matcher, next);
            }

            template<typename Matcher, typename Next, typename Next2>
            static_xpression<Matcher, Next> const &
            operator()(static_xpression<Matcher, Next> const &xpr, Next2 const &) const
            {
                return xpr;
            }
        };

        ///////////////////////////////////////////////////////////////////////////
        // CharLiteral
        template<typename Char>
        struct CharLiteral
          : proto::or_<
                proto::terminal<char>
              , proto::terminal<Char>
            >
        {};

        template<>
        struct CharLiteral<char>
          : proto::terminal<char>
        {};
        
        struct _one : mpl::int_<1> {};

        ///////////////////////////////////////////////////////////////////////////
        // ListSet
        //  matches expressions like (set= 'a','b','c')
        //  calculates the size of the set
        //  populates an array of characters
        template<typename Char>
        struct ListSet
          : or_<
                when<
                    comma<ListSet<Char>, CharLiteral<Char> >
                  , mpl::next<ListSet<Char>(_left)>()
                >
              , when<
                    assign<terminal<set_initializer>, CharLiteral<Char> >
                  , _one()
                >
            >
        {};
        
        struct fill_list_set : function_transform
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Set, typename Expr, typename Visitor>
            struct result<This(Set, Expr, Visitor)>
            {
                typedef Set type;
            };

            template<typename Set, typename Expr, typename Visitor>
            Set operator()(Set set, Expr const &expr, Visitor &visitor) const
            {
                typename Set::char_type *buffer = set.set_;
                this->fill(buffer, expr, visitor.traits());
                return set;
            }

        private:
            template<typename Char, typename Expr, typename Traits>
            void fill(Char *&buffer, Expr const &expr, Traits const &traits) const
            {
                this->fill(buffer, proto::left(expr), traits);
                *buffer++ = traits.translate(
                    char_cast<Char>(proto::arg(proto::right(expr)), traits)
                );
            }

            template<typename Char, typename Traits>
            void fill(Char *&, set_initializer_type, Traits const &) const
            {}
        };
        
        ///////////////////////////////////////////////////////////////////////////
        // Cases
        template<typename Char, typename Gram>
        struct Cases
        {
            // Some simple grammars...
            struct MarkedSubExpr
              : assign<terminal<mark_placeholder>, _>
            {};

            struct GenericQuant
              : and_<
                    if_<is_generic_quant_tag<tag_of<_> >()>
                  , unary_expr<_, Gram>
                >
            {};

            // Here are some transforms ...
            struct as_independent
              : apply_<Gram, _make_shift_right(_, true_matcher()), no_next()>
            {};

            struct as_alternate
              : apply_<Gram, _make_shift_right(_, alternate_end_matcher()), no_next()>
            {};

            struct as_alternates_list
              : reverse_fold_tree<
                    _
                  , fusion::nil()
                  , alternates_list<as_alternate, _state>(as_alternate, _state)
                >
            {};

            struct as_marker
              : apply_<
                    _
                  , _make_shift_right(
                        mark_begin_matcher(mark_number(_arg(_left)))
                      , _make_shift_right(
                            _right
                          , mark_end_matcher(mark_number(_arg(_left)))
                        )
                    )
                >
            {};

            struct add_hidden_mark
              : if_<
                    matches<_, MarkedSubExpr>()
                  , _
                  , _make_assign(mark_placeholder(get_hidden_mark(_visitor)), _)
                >
            {};

            template<typename Greedy, typename Tag>
            struct as_repeater
              : apply_<
                    _
                  , _make_shift_right(
                        repeat_begin_matcher(mark_number(_arg(_left)))
                      , _make_shift_right(
                            _right
                          , repeat_end_matcher<Greedy>(
                                mark_number(_arg(_left))
                              // BUGBUG work around gcc bug
                              , always<min_type<Tag> > //, min_type<Tag>()
                              , always<max_type<Tag> > //, max_type<Tag>()
                            )
                        )
                    )
                >
            {};

            template<typename Greedy, typename Tag, uint_t = min_type<Tag>::value, uint_t = max_type<Tag>::value>
            struct as_default_repeat_impl
              : apply_<as_repeater<Greedy, Tag>, as_marker(add_hidden_mark(_arg))>
            {};

            template<typename Greedy, typename Tag, uint_t Max>
            struct as_default_repeat_impl<Greedy, Tag, 0, Max>
              : if_<
                    Greedy()
                  , _make_negate(_make_logical_not(as_default_repeat_impl<Greedy, generic_quant_tag<1, Max> >))
                  , _make_logical_not(as_default_repeat_impl<Greedy, generic_quant_tag<1, Max> >)
                >
            {};

            template<typename Greedy, typename Tag>
            struct as_default_repeat_impl<Greedy, Tag, 0, 1>
              : if_<
                    Greedy()
                  , _make_negate(_make_logical_not(_arg))
                  , _make_logical_not(_arg)
                >
            {};

            template<typename Greedy, typename Tag>
            struct as_default_repeat
              : as_default_repeat_impl<Greedy, Tag>
            {};

            template<typename Greedy>
            struct as_simple_repeat
              : apply_<
                    _
                  , simple_repeat_matcher<as_independent(_arg), Greedy>(
                        as_independent(_arg)
                      , min_type<tag_of<_> >()
                      , max_type<tag_of<_> >()
                      , get_width(as_independent(_arg))
                    )
                >
            {};

            template<typename Greedy>
            struct as_repeat
              : if_<
                    use_simple_repeat<_arg, Char>()
                  , as_simple_repeat<Greedy>
                  , Gram(as_default_repeat<Greedy, tag_of<_> >(_))
                >
            {};

            template<typename Greedy>
            struct as_optional
              : if_<
                    matches<_, MarkedSubExpr>()
                  , optional_mark_matcher<as_alternate, Greedy>(as_alternate, mark_number(_arg(_left)))
                  , optional_matcher<as_alternate, Greedy>(as_alternate)
                >
            {};
            
            struct as_list_set
              : apply_<
                    _
                  , fill_list_set(
                        set_matcher<traits(_visitor), ListSet<Char>(_) >()
                      , _
                      , _visitor
                    )
                >
            {};

            // Here are the cases, which use the transforms defined above.
            template<typename Tag, typename Dummy = void>
            struct case_
              : not_<_>
            {};

            template<typename Dummy>
            struct case_<tag::terminal, Dummy>
              : when<_, as_matcher(_arg, _visitor)>
            {};

            template<typename Dummy>
            struct case_<tag::shift_right, Dummy>
              : when<
                    shift_right<Gram, Gram>
                  , reverse_fold_tree<_, _state, in_sequence(Gram, _state)>
                >
            {};

            template<typename Dummy>
            struct case_<tag::bitwise_or, Dummy>
              : when<
                    bitwise_or<Gram, Gram>
                  , alternate_matcher<as_alternates_list, traits(_visitor)>(as_alternates_list)
                >
            {};

            template<typename Dummy>
            struct case_<tag::dereference, Dummy>
              : when<dereference<Gram>, as_repeat<greedy_t> >
            {};

            template<typename Dummy>
            struct case_<tag::posit, Dummy>
              : when<posit<Gram>, as_repeat<greedy_t> >
            {};

            template<uint_t Min, uint_t Max, typename Dummy>
            struct case_<generic_quant_tag<Min, Max>, Dummy>
              : when<unary_expr<generic_quant_tag<Min, Max>, Gram>, as_repeat<greedy_t> >
            {};

            template<typename Dummy>
            struct case_<tag::logical_not, Dummy>
              : when<logical_not<Gram>, as_optional<greedy_t>(_arg)>
            {};

            template<typename Dummy>
            struct case_<tag::negate, Dummy>
              : or_<
                    when<negate<dereference<Gram> > , as_repeat<non_greedy_t>(_arg)>
                  , when<negate<posit<Gram> >       , as_repeat<non_greedy_t>(_arg)>
                  , when<negate<GenericQuant>       , as_repeat<non_greedy_t>(_arg)>
                  , when<negate<logical_not<Gram> > , as_optional<non_greedy_t>(_arg(_arg))>
                >
            {};

            template<typename Dummy>
            struct case_<tag::assign, Dummy>
              : or_<
                    when<assign<terminal<mark_placeholder>, Gram>, Gram(as_marker)>
                  , when<ListSet<Char>, as_list_set>
                >
            {};

            template<typename Dummy>
            struct case_<tag::comma, Dummy>
              : when<ListSet<Char>, as_list_set>
            {};

            template<typename Dummy>
            struct case_<keeper_tag, Dummy>
              : when<
                    unary_expr<keeper_tag, Gram>
                  , keeper_matcher<as_independent(_arg)>(as_independent(_arg))
                >
            {};

            template<typename Dummy>
            struct case_<lookahead_tag, Dummy>
              : when<
                    unary_expr<lookahead_tag, Gram>
                  , lookahead_matcher<as_independent(_arg)>(as_independent(_arg), false_())
                >
            {};

            template<typename Dummy>
            struct case_<lookbehind_tag, Dummy>
              : when<
                    unary_expr<lookbehind_tag, Gram>
                  , lookbehind_matcher<as_independent(_arg)>(
                        as_independent(_arg)
                      , get_width(as_independent(_arg))
                      , false_()
                    )
                >
            {};

            template<typename Dummy>
            struct case_<tag::complement, Dummy>
              : or_<
                    // ~_b
                    when<
                        complement<terminal<assert_word_placeholder<word_boundary<true_> > > >
                      , assert_word_matcher<word_boundary<false_>, traits(_visitor)>(traits(_visitor))
                    >
                    // ~_n
                  , when<
                        complement<CharLiteral<Char> >
                      , literal_matcher<traits(_visitor), icase<_visitor>, true_>(_arg(_arg), traits(_visitor))
                    >
                    // ~_ln
                  , when<
                        complement<terminal<logical_newline_placeholder> >
                      , posix_charset_matcher<traits(_visitor)>(newline(traits(_visitor)), true_())
                    >
                    // ~alpha
                  , when<
                        complement<terminal<posix_charset_placeholder> >
                      , as_posix_charset(_arg(_arg), _visitor, false_())
                    >
                    // ~range('a','z')
                  , when<
                        complement<terminal<range_placeholder<_> > >
                      , range_matcher<traits(_visitor), icase<_visitor> >(_arg(_arg), true_())
                    >
                    // ~before(...)
                  , when<
                        complement<unary_expr<lookahead_tag, Gram> >
                      , lookahead_matcher<as_independent(_arg(_arg))>(
                            as_independent(_arg(_arg))
                          , true_()
                        )
                    >
                    // ~after(...)
                  , when<
                        complement<unary_expr<lookbehind_tag, Gram> >
                      , lookbehind_matcher<as_independent(_arg(_arg))>(
                            as_independent(_arg(_arg))
                          , get_width(as_independent(_arg(_arg)))
                          , true_()
                        )
                    >
                >
            {};
        };

    } // namespace detail

    ///////////////////////////////////////////////////////////////////////////
    // Grammar
    template<typename Char>
    struct Grammar
      : proto::switch_<grammar_detail::Cases<Char, Grammar<Char> > >
    {};

    ///////////////////////////////////////////////////////////////////////////
    // INVALID_REGULAR_EXPRESSION
    struct INVALID_REGULAR_EXPRESSION
      : mpl::false_
    {};

    ///////////////////////////////////////////////////////////////////////////
    // is_valid_regex
    template<typename Expr, typename Char>
    struct is_valid_regex
      : proto::matches<Expr, Grammar<Char> >
    {};

}} // namespace boost::xpressive

#endif
