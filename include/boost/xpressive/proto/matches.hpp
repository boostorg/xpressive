///////////////////////////////////////////////////////////////////////////////
/// \file matches.hpp
/// Contains definition of the matches\<\> meta-function.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_MATCHES_HPP_EAN_10_28_2007
#define BOOST_PROTO_MATCHES_HPP_EAN_10_28_2007

#include <boost/mpl/if.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/void.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/transform/arg.hpp>

// BUGBUG
#include <boost/mpl/aux_/template_arity.hpp>
#include <boost/mpl/aux_/lambda_arity_param.hpp>

#define UNREF(X)                                                                \
    typename remove_reference<X>::type

#define UNCVREF(X)                                                              \
    typename remove_cv<UNREF(X)>::type

namespace boost { namespace proto
{

    namespace wildns_
    {
        struct _ : _expr
        {
            typedef _ proto_base_expr;
        };
    }

    namespace result_of
    {

        namespace detail
        {
            template<typename Expr, typename Grammar>
            struct matches_;

            template<bool Head, typename... Preds>
            struct and_
              : mpl::bool_<Head>
            {};

            template<typename Head, typename... Preds>
            struct and_<true, Head, Preds...>
              : and_<Head::value, Preds...>
            {};

            template<typename Head>
            struct and_<true, Head>
              : Head
            {};

            template<bool Head, typename... Preds>
            struct or_
              : mpl::bool_<Head>
            {};

            template<typename Head, typename... Preds>
            struct or_<false, Head, Preds...>
              : or_<Head::value, Preds...>
            {};

            template<typename Head>
            struct or_<false, Head>
              : Head
            {};

            template<typename T, typename U>
            struct array_matches
              : mpl::false_
            {};

            template<typename T, std::size_t M>
            struct array_matches<T[M], T *>
              : mpl::true_
            {};

            template<typename T, std::size_t M>
            struct array_matches<T[M], T const *>
              : mpl::true_
            {};

            template<typename T, std::size_t M>
            struct array_matches<T[M], T[proto::N]>
              : mpl::true_
            {};

            template<typename T, typename U
                BOOST_MPL_AUX_LAMBDA_ARITY_PARAM(long Arity = mpl::aux::template_arity<U>::value)
            >
            struct lambda_matches;

            template<typename T, typename U>
            struct lambda_matches_aux_
              : mpl::false_
            {};

            template<
                template<typename, typename...> class T
              , typename E0, typename... ET
              , typename G0, typename... GT
            >
            struct lambda_matches_aux_<T<E0, ET...>, T<G0, GT...> >
              : and_<
                    lambda_matches<E0, G0>::value
                  , lambda_matches<ET, GT>...
                >
            {};

            template<typename T, typename U BOOST_MPL_AUX_LAMBDA_ARITY_PARAM(long Arity)>
            struct lambda_matches
              : lambda_matches_aux_<T, U>
            {};

            template<typename T, typename U>
            struct lambda_matches<T, U BOOST_MPL_AUX_LAMBDA_ARITY_PARAM(-1)>
              : mpl::false_
            {};

            template<typename T>
            struct lambda_matches<T, _ BOOST_MPL_AUX_LAMBDA_ARITY_PARAM(-1)>
              : mpl::true_
            {};

            template<typename T>
            struct lambda_matches<T, T BOOST_MPL_AUX_LAMBDA_ARITY_PARAM(-1)>
              : mpl::true_
            {};

            template<typename T, std::size_t M, typename U>
            struct lambda_matches<T[M], U BOOST_MPL_AUX_LAMBDA_ARITY_PARAM(-1)>
              : array_matches<T[M], U>
            {};

            // How terminal_matches<> handles references and cv-qualifiers.
            // The cv and ref matter *only* if the grammar has a top-level ref.
            //
            // Expr     |   Grammar  |  Match
            // ------------------------------
            // T            T           yes
            // T &          T           yes
            // T const &    T           yes
            // T            T &         no
            // T &          T &         yes
            // T const &    T &         no
            // T            T const &   no
            // T &          T const &   no
            // T const &    T const &   yes

            template<typename T, typename U>
            struct is_cv_ref_compatible
              : mpl::true_
            {};

            template<typename T, typename U>
            struct is_cv_ref_compatible<T, U &>
              : mpl::false_
            {};

            template<typename T, typename U>
            struct is_cv_ref_compatible<T &, U &>
              : mpl::bool_<is_const<T>::value == is_const<U>::value>
            {};

            // terminal_matches
            template<typename T, typename U>
            struct terminal_matches
              : and_<
                    is_cv_ref_compatible<T, U>::value
                  , lambda_matches<UNCVREF(T), UNCVREF(U)>
                >
            {};

            template<typename T>
            struct terminal_matches<T, T>
              : mpl::true_
            {};

            template<typename T>
            struct terminal_matches<T &, T>
              : mpl::true_
            {};

            template<typename T>
            struct terminal_matches<T const &, T>
              : mpl::true_
            {};

