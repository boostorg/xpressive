#ifndef BOOST_PP_IS_ITERATING
    ///////////////////////////////////////////////////////////////////////////////
    /// \file nary_expr.hpp
    /// Definintion of nary_expr<> and function<>
    //
    //  Copyright 2007 Eric Niebler. Distributed under the Boost
    //  Software License, Version 1.0. (See accompanying file
    //  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    #ifndef BOOST_PROTO_DETAIL_NARY_EXPR_HPP_EAN_11_07_2007
    #define BOOST_PROTO_DETAIL_NARY_EXPR_HPP_EAN_11_07_2007

    #include <boost/mpl/bool.hpp>
    #include <boost/type_traits.hpp>
    #include <boost/xpressive/proto/proto_fwd.hpp>
    #include <boost/xpressive/proto/detail/apply_args.hpp>
    #include <boost/xpressive/proto/detail/define.hpp>

    namespace boost { namespace proto
    {
        namespace op
        {

        #if defined(BOOST_HAS_VARIADIC_TMPL) && defined(BOOST_HAS_RVALUE_REFS)
            template<typename Tag BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_PROTO_MAX_ARITY, typename A), typename... Rest>
            struct nary_expr<Tag BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_PROTO_MAX_ARITY, A), Rest...>
            {
                typedef expr<Tag, args<BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, A), Rest...> > type;
                typedef type proto_base_expr;

                template<typename Sig>
                struct result;

                template<typename This, typename Expr, typename State, typename Visitor>
                struct result<This(Expr, State, Visitor)>
                {
                    typedef detail::apply_args<
                        UNREF(Expr)::proto_args
                      , State
                      , Visitor
                      , args<BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, A), Rest...>
                    > apply_;
                    typedef expr<UNREF(Expr)::proto_tag, typename apply_::type> type;
                };

                template<typename Expr, typename State, typename Visitor>
                typename result<nary_expr(Expr const &, State const &, Visitor &)>::type
                operator()(Expr const &expr, State const &state, Visitor &visitor) const
                {
                    typedef result<nary_expr(Expr const &, State const &, Visitor &)> result_;
                    typename result_::type that = {
                        result_::apply_::call(expr.proto_base().proto_args_, state, visitor)
                    };
                    return that;
                }
            };

            template<BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, typename A), typename... Rest>
            struct function<BOOST_PP_ENUM_PARAMS(BOOST_PROTO_MAX_ARITY, A), Rest...>
              : nary_expr<tag::function BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_PROTO_MAX_ARITY, A), Rest...>
            {};

        #endif

        #define BOOST_PP_ITERATION_PARAMS_1 (3, (1, BOOST_PROTO_MAX_ARITY, <boost/xpressive/proto/detail/nary_expr.hpp>))
        #include BOOST_PP_ITERATE()

        }
    }}

    #include <boost/xpressive/proto/detail/undef.hpp>
    #endif

#else

    #define N BOOST_PP_ITERATION()

            template<typename Tag BOOST_PP_ENUM_TRAILING_PARAMS(N, typename A)>
            struct nary_expr<Tag BOOST_PP_ENUM_TRAILING_PARAMS(N, A)>
            {
                typedef expr<Tag, args<BOOST_PP_ENUM_PARAMS(N, A)> > type;
                typedef type proto_base_expr;

                template<typename Sig>
                struct result;

                template<typename This, typename Expr, typename State, typename Visitor>
                struct result<This(Expr, State, Visitor)>
                {
                    typedef detail::apply_args<
                        UNREF(Expr)::proto_args
                      , State
                      , Visitor
                        #define TMP(Z, M, DATA) , BOOST_PP_CAT(A, BOOST_PP_DEC(N))
                      , args<BOOST_PP_ENUM_PARAMS(N, A) BOOST_PP_REPEAT_FROM_TO(N, BOOST_PROTO_MAX_ARITY, TMP, ~)>
                        #undef TMP
                    > apply_;
                    typedef expr<UNREF(Expr)::proto_tag, typename apply_::type> type;
                };

                template<typename Expr, typename State, typename Visitor>
                typename result<nary_expr(Expr const &, State const &, Visitor &)>::type
                operator()(Expr const &expr, State const &state, Visitor &visitor) const
                {
                    typedef result<nary_expr(Expr const &, State const &, Visitor &)> result_;
                    typename result_::type that = {
                        result_::apply_::call(expr.proto_base().proto_args_, state, visitor)
                    };
                    return that;
                }
            };

            template<BOOST_PP_ENUM_PARAMS(N, typename A)>
            struct function<BOOST_PP_ENUM_PARAMS(N, A)>
              : nary_expr<tag::function BOOST_PP_ENUM_TRAILING_PARAMS(N, A)>
            {};

    #undef N

#endif
