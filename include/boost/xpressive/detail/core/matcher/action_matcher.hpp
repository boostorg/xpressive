///////////////////////////////////////////////////////////////////////////////
// action_matcher.hpp
//
//  Copyright 2007 Eric Niebler.
//  Copyright 2007 David Jenkins.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_XPRESSIVE_DETAIL_CORE_MATCHER_ACTION_MATCHER_HPP_EAN_10_04_2005
#define BOOST_XPRESSIVE_DETAIL_CORE_MATCHER_ACTION_MATCHER_HPP_EAN_10_04_2005

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/version.hpp>
#include <boost/ref.hpp>
#include <boost/assert.hpp>
#include <boost/mpl/if.hpp>
#include <boost/throw_exception.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/xpressive/detail/detail_fwd.hpp>
#include <boost/xpressive/detail/core/quant_style.hpp>
#include <boost/xpressive/detail/core/action.hpp>
#include <boost/xpressive/detail/core/state.hpp>
#include <boost/xpressive/detail/static/grammar.hpp>
#include <boost/xpressive/proto/proto.hpp>
#include <boost/xpressive/proto/context.hpp>
#include <boost/xpressive/match_results.hpp> // for type_info_less
#if BOOST_VERSION >= 103500
# include <boost/xpressive/proto/fusion.hpp>
# include <boost/fusion/include/transform_view.hpp>
# include <boost/fusion/include/invoke.hpp>
# include <boost/fusion/include/push_front.hpp>
# include <boost/fusion/include/pop_front.hpp>
#endif

namespace boost { namespace xpressive { namespace detail
{

    #if BOOST_VERSION >= 103500
    struct DataMember
      : proto::mem_ptr<proto::_, proto::terminal<proto::_> >
    {};

    template<typename Expr, long N>
    struct child_
      : remove_reference<
            typename proto::result_of::arg_c<Expr, N>::type
        >
    {};

    ///////////////////////////////////////////////////////////////////////////////
    // mem_ptr_eval
    //  Rewrites expressions of the form x->*foo(a) into foo(x, a) and then
    //  evaluates them.
    template<typename Expr, typename Context, bool IsDataMember = proto::matches<Expr, DataMember>::value>
    struct mem_ptr_eval
    {
        typedef typename child_<Expr, 0>::type left_type;
        typedef typename child_<Expr, 1>::type right_type;

        typedef
            typename proto::result_of::value_at_c<
                typename proto::result_of::arg_c<right_type, 0>::type
              , 0
            >::type
        function_type;

        typedef
            fusion::transform_view<
                typename fusion::result_of::push_front<
                    typename fusion::result_of::pop_front<right_type>::type const
                  , reference_wrapper<left_type>
                >::type const
              , proto::eval_fun<Context>
            >
        evaluated_args;

        typedef
            typename fusion::result_of::invoke<function_type, evaluated_args>::type
        result_type;

