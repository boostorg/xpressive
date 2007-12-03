///////////////////////////////////////////////////////////////////////////////
/// \file default.hpp
/// Definintion of default_context, a default evaluation context for
/// proto::eval() that uses Boost.Typeof to deduce return types
/// of the built-in operators.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_CONTEXT_DEFAULT_HPP_EAN_01_08_2007
#define BOOST_PROTO_CONTEXT_DEFAULT_HPP_EAN_01_08_2007

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/mpl/if.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/type_traits.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/tags.hpp>
#include <boost/xpressive/proto/eval.hpp>
#include <boost/xpressive/proto/traits.hpp> // for proto::arg_c()

// If we're generating doxygen documentation, hide all the nasty
// Boost.Typeof gunk.
#ifndef BOOST_PROTO_DOXYGEN_INVOKED
    #define BOOST_PROTO_DECLTYPE_NESTED_TYPEDEF_TPL_(Nested, Expr)\
        BOOST_TYPEOF_NESTED_TYPEDEF_TPL(BOOST_PP_CAT(nested_and_hidden_, Nested), Expr)\
        struct Nested\
          : mpl::if_c<\
                1==sizeof(proto::detail::check_reference(Expr))\
              , typename BOOST_PP_CAT(nested_and_hidden_, Nested)::type &\
              , typename BOOST_PP_CAT(nested_and_hidden_, Nested)::type\
            >\
        {};

    #define BOOST_PROTO_DECLTYPE_(Expr, Type)\
        BOOST_PROTO_DECLTYPE_NESTED_TYPEDEF_TPL_(BOOST_PP_CAT(nested_, Type), (Expr))\
        typedef typename BOOST_PP_CAT(nested_, Type)::type Type;
#else
    /// INTERNAL ONLY
    ///
    #define BOOST_PROTO_DECLTYPE_NESTED_TYPEDEF_TPL_(Nested, Expr)
    /// INTERNAL ONLY
    ///
    #define BOOST_PROTO_DECLTYPE_(Expr, Type)\
        typedef detail::unspecified Type;
#endif

namespace boost { namespace proto
{
    namespace detail
    {
        template<typename T> T make();

        template<typename T>
        char check_reference(T &);

        template<typename T>
        char (&check_reference(T const &))[2];

        template<typename A0, typename A1>
        struct comma_result
        {
            BOOST_PROTO_DECLTYPE_((proto::detail::make<A0>(), proto::detail::make<A1>()), type)
        };

        template<typename A0>
        struct comma_result<A0, void>
        {
            typedef void type;
        };

        template<typename A1>
        struct comma_result<void, A1>
        {
            typedef A1 type;
        };

        template<>
        struct comma_result<void, void>
        {
            typedef void type;
        };

        template<typename T, typename U = T>
        struct result_of_fixup
          : mpl::if_<is_function<T>, T *, U>
        {};

        template<typename T, typename U>
        struct result_of_fixup<T &, U>
          : result_of_fixup<T, T>
        {};

        template<typename T, typename U>
        struct result_of_fixup<T *, U>
          : result_of_fixup<T, U>
        {};

        template<typename T, typename U>
        struct result_of_fixup<T const, U>
          : result_of_fixup<T, U>
        {};

        //// Tests for result_of_fixup
        //struct bar {};
        //BOOST_MPL_ASSERT((is_same<bar,        result_of_fixup<bar>::type>));
        //BOOST_MPL_ASSERT((is_same<bar const,  result_of_fixup<bar const>::type>));
        //BOOST_MPL_ASSERT((is_same<bar,        result_of_fixup<bar &>::type>));
        //BOOST_MPL_ASSERT((is_same<bar const,  result_of_fixup<bar const &>::type>));
        //BOOST_MPL_ASSERT((is_same<void(*)(),  result_of_fixup<void(*)()>::type>));
        //BOOST_MPL_ASSERT((is_same<void(*)(),  result_of_fixup<void(* const)()>::type>));
        //BOOST_MPL_ASSERT((is_same<void(*)(),  result_of_fixup<void(* const &)()>::type>));
        //BOOST_MPL_ASSERT((is_same<void(*)(),  result_of_fixup<void(&)()>::type>));

    #if BOOST_WORKAROUND(BOOST_MSVC, >= 1400)
        template<typename T> T &make_ref_(T &t);
        template<typename T> T const &make_ref_(T const &t);
        #define BOOST_PROTO_REF(x) proto::detail::make_ref_(x)
    #else
        #define BOOST_PROTO_REF(x) x
    #endif

        template<typename Expr, typename Context, typename Indices>
        struct default_eval_function_;

        template<typename Expr, typename Context, int Zero, int... Indices>
        struct default_eval_function_<Expr, Context, op::detail::indices<Zero, Indices...> >
        {
            typedef
                typename proto::detail::result_of_fixup<
                    typename proto::result_of::eval<
                        typename proto::result_of::arg_c<Expr, 0>::type
                      , Context
                    >::type
                >::type
            function_type;

            typedef
                typename boost::result_of<
                    function_type(
                        typename proto::result_of::eval<
                            typename proto::result_of::arg_c<Expr, Indices>::type
                          , Context
                        >::type...
                    )
                >::type
            result_type;

