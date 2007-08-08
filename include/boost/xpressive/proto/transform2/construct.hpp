#ifndef BOOST_PP_IS_ITERATING
    ///////////////////////////////////////////////////////////////////////////////
    /// \file construct.hpp
    /// For constructing an arbitrary type from a bunch of transforms.
    //
    //  Copyright 2004 Eric Niebler. Distributed under the Boost
    //  Software License, Version 1.0. (See accompanying file
    //  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    #ifndef BOOST_PROTO_TRANSFORM2_CONSTRUCT_HPP_EAN_12_26_2006
    #define BOOST_PROTO_TRANSFORM2_CONSTRUCT_HPP_EAN_12_26_2006

    #include <boost/xpressive/proto/detail/prefix.hpp>
    #include <boost/preprocessor/iterate.hpp>
    #include <boost/preprocessor/facilities/intercept.hpp>
    #include <boost/preprocessor/repetition/enum.hpp>
    #include <boost/preprocessor/repetition/enum_params.hpp>
    #include <boost/preprocessor/repetition/enum_trailing.hpp>
    #include <boost/preprocessor/repetition/enum_binary_params.hpp>
    #include <boost/preprocessor/repetition/enum_trailing_params.hpp>
    #include <boost/mpl/bool.hpp>
    #include <boost/type_traits/is_pod.hpp>
    #include <boost/xpressive/proto/proto_fwd.hpp>
    #include <boost/xpressive/proto/detail/suffix.hpp>

    namespace boost { namespace proto { namespace transform2
    {
        namespace detail_
        {
            template<typename T>
            struct is_aggregate
              : is_pod<T>
            {};

            template<typename Tag, typename Args, long N>
            struct is_aggregate<expr<Tag, Args, N> >
              : mpl::true_
            {};

            template<typename T, bool HasType = mpl::aux::has_type<T>::value>
            struct nested_type
            {
                typedef typename T::type type;
            };

            template<typename T>
            struct nested_type<T, false>
            {
                typedef T type;
            };

            template<
                typename OriginalT, typename Expr, typename State, typename Visitor, typename AppliedT
                BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(BOOST_PROTO_MAX_ARITY, typename A, = no_type BOOST_PP_INTERCEPT)
              , typename EnableIf = void
            >
            struct nested_type_if
              : nested_type<AppliedT>
            {
                typedef yes_type proto_transform_applied;
            };

            template<
                typename OriginalT, typename Expr, typename State, typename Visitor, typename AppliedT
                BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_PROTO_MAX_ARITY, typename A)
            >
            struct nested_type_if<
                OriginalT, Expr, State, Visitor, AppliedT
                BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_PROTO_MAX_ARITY, A)
              , typename AppliedT::proto_is_transform
            >
              : OriginalT::template apply<Expr, State, Visitor>
            {
                typedef yes_type proto_transform_applied;
            };

            template<typename OriginalT, typename Expr, typename State, typename Visitor, typename AppliedT>
            struct nested_type_if<
                OriginalT, Expr, State, Visitor, AppliedT
                BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_PROTO_MAX_ARITY, no_type BOOST_PP_INTERCEPT)
              , void
            >
            {
                typedef AppliedT type;
                typedef no_type proto_transform_applied;
            };

            template<typename Transform, typename EnableIf>
            struct as_transform
            {
                typedef case_<_, typename remove_pointer<Transform>::type> type;
            };

            template<typename Transform>
            struct as_transform<Transform, typename Transform::proto_is_transform>
            {
                typedef Transform type;
            };

            template<typename R, typename Expr, typename State, typename Visitor
                     BOOST_MPL_AUX_LAMBDA_ARITY_PARAM(long Arity = mpl::aux::template_arity<R>::value)>
            struct apply_aux_
            {
                typedef R type;
                typedef no_type proto_transform_applied;
            };

            template<typename R, typename Expr, typename State, typename Visitor, bool IsTransform = is_transform<R>::value>
            struct apply_
              : apply_aux_<R, Expr, State, Visitor>
            {};

            template<typename R, typename Expr, typename State, typename Visitor>
            struct apply_<R, Expr, State, Visitor, true>
              : nested_type<typename R::template apply<Expr, State, Visitor>::type>
            {
                typedef yes_type proto_transform_applied;
            };

            // work around GCC bug
            template<typename Tag, typename Args, long N, typename Expr, typename State, typename Visitor>
            struct apply_<expr<Tag, Args, N>, Expr, State, Visitor, false>
            {
                typedef expr<Tag, Args, N> type;
                typedef no_type proto_transform_applied;
            };

            template<typename T>
            void ignore_unused(T const &)
            {}
        }

        template<typename Grammar, typename ConstructorFun>
        struct construct
          : transform_base
        {
            template<typename Expr, typename State, typename Visitor>
            struct apply
              : ConstructorFun::template apply<typename Grammar::template apply<Expr, State, Visitor>::type, State, Visitor>
            {};

            template<typename Expr, typename State, typename Visitor>
            static typename apply<Expr, State, Visitor>::type
            call(Expr const &expr, State const &state, Visitor &visitor)
            {
                return ConstructorFun::call(Grammar::call(expr, state, visitor), state, visitor);
            }
        };

        template<typename Grammar, typename Transform>
        struct case_
          : transform_base
        {
            typedef typename Grammar::proto_base_expr proto_base_expr;

            template<typename Expr, typename State, typename Visitor>
            struct apply
              : construct<_, Transform>::template apply<Expr, State, Visitor>
            {};

            template<typename Expr, typename State, typename Visitor>
            static typename apply<Expr, State, Visitor>::type
            call(Expr const &expr, State const &state, Visitor &visitor)
            {
                return construct<_, Transform>::call(expr, state, visitor);
            }
        };

        template<typename Grammar, typename Transform>
        struct case_<Grammar ***, Transform>
          : transform_base
        {
            typedef typename Grammar::proto_base_expr proto_base_expr;

            template<typename Expr, typename State, typename Visitor>
            struct apply
              : construct<Grammar, Transform>::template apply<Expr, State, Visitor>
            {};

            template<typename Expr, typename State, typename Visitor>
            static typename apply<Expr, State, Visitor>::type
            call(Expr const &expr, State const &state, Visitor &visitor)
            {
                return construct<Grammar, Transform>::call(expr, state, visitor);
            }
        };

        template<typename Transform>
        struct default_
          : case_<_, Transform>
        {};

        #define BOOST_PROTO_APPLY_(Z, N, DATA)                                                      \
            typename apply_<BOOST_PP_CAT(DATA, N), Expr, State, Visitor>::type                      \
            /**/

        #define BOOST_PROTO_IS_APPLIED_(Z, N, DATA)                                                 \
            typename apply_<BOOST_PP_CAT(DATA, N), Expr, State, Visitor>::proto_transform_applied   \
            /**/

        #define BOOST_PP_ITERATION_PARAMS_1 (3, (0, BOOST_PROTO_MAX_ARITY, <boost/xpressive/proto/transform2/construct.hpp>))
        #include BOOST_PP_ITERATE()

        #undef BOOST_PROTO_APPLY_
        #undef BOOST_PROTO_IS_APPLIED_

    }}}

    namespace boost { namespace proto
    {
        template<typename Grammar, typename ConstructorFun>
        struct is_transform<transform2::construct<Grammar, ConstructorFun> >
          : mpl::true_
        {};

        template<typename Grammar, typename Transform>
        struct is_transform<transform2::case_<Grammar, Transform> >
          : mpl::true_
        {};

        template<typename Transform>
        struct is_transform<transform2::default_<Transform> >
          : mpl::true_
        {};
    }}

    #endif

