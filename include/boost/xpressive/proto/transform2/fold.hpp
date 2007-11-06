#ifndef BOOST_PP_IS_ITERATING
    ///////////////////////////////////////////////////////////////////////////////
    /// \file fold.hpp
    /// A special-purpose proto transform for merging sequences of binary operations.
    /// It transforms the right operand and passes the result as state while transforming
    /// the left. Or, it might do the left first, if you choose.
    //
    //  Copyright 2004 Eric Niebler. Distributed under the Boost
    //  Software License, Version 1.0. (See accompanying file
    //  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    #ifndef BOOST_PROTO_TRANSFORM2_FOLD_HPP_EAN_12_16_2006
    #define BOOST_PROTO_TRANSFORM2_FOLD_HPP_EAN_12_16_2006

    #include <boost/xpressive/proto/detail/prefix.hpp>
    #include <boost/preprocessor/cat.hpp>
    #include <boost/preprocessor/arithmetic/inc.hpp>
    #include <boost/preprocessor/arithmetic/sub.hpp>
    #include <boost/preprocessor/iteration/iterate.hpp>
    #include <boost/preprocessor/repetition/repeat.hpp>
    #include <boost/xpressive/proto/proto_fwd.hpp>
    #include <boost/xpressive/proto/traits.hpp>
    #include <boost/xpressive/proto/transform2/branch.hpp>
    #include <boost/xpressive/proto/detail/suffix.hpp>

    // BUGBUG
    #include <boost/xpressive/proto/fusion.hpp>
    #include <boost/fusion/include/fold.hpp>
    #include <boost/fusion/include/pop_front.hpp>
    #include <boost/fusion/include/reverse_view.hpp>

    namespace boost { namespace proto { namespace transform2
    {

        namespace detail_
        {
            template<typename T>
            struct uncvref
              : remove_cv<typename remove_reference<T>::type>
            {};

            template<typename Transform, typename Visitor>
            struct as_callable
            {
                as_callable(Visitor &v)
                  : v_(v)
                {}

                typedef typename detail_::as_transform<Transform>::type Tfx;

                template<typename Sig>
                struct result;

                template<typename This, typename Expr, typename State>
                struct result<This(Expr, State)>
                {
                    typedef typename Tfx::template apply<
                        typename uncvref<Expr>::type
                      , typename uncvref<State>::type
                      , Visitor
                    >::type type;
                };

                template<typename Expr, typename State>
                typename Tfx::template apply<Expr, State, Visitor>::type
                operator()(Expr const &expr, State const &state) const
                {
                    return Tfx::call(expr, state, this->v_);
                }

            private:
                Visitor &v_;
            };

            template<typename Transform, typename Expr, typename State, typename Visitor,
                long Arity = Expr::proto_arity::value>
            struct fold_impl
            {};

            template<typename Transform, typename Expr, typename State, typename Visitor,
                long Arity = Expr::proto_arity::value>
            struct reverse_fold_impl
            {};

            #define BOOST_PROTO_ARG_N_TYPE(n)\
                BOOST_PP_CAT(proto_arg, n)\
                /**/

            #define BOOST_PROTO_FOLD_STATE_TYPE(z, n, data)                                         \
                typedef typename Transform::template                                                \
                    apply<                                                                          \
                        typename Expr::BOOST_PROTO_ARG_N_TYPE(n)::proto_base_expr                   \
                      , BOOST_PP_CAT(state, n)                                                      \
                      , Visitor                                                                     \
                    >::type                                                                         \
                BOOST_PP_CAT(state, BOOST_PP_INC(n));                                               \
                /**/

            #define BOOST_PROTO_FOLD_STATE(z, n, data)                                              \
                BOOST_PP_CAT(state, BOOST_PP_INC(n)) const &                                        \
                    BOOST_PP_CAT(s, BOOST_PP_INC(n)) = Transform::call(                             \
                        expr.BOOST_PP_CAT(arg, n).proto_base()                                      \
                      , BOOST_PP_CAT(s, n)                                                          \
                      , visitor                                                                     \
                    );                                                                              \
                /**/

            #define BOOST_PROTO_REVERSE_FOLD_STATE_TYPE(z, n, data)                                 \
                typedef typename Transform::template                                                \
                    apply<                                                                          \
                        typename Expr::BOOST_PROTO_ARG_N_TYPE(                                      \
                            BOOST_PP_SUB(data, BOOST_PP_INC(n)))::proto_base_expr                   \
                      , BOOST_PP_CAT(state, BOOST_PP_SUB(data, n))                                  \
                      , Visitor                                                                     \
                    >::type                                                                         \
                BOOST_PP_CAT(state, BOOST_PP_SUB(data, BOOST_PP_INC(n)));                           \
                /**/

            #define BOOST_PROTO_REVERSE_FOLD_STATE(z, n, data)                                      \
                BOOST_PP_CAT(state, BOOST_PP_SUB(data, BOOST_PP_INC(n))) const &                    \
                    BOOST_PP_CAT(s, BOOST_PP_SUB(data, BOOST_PP_INC(n))) = Transform::call(         \
                        expr.BOOST_PP_CAT(arg, BOOST_PP_SUB(data, BOOST_PP_INC(n))).proto_base()    \
                      , BOOST_PP_CAT(s, BOOST_PP_SUB(data, n))                                      \
                      , visitor                                                                     \
                    );                                                                              \
                /**/

            #define BOOST_PP_ITERATION_PARAMS_1 (3, (1, BOOST_PROTO_MAX_ARITY, <boost/xpressive/proto/transform2/fold.hpp>))
            #include BOOST_PP_ITERATE()

            #undef BOOST_PROTO_REVERSE_FOLD_STATE
            #undef BOOST_PROTO_REVERSE_FOLD_STATE_TYPE
            #undef BOOST_PROTO_FOLD_STATE
            #undef BOOST_PROTO_FOLD_STATE_TYPE
            #undef BOOST_PROTO_ARG_N_TYPE
        }

        template<typename Sequence, typename State, typename Fun>
        struct fold
          : transform_base
        {
            typedef typename detail_::as_transform<Sequence>::type MakeSequence;
            typedef typename detail_::as_transform<State>::type MakeState;

            template<typename E, typename S, typename V>
            struct apply
              : fusion::result_of::fold<
                    typename MakeSequence::template apply<E, S, V>::type
                  , typename MakeState::template apply<E, S, V>::type
                  , detail_::as_callable<Fun, V>
                >
            {};

            template<typename E, typename S, typename V>
            static typename apply<E, S, V>::type
            call(E const &e, S const &s, V &v)
            {
                detail_::as_callable<Fun, V> fun(v);
                return fusion::fold(
                    MakeSequence::call(e, s, v)
                  , MakeState::call(e, s, v)
                  , fun
                );
            }
        };

        // Compile-time optimization:
        template<typename State, typename Fun>
        struct fold<_children, State, Fun>
          : transform_base
        {
            typedef typename detail_::as_transform<State>::type make_state;
            typedef typename detail_::as_transform<Fun>::type transform_type;

            template<typename E, typename S, typename V>
            struct apply
              : detail_::reverse_fold_impl<
                    transform_type
                  , E
                  , typename make_state::template apply<E, S, V>::type
                  , V
                >
            {};

            template<typename E, typename S, typename V>
            static typename apply<E, S, V>::type
            call(E const &e, S const &s, V &v)
            {
                return apply<E, S, V>::call(e, make_state::call(e, s, v), v);
            }
        };

        template<typename Sequence, typename State, typename Fun>
        struct reverse_fold
          : transform_base
        {
            typedef typename detail_::as_transform<Sequence>::type MakeSequence;
            typedef typename detail_::as_transform<State>::type MakeState;

            template<typename E, typename S, typename V>
            struct apply
              : fusion::result_of::fold<
                    fusion::reverse_view<typename MakeSequence::template apply<E, S, V>::type>
                  , typename MakeState::template apply<E, S, V>::type
                  , detail_::as_callable<Fun, V>
                >
            {};

            template<typename E, typename S, typename V>
            static typename apply<E, S, V>::type
            call(E const &e, S const &s, V &v)
            {
                detail_::as_callable<Fun, V> fun(v);
                typedef typename MakeSequence::template apply<E, S, V>::type Seq;
                Seq seq = MakeSequence::call(e, s, v);
                return fusion::fold(
                    fusion::reverse_view<Seq>(seq)
                  , MakeState::call(e, s, v)
                  , fun
                );
            }
        };

        template<typename State, typename Fun>
        struct reverse_fold<_children, State, Fun>
          : transform_base
        {
            typedef typename detail_::as_transform<State>::type make_state;
            typedef typename detail_::as_transform<Fun>::type transform_type;

            template<typename E, typename S, typename V>
            struct apply
              : detail_::reverse_fold_impl<
                    transform_type
                  , E
                  , typename make_state::template apply<E, S, V>::type
                  , V
                >
            {};

            template<typename E, typename S, typename V>
            static typename apply<E, S, V>::type
            call(E const &e, S const &s, V &v)
            {
                return apply<E, S, V>::call(e, make_state::call(e, s, v), v);
            }
        };

        struct _children
          : transform_base
        {
            template<typename E, typename, typename>
            struct apply
            {
                typedef proto::children<E const> type;
            };

            template<typename E, typename S, typename V>
            static proto::children<E const> call(E const &e, S const &, V &)
            {
                return proto::children_of(e);
            };
        };

        template<typename Sequence>
        struct pop_front
          : transform_base
        {
            template<typename E, typename S, typename V>
            struct apply
              : fusion::result_of::pop_front<typename Sequence::template apply<E, S, V>::type const>
            {};

            template<typename E, typename S, typename V>
            static typename apply<E, S, V>::type
            call(E const &e, S const &s, V &v)
            {
                return fusion::pop_front(Sequence::call(e, s, v));
            };
        };

    }}}

    namespace boost { namespace proto
    {
        template<typename Sequence, typename State, typename Fun>
        struct is_transform<transform2::fold<Sequence, State, Fun> >
          : mpl::true_
        {};

        template<typename Sequence, typename State, typename Fun>
        struct is_transform<transform2::reverse_fold<Sequence, State, Fun> >
          : mpl::true_
        {};

        template<>
        struct is_transform<transform2::_children>
          : mpl::true_
        {};

        template<typename Sequence>
        struct is_transform<transform2::pop_front<Sequence> >
          : mpl::true_
        {};
    }}

    #endif

