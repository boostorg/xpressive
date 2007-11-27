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

    // work around gcc bug.
    template<unsigned int Min, unsigned int Max>
    struct transform_category<xpressive::detail::generic_quant_tag<Min, Max> >
    {
        typedef no_transform type;
    };
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

        // BUGBUG make_expr uses as_expr, not as_arg. Is that right?
        typedef functional::make_expr<tag::shift_right> _make_shift_right;
        typedef functional::make_expr<tag::terminal> _make_terminal;
        typedef functional::make_expr<tag::assign> _make_assign;
        typedef functional::make_expr<tag::logical_not> _make_logical_not;

        template<typename Visitor>
        struct traits_type
        {
            typedef typename Visitor::traits_type type;
        };

        struct greedy : mpl::true_ {};
        struct non_greedy : mpl::false_ {};

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
        // Cases
        template<typename Char, typename Gram>
        struct Cases
        {
            // Here are some transforms ...
            struct as_independent
              : apply_<Gram, _make_shift_right(_, true_matcher()), no_next()>
            {};

            struct as_alternate
              : apply_<Gram, _make_shift_right(_, alternate_end_matcher()), no_next()>
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

            struct MarkedSubExpr
              : assign<terminal<mark_placeholder>, _>
            {};

            struct add_hidden_mark
              : if_<
                    matches<_, MarkedSubExpr>()
                  , _
                  , _make_assign(mark_placeholder(get_hidden_mark(_visitor)), _)
                >
            {};

            template<typename Tag>
            struct as_repeater
              : apply_<
                    _
                  , _make_shift_right(
                        repeat_begin_matcher(mark_number(_arg(_left)))
                      , _make_shift_right(
                            _right
                          , repeat_end_matcher<greedy_t>(
                                mark_number(_arg(_left))
                              // BUGBUG work around gcc bug
                              //, min_type<Tag>()
                              //, max_type<Tag>()
                              , always<min_type<Tag> >
                              , always<max_type<Tag> >
                            )
                        )
                    )
                >
            {};

            template<typename Tag, uint_t, uint_t>
            struct as_default_repeat_impl
              : apply_<as_repeater<Tag>, as_marker(add_hidden_mark(_arg))>
            {};

            template<typename Tag, uint_t Max>
            struct as_default_repeat_impl<Tag, 0, Max>
              : apply_<_, _make_logical_not(as_default_repeat_impl<generic_quant_tag<1, Max>, 1, Max>)>
            {};

            template<typename Tag>
            struct as_default_repeat_impl<Tag, 0, 1>
              : apply_<_, _make_logical_not(_arg)>
            {};

            template<typename Tag>
            struct as_default_repeat
              : as_default_repeat_impl<Tag, min_type<Tag>::value, max_type<Tag>::value>
            {};

            struct as_simple_repeat
              : apply_<
                    _
                  , simple_repeat_matcher<as_independent(_arg), greedy_t>(
                        as_independent(_arg)
                      , min_type<tag_of<_> >()
                      , max_type<tag_of<_> >()
                      , get_width(as_independent(_arg))
                    )
                >
            {};

            struct as_repeat
              : if_<
                    use_simple_repeat<_arg, Char>()
                  , as_simple_repeat
                  , Gram(as_default_repeat<tag_of<_> >(_))
                >
            {};

            // Here are the cases, which use the transforms defined above.
            template<typename Tag, typename Dummy = void>
            struct case_
              : not_<_>
            {};

            template<typename Dummy>
            struct case_<tag::terminal, Dummy>
              : proto::case_< terminal<_>, as_matcher(_arg, _visitor) >
            {};

            template<typename Dummy>
            struct case_<tag::shift_right, Dummy>
              : proto::case_<
                    shift_right<Gram, Gram>
                  , reverse_fold_tree<_, _state, in_sequence(Gram, _state) >
                >
            {};

            struct as_alternates_list
              : reverse_fold_tree<
                    _
                  , fusion::nil()
                  , alternates_list<as_alternate, _state>(as_alternate, _state)
                >
            {};

            template<typename Dummy>
            struct case_<tag::bitwise_or, Dummy>
              : proto::case_<
                    bitwise_or<Gram, Gram>
                  , alternate_matcher<as_alternates_list, traits_type<_visitor> >(as_alternates_list)
                >
            {};

            template<typename Dummy>
            struct case_<tag::dereference, Dummy>
              : proto::case_<dereference<Gram>, as_repeat>
            {};

            template<typename Dummy>
            struct case_<tag::posit, Dummy>
              : proto::case_<posit<Gram>, as_repeat>
            {};

            template<uint_t Min, uint_t Max, typename Dummy>
            struct case_<generic_quant_tag<Min, Max>, Dummy>
              : proto::case_<unary_expr<generic_quant_tag<Min, Max>, Gram>, as_repeat>
            {};

            template<typename Dummy>
            struct case_<tag::logical_not, Dummy>
              : proto::or_<
                    proto::case_<
                        logical_not<assign<terminal<mark_placeholder>, Gram> >
                      , optional_mark_matcher<as_alternate(_arg), greedy_t>(
                            as_alternate(_arg), mark_number(_arg(_left(_arg)))
                        )
                    >
                  , proto::case_<
                        logical_not<Gram>
                      , optional_matcher<as_alternate(_arg), greedy_t>(
                            as_alternate(_arg)
                        )
                    >
                >
            {};

            template<typename Dummy>
            struct case_<tag::assign, Dummy>
              : proto::case_<
                    assign<terminal<mark_placeholder>, Gram>
                  , Gram(as_marker)
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