            template<typename T>
            struct terminal_matches<T, _>
              : mpl::true_
            {};

            template<typename T>
            struct terminal_matches<T, exact<T> >
              : mpl::true_
            {};

            template<typename T, typename U>
            struct terminal_matches<T, convertible_to<U> >
              : is_convertible<T, U>
            {};

            template<bool True, typename Cons1, typename Cons2>
            struct nonterminal_matches;

            template<typename Head1, typename... Tail1, typename Head2, typename... Tail2>
            struct nonterminal_matches<true, args<Head1, Tail1...>, args<Head2, Tail2...> >
              : nonterminal_matches<
                    matches_<
                        UNREF(Head1)::proto_base_expr
                      , UNREF(Head2)::proto_base_expr
                    >::value
                  , args<Tail1...>
                  , args<Tail2...>
                >
            {};

            template<typename Cons1, typename Cons2>
            struct nonterminal_matches<false, Cons1, Cons2>
              : mpl::false_
            {};

            template<>
            struct nonterminal_matches<true, args<>, args<> >
              : mpl::true_
            {};

            template<
                typename Args1
              , typename Args2
              , bool IsVararg = control::detail::is_vararg<typename back<Args2>::type>::value
            >
            struct vararg_matches
              : mpl::false_
            {};

            template<typename Head1, typename... Tail1, typename Head2, typename... Tail2>
            struct vararg_matches<args<Head1, Tail1...>, args<Head2, Tail2...>, true>
              : and_<
                    matches_<
                        UNREF(Head1)::proto_base_expr
                      , UNREF(Head2)::proto_base_expr
                    >::value
                  , vararg_matches<
                        args<Tail1...>
                      , args<Tail2...>
                      , true
                    >
                >
            {};

            template<typename Head1, typename... Tail1, typename Head2>
            struct vararg_matches<args<Head1, Tail1...>, args<Head2>, true>
              : and_<
                    matches_<
                        UNREF(Head1)::proto_base_expr
                      , UNREF(Head2)::proto_base_expr
                    >::value
                  , vararg_matches<
                        args<Tail1...>
                      , args<Head2>
                      , true
                    >
                >
            {};

            template<typename Head2>
            struct vararg_matches<args<>, args<Head2>, true>
              : mpl::true_
            {};

            template<typename Expr, typename Grammar>
            struct matches_
              : mpl::false_
            {};

            template<typename Expr>
            struct matches_<Expr, _>
              : mpl::true_
            {};

            template<
                typename Tag1
              , typename Head1
              , typename... Tail1
              , long N
              , typename Head2
              , typename... Tail2
            >
            struct matches_<expr<Tag1, args<Head1, Tail1...>, N>, expr<Tag1, args<Head2, Tail2...>, N> >
              : nonterminal_matches<
                    matches_<
                        UNREF(Head1)::proto_base_expr
                      , UNREF(Head2)::proto_base_expr
                    >::value
                  , args<Tail1...>
                  , args<Tail2...>
                >
            {};

            template<
                typename Tag1
              , typename Head1
              , typename... Tail1
              , long N
              , typename Head2
              , typename... Tail2
            >
            struct matches_<expr<Tag1, args<Head1, Tail1...>, N>, expr<_, args<Head2, Tail2...>, N> >
              : nonterminal_matches<
                    matches_<
                        UNREF(Head1)::proto_base_expr
                      , UNREF(Head2)::proto_base_expr
                    >::value
                  , args<Tail1...>
                  , args<Tail2...>
                >
            {};

            template<typename Arg1, typename Arg2>
            struct matches_<expr<tag::terminal, term<Arg1>, 0>, expr<tag::terminal, term<Arg2>, 0> >
              : terminal_matches<Arg1, Arg2>
            {};

            template<typename Tag1, typename Args1, long N1, typename Args2, long N2>
            struct matches_<expr<Tag1, Args1, N1>, expr<Tag1, Args2, N2> >
              : vararg_matches<Args1, Args2>
            {};

            template<typename Tag1, typename Args1, long N1, typename Args2, long N2>
            struct matches_<expr<Tag1, Args1, N1>, expr<_, Args2, N2> >
              : vararg_matches<Args1, Args2>
            {};

            // handle proto::or_
            template<typename Expr, typename Head, typename... Tail>
            struct matches_<Expr, proto::or_<Head, Tail...> >
              : or_<
                    matches_<
                        typename Expr::proto_base_expr
                      , typename Head::proto_base_expr
                    >::value
                  , matches_<
                        typename Expr::proto_base_expr
                      , typename Tail::proto_base_expr
                    >...
                >
            {};

            // handle proto::and_
            template<typename Expr, typename Head, typename... Tail>
            struct matches_<Expr, proto::and_<Head, Tail...> >
              : and_<
                    matches_<
                        typename Expr::proto_base_expr
                      , typename Head::proto_base_expr
                    >::value
                  , matches_<
                        typename Expr::proto_base_expr
                      , typename Tail::proto_base_expr
                    >...
                >
            {};