#else

    #define N BOOST_PP_ITERATION()

        #if N > 0
        namespace detail_
        {
            template<
                template<BOOST_PP_ENUM_PARAMS(N, typename BOOST_PP_INTERCEPT)> class T
                BOOST_PP_ENUM_TRAILING_PARAMS(N, typename G),
                typename Expr, typename State, typename Visitor
            >
            struct apply_aux_<T<BOOST_PP_ENUM_PARAMS(N, G)>, Expr, State, Visitor BOOST_MPL_AUX_LAMBDA_ARITY_PARAM(N)>
              : nested_type_if<
                    T<BOOST_PP_ENUM_PARAMS(N, G)>, Expr, State, Visitor
                  , T<BOOST_PP_ENUM(N, BOOST_PROTO_APPLY_, G)>
                    BOOST_PP_ENUM_TRAILING(N, BOOST_PROTO_IS_APPLIED_, G)
                >
            {};
        }
        #endif

        template<typename Grammar, typename Result BOOST_PP_ENUM_TRAILING_PARAMS(N, typename Arg)>
        struct construct<Grammar, Result(BOOST_PP_ENUM_PARAMS(N, Arg))>
          : transform_base
        {
            template<typename Expr, typename State, typename Visitor>
            struct apply
              : detail_::apply_<Result, typename Grammar::template apply<Expr, State, Visitor>::type, State, Visitor>
            {};

            template<typename Expr, typename State, typename Visitor>
            static typename apply<Expr, State, Visitor>::type
            call(Expr const &expr, State const &state, Visitor &visitor)
            {
                typedef typename apply<Expr, State, Visitor>::type result_type;
                return construct::call_(expr, state, visitor, detail_::is_aggregate<result_type>());
            }

        private:
            /// INTERNAL ONLY
            ///
            template<typename Expr, typename State, typename Visitor>
            static typename apply<Expr, State, Visitor>::type
            call_(Expr const &expr, State const &state, Visitor &visitor, mpl::true_)
            {
                typename Grammar::template apply<Expr, State, Visitor>::type expr2
                    = Grammar::call(expr, state, visitor);
                detail_::ignore_unused(expr2);
                typename apply<Expr, State, Visitor>::type that = {
                    BOOST_PP_ENUM_BINARY_PARAMS(N, detail_::as_transform<Arg, >::type::call(expr2, state, visitor) BOOST_PP_INTERCEPT)
                };
                return that;
            }

            /// INTERNAL ONLY
            ///
            template<typename Expr, typename State, typename Visitor>
            static typename apply<Expr, State, Visitor>::type
            call_(Expr const &expr, State const &state, Visitor &visitor, mpl::false_)
            {
                typename Grammar::template apply<Expr, State, Visitor>::type expr2
                    = Grammar::call(expr, state, visitor);
                detail_::ignore_unused(expr2);
                return typename apply<Expr, State, Visitor>::type(
                    BOOST_PP_ENUM_BINARY_PARAMS(N, detail_::as_transform<Arg, >::type::call(expr2, state, visitor) BOOST_PP_INTERCEPT)
                );
            }
        };

    #undef N

#endif