            result_type operator ()(Expr &expr, Context &context) const
            {
                return proto::eval(proto::arg_c<0>(expr), context)(
                    proto::eval(proto::arg_c<Indices>(expr), context)...
                );
            }
        };

    } // namespace detail

    namespace context
    {
        template<typename Expr, typename Context, typename Tag>
        struct default_eval
        {};

        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_UNARY_OP_RESULT(Op, Tag)                                                \
        template<typename Expr, typename Context>                                               \
        struct default_eval<Expr, Context, Tag>                                                 \
        {                                                                                       \
        private:                                                                                \
            static Expr &sexpr;                                                                 \
            static Context &sctx;                                                               \
        public:                                                                                 \
            BOOST_PROTO_DECLTYPE_(Op proto::eval(BOOST_PROTO_REF(proto::arg_c<0>(sexpr)), sctx), result_type)\
            result_type operator()(Expr &expr, Context &ctx) const                              \
            {                                                                                   \
                return Op proto::eval(proto::arg_c<0>(expr), ctx);                              \
            }                                                                                   \
        };                                                                                      \
        /**/

        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_BINARY_OP_RESULT(Op, Tag)                                               \
        template<typename Expr, typename Context>                                               \
        struct default_eval<Expr, Context, Tag>                                                 \
        {                                                                                       \
        private:                                                                                \
            static Expr &sexpr;                                                                 \
            static Context &sctx;                                                               \
        public:                                                                                 \
            BOOST_PROTO_DECLTYPE_(proto::eval(BOOST_PROTO_REF(proto::arg_c<0>(sexpr)), sctx) Op proto::eval(BOOST_PROTO_REF(proto::arg_c<1>(sexpr)), sctx), result_type)\
            result_type operator()(Expr &expr, Context &ctx) const                              \
            {                                                                                   \
                return proto::eval(proto::arg_c<0>(expr), ctx) Op proto::eval(proto::arg_c<1>(expr), ctx);\
            }                                                                                   \
        };                                                                                      \
        /**/

        BOOST_PROTO_UNARY_OP_RESULT(+, proto::tag::posit)
        BOOST_PROTO_UNARY_OP_RESULT(-, proto::tag::negate)
        BOOST_PROTO_UNARY_OP_RESULT(*, proto::tag::dereference)
        BOOST_PROTO_UNARY_OP_RESULT(~, proto::tag::complement)
        BOOST_PROTO_UNARY_OP_RESULT(&, proto::tag::address_of)
        BOOST_PROTO_UNARY_OP_RESULT(!, proto::tag::logical_not)
        BOOST_PROTO_UNARY_OP_RESULT(++, proto::tag::pre_inc)
        BOOST_PROTO_UNARY_OP_RESULT(--, proto::tag::pre_dec)

        BOOST_PROTO_BINARY_OP_RESULT(<<, proto::tag::shift_left)
        BOOST_PROTO_BINARY_OP_RESULT(>>, proto::tag::shift_right)
        BOOST_PROTO_BINARY_OP_RESULT(*, proto::tag::multiplies)
        BOOST_PROTO_BINARY_OP_RESULT(/, proto::tag::divides)
        BOOST_PROTO_BINARY_OP_RESULT(%, proto::tag::modulus)
        BOOST_PROTO_BINARY_OP_RESULT(+, proto::tag::plus)
        BOOST_PROTO_BINARY_OP_RESULT(-, proto::tag::minus)
        BOOST_PROTO_BINARY_OP_RESULT(<, proto::tag::less)
        BOOST_PROTO_BINARY_OP_RESULT(>, proto::tag::greater)
        BOOST_PROTO_BINARY_OP_RESULT(<=, proto::tag::less_equal)
        BOOST_PROTO_BINARY_OP_RESULT(>=, proto::tag::greater_equal)
        BOOST_PROTO_BINARY_OP_RESULT(==, proto::tag::equal_to)
        BOOST_PROTO_BINARY_OP_RESULT(!=, proto::tag::not_equal_to)
        BOOST_PROTO_BINARY_OP_RESULT(||, proto::tag::logical_or)
        BOOST_PROTO_BINARY_OP_RESULT(&&, proto::tag::logical_and)
        BOOST_PROTO_BINARY_OP_RESULT(&, proto::tag::bitwise_and)
        BOOST_PROTO_BINARY_OP_RESULT(|, proto::tag::bitwise_or)
        BOOST_PROTO_BINARY_OP_RESULT(^, proto::tag::bitwise_xor)
        BOOST_PROTO_BINARY_OP_RESULT(->*, proto::tag::mem_ptr)