            // handle proto::if_
            template<typename Expr, typename If, typename Then, typename Else>
            struct matches_<Expr, proto::if_<If, Then, Else> >
              : mpl::eval_if<
                    UNCVREF(typename boost::result_of<otherwise<If>(Expr, mpl::void_, mpl::void_)>::type)
                  , matches_<Expr, typename Then::proto_base_expr>
                  , matches_<Expr, typename Else::proto_base_expr>
                >::type
            {};

            template<typename Expr, typename If>
            struct matches_<Expr, proto::if_<If> >
              : remove_cv<
                    typename remove_reference<
                        typename boost::result_of<when<_, If>(Expr, mpl::void_, mpl::void_)>::type
                    >::type
                >::type
            {};

            // handle proto::not_
            template<typename Expr, typename Grammar>
            struct matches_<Expr, proto::not_<Grammar> >
              : mpl::not_<matches_<Expr, typename Grammar::proto_base_expr> >
            {};

            // handle proto::switch_
            template<typename Expr, typename Cases>
            struct matches_<Expr, switch_<Cases> >
              : matches_<
                    Expr
                  , typename Cases::template case_<typename Expr::proto_tag>::proto_base_expr
                >
            {};

        }

        template<typename Expr, typename Grammar>
        struct matches
          : detail::matches_<
                typename Expr::proto_base_expr
              , typename Grammar::proto_base_expr
            >
        {};

    }

    namespace control
    {
        namespace detail
        {
            template<typename T, typename EnableIf>
            struct is_vararg
              : mpl::false_
            {};

            template<typename T>
            struct is_vararg<T, typename T::proto_is_vararg_>
              : mpl::true_
            {};

            template<typename Expr, typename... Alts>
            struct which;

            template<typename Expr, typename Head, typename... Tail>
            struct which<Expr, Head, Tail...>
              : mpl::if_<
                    matches<Expr, Head>
                  , mpl::identity<Head>
                  , which<Expr, Tail...>
                >::type
            {};

        } // namespace detail

        template<typename... Alts>
        struct or_ : callable
        {
            typedef or_ proto_base_expr;

            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
            {
                typedef typename boost::result_of<
                    typename detail::which<UNCVREF(Expr), Alts...>::type(Expr, State, Visitor)
                >::type type;
            };

            template<typename Expr, typename State, typename Visitor>
            typename result<or_(Expr const &, State const &, Visitor &)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                typedef typename detail::which<Expr, Alts...>::type which;
                return which()(expr, state, visitor);
            }
        };

        template<typename... Alts>
        struct and_ : back<args<Alts...> >::type
        {
            typedef and_ proto_base_expr;
        };

        // if_
        template<typename If, typename Then, typename Else>
        struct if_ : callable
        {
            typedef if_ proto_base_expr;

            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
              : mpl::eval_if<
                    UNCVREF(typename boost::result_of<otherwise<If>(Expr, State, Visitor)>::type)
                  , boost::result_of<when<_, Then>(Expr, State, Visitor)>
                  , boost::result_of<when<_, Else>(Expr, State, Visitor)>
                >
            {};

            template<typename Expr, typename State, typename Visitor>
            typename result<if_(Expr const &, State const &, Visitor &)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                typedef
                    typename mpl::if_<
                        UNCVREF(typename boost::result_of<otherwise<If>(Expr const &, State const &, Visitor &)>::type)
                      , when<_, Then>
                      , when<_, Else>
                    >::type
                branch;

                return branch()(expr, state, visitor);
            }
        };

        template<typename Grammar>
        struct not_ : _expr
        {
            typedef not_ proto_base_expr;
        };

        template<typename Cases>
        struct switch_ : callable
        {
            typedef switch_ proto_base_expr;

            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
              : boost::result_of<typename Cases::template case_<UNREF(Expr)::proto_tag>(Expr, State, Visitor)>
            {};

            template<typename Expr, typename State, typename Visitor>
            typename result<switch_(Expr const &, State const &, Visitor &)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                typedef typename Cases::template case_<typename Expr::proto_tag> case_;
                return case_()(expr, state, visitor);
            }
        };

        template<typename T>
        struct exact
        {};

        template<typename T>
        struct convertible_to
        {};

        template<typename Grammar>
        struct vararg
          : Grammar
        {
            typedef void proto_is_vararg_;
        };

    }

    template<typename... Args>
    struct is_callable<or_<Args...> >
      : mpl::true_
    {};

    template<typename... Args>
    struct is_callable<and_<Args...> >
      : mpl::true_
    {};

    template<typename Grammar>
    struct is_callable<not_<Grammar> >
      : mpl::true_
    {};

    template<typename If, typename Then, typename Else>
    struct is_callable<if_<If, Then, Else> >
      : mpl::true_
    {};

    template<typename Grammar>
    struct is_callable<vararg<Grammar> >
      : mpl::true_
    {};

    template<>
    struct is_callable<_>
      : mpl::true_
    {};

}}

#undef UNREF
#undef UNCVREF

#endif