#else

    #define N BOOST_PP_ITERATION()

            template<typename Transform, typename Expr, typename state0, typename Visitor>
            struct fold_impl<Transform, Expr, state0, Visitor, N>
            {
                BOOST_PP_REPEAT(N, BOOST_PROTO_FOLD_STATE_TYPE, N)
                typedef BOOST_PP_CAT(state, N) type;

                static type call(Expr const &expr, state0 const &s0, Visitor &visitor)
                {
                    BOOST_PP_REPEAT(N, BOOST_PROTO_FOLD_STATE, N)
                    return BOOST_PP_CAT(s, N);
                }
            };

            template<typename Transform, typename Expr, typename BOOST_PP_CAT(state, N), typename Visitor>
            struct reverse_fold_impl<Transform, Expr, BOOST_PP_CAT(state, N), Visitor, N>
            {
                BOOST_PP_REPEAT(N, BOOST_PROTO_REVERSE_FOLD_STATE_TYPE, N)
                typedef state0 type;

                static type call(Expr const &expr, BOOST_PP_CAT(state, N) const &BOOST_PP_CAT(s, N), Visitor &visitor)
                {
                    BOOST_PP_REPEAT(N, BOOST_PROTO_REVERSE_FOLD_STATE, N)
                    return s0;
                }
            };

    #undef N

#endif