        BOOST_PROTO_BINARY_OP_RESULT(=, proto::tag::assign)
        BOOST_PROTO_BINARY_OP_RESULT(<<=, proto::tag::shift_left_assign)
        BOOST_PROTO_BINARY_OP_RESULT(>>=, proto::tag::shift_right_assign)
        BOOST_PROTO_BINARY_OP_RESULT(*=, proto::tag::multiplies_assign)
        BOOST_PROTO_BINARY_OP_RESULT(/=, proto::tag::divides_assign)
        BOOST_PROTO_BINARY_OP_RESULT(%=, proto::tag::modulus_assign)
        BOOST_PROTO_BINARY_OP_RESULT(+=, proto::tag::plus_assign)
        BOOST_PROTO_BINARY_OP_RESULT(-=, proto::tag::minus_assign)
        BOOST_PROTO_BINARY_OP_RESULT(&=, proto::tag::bitwise_and_assign)
        BOOST_PROTO_BINARY_OP_RESULT(|=, proto::tag::bitwise_or_assign)
        BOOST_PROTO_BINARY_OP_RESULT(^=, proto::tag::bitwise_xor_assign)

        template<typename Expr, typename Context>

        struct default_eval<Expr, Context, proto::tag::terminal>
        {
            typedef
                typename mpl::if_<
                    is_const<Expr>
                  , typename proto::result_of::arg<Expr>::const_reference
                  , typename proto::result_of::arg<Expr>::reference
                >::type
            result_type;

            result_type operator()(Expr &expr, Context &) const
            {
                return proto::arg(expr);
            }
        };

        // Handle post-increment specially.
        template<typename Expr, typename Context>
        struct default_eval<Expr, Context, proto::tag::post_inc>
        {
        private:
            static Expr &sexpr;
            static Context &sctx;
        public:
            BOOST_PROTO_DECLTYPE_(proto::eval(BOOST_PROTO_REF(proto::arg_c<0>(sexpr)), sctx) ++, result_type)
            result_type operator()(Expr &expr, Context &ctx) const
            {
                return proto::eval(proto::arg_c<0>(expr), ctx) ++;
            }
        };

        // Handle post-decrement specially.
        template<typename Expr, typename Context>
        struct default_eval<Expr, Context, proto::tag::post_dec>
        {
        private:
            static Expr &sexpr;
            static Context &sctx;
        public:
            BOOST_PROTO_DECLTYPE_(proto::eval(BOOST_PROTO_REF(proto::arg_c<0>(sexpr)), sctx) --, result_type)
            result_type operator()(Expr &expr, Context &ctx) const
            {
                return proto::eval(proto::arg_c<0>(expr), ctx) --;
            }
        };

        // Handle subscript specially.
        template<typename Expr, typename Context>
        struct default_eval<Expr, Context, proto::tag::subscript>
        {
        private:
            static Expr &sexpr;
            static Context &sctx;
        public:
            BOOST_PROTO_DECLTYPE_(proto::eval(BOOST_PROTO_REF(proto::arg_c<0>(sexpr)), sctx)[proto::eval(BOOST_PROTO_REF(proto::arg_c<1>(sexpr)), sctx)], result_type)
            result_type operator()(Expr &expr, Context &ctx) const
            {
                return proto::eval(proto::arg_c<0>(expr), ctx)[proto::eval(proto::arg_c<1>(expr), ctx)];
            }
        };

        // Handle if_else_ specially.
        template<typename Expr, typename Context>
        struct default_eval<Expr, Context, proto::tag::if_else_>
        {
        private:
            static Expr &sexpr;
            static Context &sctx;
        public:
            BOOST_PROTO_DECLTYPE_(
                proto::eval(BOOST_PROTO_REF(proto::arg_c<0>(sexpr)), sctx)
              ? proto::eval(BOOST_PROTO_REF(proto::arg_c<1>(sexpr)), sctx)
              : proto::eval(BOOST_PROTO_REF(proto::arg_c<2>(sexpr)), sctx)
              , result_type
            )
            result_type operator()(Expr &expr, Context &ctx) const
            {
                return proto::eval(proto::arg_c<0>(expr), ctx)
                     ? proto::eval(proto::arg_c<1>(expr), ctx)
                     : proto::eval(proto::arg_c<2>(expr), ctx);
            }
        };

        // Handle comma specially.
        template<typename Expr, typename Context>
        struct default_eval<Expr, Context, proto::tag::comma>
        {
            typedef typename proto::result_of::eval<typename proto::result_of::arg_c<Expr, 0>::type, Context>::type proto_arg0;
            typedef typename proto::result_of::eval<typename proto::result_of::arg_c<Expr, 1>::type, Context>::type proto_arg1;
            typedef typename proto::detail::comma_result<proto_arg0, proto_arg1>::type result_type;
            result_type operator()(Expr &expr, Context &ctx) const
            {
                return proto::eval(proto::arg_c<0>(expr), ctx), proto::eval(proto::arg_c<1>(expr), ctx);
            }
        };

        // Handle function specially
        template<typename Expr, typename Context>
        struct default_eval<Expr, Context, tag::function>
          : proto::detail::default_eval_function_<
                Expr
              , Context
              , typename op::detail::make_indices<Expr::proto_arity>::type
            >
        {};

        /// default_context
        ///
        struct default_context
        {
            /// default_context::eval
            ///
            template<typename Expr, typename ThisContext = default_context const>
            struct eval
              : default_eval<Expr, ThisContext>
            {};
        };

    } // namespace context

}} // namespace boost::proto

#endif