        result_type operator()(Expr &expr, Context &ctx) const
        {
            return fusion::invoke<function_type>(
                proto::arg(proto::arg_c<0>(proto::right(expr)))
              , evaluated_args(
                    fusion::push_front(
                        fusion::pop_front(proto::right(expr))
                      , boost::ref(proto::left(expr))
                    )
                  , proto::eval_fun<Context>(ctx)
                )
            );
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    // mem_ptr_eval
    //  Rewrites expressions of the form x->*foo into foo(x) and then
    //  evaluates them.
    template<typename Expr, typename Context>
    struct mem_ptr_eval<Expr, Context, true>
    {
        typedef typename child_<Expr, 0>::type left_type;
        typedef typename child_<Expr, 1>::type right_type;

        typedef
            typename proto::result_of::value_at_c<right_type, 0>::type
        function_type;

        typedef typename boost::result_of<
            function_type(typename proto::result_of::eval<left_type, Context>::type)
        >::type result_type;

        result_type operator()(Expr &expr, Context &ctx) const
        {
            return proto::arg(proto::right(expr))(
                proto::eval(proto::left(expr), ctx)
            );
        }
    };
    #endif

    struct attr_with_default_tag
    {};

    template<typename T>
    struct opt;

    ///////////////////////////////////////////////////////////////////////////////
    // action_context
    //
    struct action_context
    {
        explicit action_context(action_args_type *action_args)
          : action_args_(action_args)
        {}

        action_args_type const &args() const
        {
            return *this->action_args_;
        }

        // eval_terminal
        template<typename Expr, typename Arg>
        struct eval_terminal
          : proto::default_eval<Expr, action_context const>
        {};

        template<typename Expr, typename Arg>
        struct eval_terminal<Expr, reference_wrapper<Arg> >
        {
            typedef Arg &result_type;
            result_type operator()(Expr &expr, action_context const &) const
            {
                return proto::arg(expr).get();
            }
        };

        template<typename Expr, typename Arg>
        struct eval_terminal<Expr, opt<Arg> >
        {
            typedef Arg const &result_type;
            result_type operator()(Expr &expr, action_context const &) const
            {
                return proto::arg(expr);
            }
        };

        template<typename Expr, typename Type, typename Int>
        struct eval_terminal<Expr, action_arg<Type, Int> >
        {
            typedef typename action_arg<Type, Int>::reference result_type;
            result_type operator()(Expr &expr, action_context const &ctx) const
            {
                action_args_type::const_iterator where_ = ctx.args().find(&typeid(proto::arg(expr)));
                if(where_ == ctx.args().end())
                {
                    boost::throw_exception(
                        regex_error(
                            regex_constants::error_badarg
                          , "An argument to an action was unspecified"
                        )
                    );
                }
                return proto::arg(expr).cast(where_->second);
            }
        };

        // eval
        template<typename Expr, typename Tag = typename Expr::proto_tag>
        struct eval
          : proto::default_eval<Expr, action_context const>
        {};

        template<typename Expr>
        struct eval<Expr, proto::tag::terminal>
          : eval_terminal<Expr, typename unref_arg_c<Expr, 0>::type>
        {};

        // Evaluate attributes like a1|42
        template<typename Expr>
        struct eval<Expr, attr_with_default_tag>
        {
            typedef
                typename proto::result_of::value_at_c<
                    typename proto::result_of::left<
                        typename proto::result_of::arg<
                            Expr
                        >::type
                    >::type
                  , 0
                >::type
            temp_type;

            typedef typename temp_type::value_type result_type;

            result_type operator ()(Expr const &expr, action_context const &ctx) const
            {
                return proto::arg(proto::left(proto::arg(expr))).t_
                    ? *proto::arg(proto::left(proto::arg(expr))).t_
                    :  proto::eval(proto::right(proto::arg(expr)), ctx);
            }
        };

        #if BOOST_VERSION >= 103500
        template<typename Expr>
        struct eval<Expr, proto::tag::mem_ptr>
          : mem_ptr_eval<Expr, action_context const>
        {};
        #endif

    private:
        action_args_type *action_args_;
    };

    ///////////////////////////////////////////////////////////////////////////////
    // action
    //
    template<typename Actor>
    struct action
      : actionable
    {
        action(Actor const &actor)
          : actionable()
          , actor_(actor)
        {
        }

        virtual void execute(action_args_type *action_args) const
        {
            action_context const ctx(action_args);
            proto::eval(this->actor_, ctx);
        }

    private:
        Actor actor_;
    };

    struct get_sub_match : proto::callable
    {
        template<typename Sig>
        struct result;

        template<typename This, typename State, typename Index>
        struct result<This(State, Index)>
        {
            typedef sub_match<typename remove_reference<State>::type::iterator> type;
        };

        template<typename State, typename Index>
        sub_match<typename State::iterator> operator()(State const &state, Index i)
        {
            return state.sub_matches_[i];
        }
    };

    struct get_attr_slot : proto::callable
    {
        template<typename Sig>
        struct result;

        template<typename This, typename State, typename Attr>
        struct result<This(State, Attr)>
        {
            typedef typename remove_reference<Attr>::type::matcher_type::value_type::second_type const *type;
        };

        template<typename State, typename Attr>
        typename Attr::matcher_type::value_type::second_type const *
        operator()(State const &state, Attr const &)
        {
            typedef typename Attr::matcher_type::value_type::second_type attr_type;
            int slot = typename Attr::nbr_type();
            return static_cast<attr_type const *>(state.attr_context_.attr_slots_[slot-1]);
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    // opt
    //
    template<typename T>
    struct opt
    {
        typedef T value_type;
        typedef T const &reference;

        explicit opt(T const *t)
          : t_(t)
        {}

        operator reference() const
        {
            detail::ensure(0 != this->t_, regex_constants::error_badattr, "Use of uninitialized regex attribute");
            return *this->t_;
        }

        T const *t_;
    };

    template<typename Attr>
    struct attr_value_type
    {
        typedef typename Attr::matcher_type::value_type::second_type type;
    };

    template<typename Expr>
    struct const_reference
    {
        typedef typename proto::result_of::arg<Expr const>::type type;
    };

    using grammar_detail::mark_number;
    using grammar_detail::read_attr;

    ///////////////////////////////////////////////////////////////////////////////
    // BindActionArgs
    //
    struct BindActionArgs
      : proto::or_<
            proto::when<
                proto::terminal<any_matcher>
              , proto::_make_terminal(get_sub_match(proto::_state, proto::_visitor))
            >
          , proto::when<
                proto::terminal<mark_placeholder>
              , proto::_make_terminal(get_sub_match(proto::_state, mark_number(proto::_arg)))
            >
          , proto::when<
                proto::terminal<read_attr<proto::_, proto::_> >
              , proto::_make_terminal(opt<attr_value_type<proto::_arg> >(get_attr_slot(proto::_state, proto::_arg)))
            >
          , proto::when<
                proto::terminal<proto::_>
              , proto::terminal<const_reference<proto::_> >(proto::_arg)
            >
          , proto::when<
                proto::bitwise_or<
                    proto::terminal<read_attr<proto::_, proto::_> >
                  , BindActionArgs
                >
              , proto::functional::make_expr<attr_with_default_tag>(
                    proto::bitwise_or<BindActionArgs, BindActionArgs>
                )
            >
          , proto::otherwise<
                proto::nary_expr<proto::_, proto::vararg<BindActionArgs> >
            >
        >
    {};

    ///////////////////////////////////////////////////////////////////////////////
    // action_matcher
    //
    template<typename Actor>
    struct action_matcher
      : quant_style<quant_none, 0, false>
    {
        int sub_;
        Actor actor_;

        action_matcher(Actor const &actor, int sub)
          : sub_(sub)
          , actor_(actor)
        {
        }

        template<typename BidiIter, typename Next>
        bool match(match_state<BidiIter> &state, Next const &next) const
        {
            // Bind the arguments
            typedef typename boost::result_of<BindActionArgs(Actor, match_state<BidiIter>, int)>::type action_type;
            action<action_type> actor(BindActionArgs()(this->actor_, state, this->sub_));

            // Put the action in the action list
            actionable const **action_list_tail = state.action_list_tail_;
            *state.action_list_tail_ = &actor;
            state.action_list_tail_ = &actor.next;

            // Match the rest of the pattern
            if(next.match(state))
            {
                return true;
            }

            BOOST_ASSERT(0 == actor.next);
            // remove action from list
            *action_list_tail = 0;
            state.action_list_tail_ = action_list_tail;
            return false;
        }
    };

}}}

#endif
