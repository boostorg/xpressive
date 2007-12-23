///////////////////////////////////////////////////////////////////////////////
/// \file extends.hpp
/// Macros and a base class for defining end-user expression types
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_EXTENDS_HPP_EAN_11_1_2006
#define BOOST_PROTO_EXTENDS_HPP_EAN_11_1_2006

#include <boost/mpl/apply_wrap.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/traits.hpp>
#include <boost/xpressive/proto/expr.hpp>
#include <boost/xpressive/proto/args.hpp>
#include <boost/xpressive/proto/traits.hpp>
#include <boost/xpressive/proto/generate.hpp>

namespace boost { namespace proto
{
    /// INTERNAL ONLY
    ///
    #define BOOST_PROTO_CONST0

    /// INTERNAL ONLY
    ///
    #define BOOST_PROTO_CONST1 const

    #define BOOST_PROTO_EXTENDS(Expr, Derived, Domain)                                              \
        Expr expr;                                                                                  \
                                                                                                    \
        typedef Expr proto_base_expr;                                                               \
        typedef Domain proto_domain;                                                                \
        typedef Derived proto_derived_expr;                                                         \
        typedef typename Expr::proto_tag proto_tag;                                                 \
        typedef typename Expr::proto_args proto_args;                                               \
        static long const proto_arity = Expr::proto_arity;                                          \
        typedef void proto_is_expr_;                                                                \
        typedef boost::proto::tag::proto_expr fusion_tag;                                           \
                                                                                                    \
        static Derived make(Expr const &expr)                                                       \
        {                                                                                           \
            Derived that = {expr};                                                                  \
            return that;                                                                            \
        }                                                                                           \
                                                                                                    \
        Expr &proto_base()                                                                          \
        {                                                                                           \
            return this->expr;                                                                      \
        }                                                                                           \
                                                                                                    \
        Expr const &proto_base() const                                                              \
        {                                                                                           \
            return this->expr;                                                                      \
        }

#if defined(BOOST_HAS_VARIADIC_TMPL) && defined(BOOST_HAS_RVALUE_REFS)
        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_EXTENDS_ASSIGN_IMPL_(Const)                                                 \
        template<typename A>                                                                        \
        typename boost::proto::result_of::make_expr_ref<                                            \
            boost::proto::tag::assign                                                               \
          , proto_domain                                                                            \
          , proto_derived_expr BOOST_PROTO_CONST ## Const &                                         \
          , A                                                                                       \
        >::type const                                                                               \
        operator =(A &&a) BOOST_PROTO_CONST ## Const                                                \
        {                                                                                           \
            return boost::proto::result_of::make_expr_ref<                                          \
                boost::proto::tag::assign                                                           \
              , proto_domain                                                                        \
              , proto_derived_expr BOOST_PROTO_CONST ## Const &                                     \
              , A                                                                                   \
            >::call(*static_cast<proto_derived_expr BOOST_PROTO_CONST ## Const *>(this), a);        \
        }

    #define BOOST_PROTO_EXTENDS_ASSIGN()                                                            \
        BOOST_PROTO_EXTENDS_ASSIGN_IMPL_(0)                                                         \
        BOOST_PROTO_EXTENDS_ASSIGN_IMPL_(1)

        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_EXTENDS_SUBSCRIPT_IMPL_(Const)                                              \
        template<typename A>                                                                        \
        typename boost::proto::result_of::make_expr_ref<                                            \
            boost::proto::tag::subscript                                                            \
          , proto_domain                                                                            \
          , proto_derived_expr BOOST_PROTO_CONST ## Const &                                         \
          , A                                                                                       \
        >::type const                                                                               \
        operator [](A &&a) BOOST_PROTO_CONST ## Const                                               \
        {                                                                                           \
            return boost::proto::result_of::make_expr_ref<                                          \
                boost::proto::tag::subscript                                                        \
              , proto_domain                                                                        \
              , proto_derived_expr BOOST_PROTO_CONST ## Const &                                     \
              , A                                                                                   \
            >::call(*static_cast<proto_derived_expr BOOST_PROTO_CONST ## Const *>(this), a);        \
        }

    #define BOOST_PROTO_EXTENDS_SUBSCRIPT()                                                         \
        BOOST_PROTO_EXTENDS_SUBSCRIPT_IMPL_(0)                                                      \
        BOOST_PROTO_EXTENDS_SUBSCRIPT_IMPL_(1)

        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_EXTENDS_FUNCTION_IMPL_(Const)                                               \
        template<typename... A>                                                                     \
        typename boost::proto::result_of::make_expr_ref<                                            \
            boost::proto::tag::function                                                             \
          , proto_domain                                                                            \
          , proto_derived_expr BOOST_PROTO_CONST ## Const &                                         \
          , A...                                                                                    \
        >::type const                                                                               \
        operator ()(A &&... a) BOOST_PROTO_CONST ## Const                                           \
        {                                                                                           \
            return boost::proto::result_of::make_expr_ref<                                          \
                boost::proto::tag::function                                                         \
              , proto_domain                                                                        \
              , proto_derived_expr BOOST_PROTO_CONST ## Const &                                     \
              , A...                                                                                \
            >::call(*static_cast<proto_derived_expr BOOST_PROTO_CONST ## Const *>(this), a...);     \
        }

    #define BOOST_PROTO_EXTENDS_FUNCTION()                                                          \
        BOOST_PROTO_EXTENDS_FUNCTION_IMPL_(0)                                                       \
        BOOST_PROTO_EXTENDS_FUNCTION_IMPL_(1)

#else
        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_EXTENDS_ASSIGN_IMPL_(Const)                                                 \
        template<typename A>                                                                        \
        typename boost::proto::result_of::make_expr_ref<                                            \
            boost::proto::tag::assign                                                               \
          , proto_domain                                                                            \
          , proto_derived_expr BOOST_PROTO_CONST ## Const &                                         \
          , A &                                                                                     \
        >::type const                                                                               \
        operator =(A &a) BOOST_PROTO_CONST ## Const                                                 \
        {                                                                                           \
            return boost::proto::result_of::make_expr_ref<                                          \
                boost::proto::tag::assign                                                           \
              , proto_domain                                                                        \
              , proto_derived_expr BOOST_PROTO_CONST ## Const &                                     \
              , A &                                                                                 \
            >::call(*static_cast<proto_derived_expr BOOST_PROTO_CONST ## Const *>(this), a);        \
        }                                                                                           \
        template<typename A>                                                                        \
        typename boost::proto::result_of::make_expr_ref<                                            \
            boost::proto::tag::assign                                                               \
          , proto_domain                                                                            \
          , proto_derived_expr BOOST_PROTO_CONST ## Const &                                         \
          , A const &                                                                               \
        >::type const                                                                               \
        operator =(A const &a) BOOST_PROTO_CONST ## Const                                           \
        {                                                                                           \
            return boost::proto::result_of::make_expr_ref<                                          \
                boost::proto::tag::assign                                                           \
              , proto_domain                                                                        \
              , proto_derived_expr BOOST_PROTO_CONST ## Const &                                     \
              , A const &                                                                           \
            >::call(*static_cast<proto_derived_expr BOOST_PROTO_CONST ## Const *>(this), a);        \
        }

    #define BOOST_PROTO_EXTENDS_ASSIGN()                                                            \
        BOOST_PROTO_EXTENDS_ASSIGN_IMPL_(0)                                                         \
        BOOST_PROTO_EXTENDS_ASSIGN_IMPL_(1)

        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_EXTENDS_SUBSCRIPT_IMPL_(Const)                                              \
        template<typename A>                                                                        \
        typename boost::proto::result_of::make_expr_ref<                                            \
            boost::proto::tag::subscript                                                            \
          , proto_domain                                                                            \
          , proto_derived_expr BOOST_PROTO_CONST ## Const &                                         \
          , A &                                                                                     \
        >::type const                                                                               \
        operator [](A &a) BOOST_PROTO_CONST ## Const                                                \
        {                                                                                           \
            return boost::proto::result_of::make_expr_ref<                                          \
                boost::proto::tag::subscript                                                        \
              , proto_domain                                                                        \
              , proto_derived_expr BOOST_PROTO_CONST ## Const &                                     \
              , A &                                                                                 \
            >::call(*static_cast<proto_derived_expr BOOST_PROTO_CONST ## Const *>(this), a);        \
        }                                                                                           \
        template<typename A>                                                                        \
        typename boost::proto::result_of::make_expr_ref<                                            \
            boost::proto::tag::subscript                                                            \
          , proto_domain                                                                            \
          , proto_derived_expr BOOST_PROTO_CONST ## Const &                                         \
          , A const &                                                                               \
        >::type const                                                                               \
        operator [](A const &a) BOOST_PROTO_CONST ## Const                                          \
        {                                                                                           \
            return boost::proto::result_of::make_expr_ref<                                          \
                boost::proto::tag::subscript                                                        \
              , proto_domain                                                                        \
              , proto_derived_expr BOOST_PROTO_CONST ## Const &                                     \
              , A const &                                                                           \
            >::call(*static_cast<proto_derived_expr BOOST_PROTO_CONST ## Const *>(this), a);        \
        }

    #define BOOST_PROTO_EXTENDS_SUBSCRIPT()                                                         \
        BOOST_PROTO_EXTENDS_SUBSCRIPT_IMPL_(0)                                                      \
        BOOST_PROTO_EXTENDS_SUBSCRIPT_IMPL_(1)

        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_EXTENDS_FUNCTION_IMPL_1                                                     \
        ((0)(1))

        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_EXTENDS_FUNCTION_IMPL_2(R, PRODUCT)                                         \
        BOOST_PROTO_EXTENDS_FUNCTION_IMPL_8(R, BOOST_PP_SEQ_SIZE(PRODUCT), PRODUCT)

        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_EXTENDS_FUNCTION_IMPL_3(R, STATE)                                           \
        BOOST_PP_NOT_EQUAL(BOOST_PROTO_MAX_ARITY, BOOST_PP_SEQ_SIZE(STATE))

        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_EXTENDS_FUNCTION_IMPL_4(R, STATE)                                           \
        STATE BOOST_PROTO_EXTENDS_FUNCTION_IMPL_1

        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_EXTENDS_FUNCTION_IMPL_5(R, _, I, ELEM)                                      \
        BOOST_PP_COMMA_IF(I)                                                                        \
        BOOST_PP_CAT(A, I) BOOST_PP_CAT(BOOST_PROTO_CONST, ELEM) &BOOST_PP_CAT(a, I)

        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_EXTENDS_FUNCTION_IMPL_6(R, _, I, ELEM)                                      \
        , BOOST_PP_CAT(A, I) BOOST_PP_CAT(BOOST_PROTO_CONST, ELEM) &

        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_EXTENDS_FUNCTION_IMPL_7(R, STATE)                                           \
        BOOST_PP_SEQ_FOR_EACH_PRODUCT_R(R, BOOST_PROTO_EXTENDS_FUNCTION_IMPL_2, STATE)

        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_EXTENDS_FUNCTION_IMPL_8(R, SIZE, PRODUCT)                                   \
        BOOST_PROTO_EXTENDS_FUNCTION_IMPL_10(R, SIZE, PRODUCT, 0)                                   \
        BOOST_PROTO_EXTENDS_FUNCTION_IMPL_10(R, SIZE, PRODUCT, 1)

        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_EXTENDS_FUNCTION_IMPL_9(Const)                                              \
        typename boost::proto::result_of::make_expr_ref<                                            \
            boost::proto::tag::function                                                             \
          , proto_domain                                                                            \
          , proto_derived_expr BOOST_PROTO_CONST ## Const &                                         \
        >::type const                                                                               \
        operator ()() BOOST_PROTO_CONST ## Const                                                    \
        {                                                                                           \
            return boost::proto::result_of::make_expr_ref<                                          \
                boost::proto::tag::function                                                         \
              , proto_domain                                                                        \
              , proto_derived_expr BOOST_PROTO_CONST ## Const &                                     \
            >::call(*static_cast<proto_derived_expr BOOST_PROTO_CONST ## Const *>(this));           \
        }

        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_EXTENDS_FUNCTION_IMPL_10(R, SIZE, PRODUCT, Const)                           \
        template<BOOST_PP_ENUM_PARAMS(SIZE, typename A)>                                            \
        typename boost::proto::result_of::make_expr_ref<                                            \
            boost::proto::tag::function                                                             \
          , proto_domain                                                                            \
          , proto_derived_expr BOOST_PROTO_CONST ## Const &                                         \
            BOOST_PP_SEQ_FOR_EACH_I_R(R, BOOST_PROTO_EXTENDS_FUNCTION_IMPL_6, ~, PRODUCT)           \
        >::type const                                                                               \
        operator ()(BOOST_PP_SEQ_FOR_EACH_I_R(R, BOOST_PROTO_EXTENDS_FUNCTION_IMPL_5, ~, PRODUCT))  \
            BOOST_PROTO_CONST ## Const                                                              \
        {                                                                                           \
            return boost::proto::result_of::make_expr_ref<                                          \
                boost::proto::tag::function                                                         \
              , proto_domain                                                                        \
              , proto_derived_expr BOOST_PROTO_CONST ## Const &                                     \
                BOOST_PP_SEQ_FOR_EACH_I_R(R, BOOST_PROTO_EXTENDS_FUNCTION_IMPL_6, ~, PRODUCT)       \
            >::call(                                                                                \
                *static_cast<proto_derived_expr BOOST_PROTO_CONST ## Const *>(this)                 \
                BOOST_PP_ENUM_TRAILING_PARAMS(SIZE, a)                                              \
            );                                                                                      \
        }

    #define BOOST_PROTO_EXTENDS_FUNCTION()                                                          \
        BOOST_PROTO_EXTENDS_FUNCTION_IMPL_9(0)                                                      \
        BOOST_PROTO_EXTENDS_FUNCTION_IMPL_9(1)                                                      \
        BOOST_PP_FOR(                                                                               \
            BOOST_PROTO_EXTENDS_FUNCTION_IMPL_1                                                     \
          , BOOST_PROTO_EXTENDS_FUNCTION_IMPL_3                                                     \
          , BOOST_PROTO_EXTENDS_FUNCTION_IMPL_4                                                     \
          , BOOST_PROTO_EXTENDS_FUNCTION_IMPL_7                                                     \
        )

#endif

    namespace exprns_
    {
        /// \brief Empty type to be used as a dummy template parameter of
        ///     POD expression wrappers. It allows argument-dependent lookup
        ///     to find Proto's operator overloads.
        ///
        /// \c proto::is_proto_expr allows argument-dependent lookup
        ///     to find Proto's operator overloads. For example:
        ///
        /// \code
        /// template<typename T, typename Dummy = proto::is_proto_expr>
        /// struct my_terminal
        /// {
        ///     BOOST_PROTO_EXTENDS(
        ///         typename proto::terminal<T>::type
        ///       , my_terminal<T>
        ///       , default_domain
        ///     )
        /// };
        ///
        /// // ...
        /// my_terminal<int> _1, _2;
        /// _1 + _2; // OK, uses proto::operator+
        /// \endcode
        ///
        /// Without the second \c Dummy template parameter, Proto's operator
        /// overloads would not be considered by name lookup.
        struct is_proto_expr
        {};

        /// \brief extends\<\> class template for adding behaviors to a proto expression template
        ///
        template<typename Expr, typename Derived, typename Domain>
        struct extends
        {
            extends()
              : expr()
            {}

            extends(extends const &that)
              : expr(that.expr)
            {}

            extends(Expr const &expr_)
              : expr(expr_)
            {}

            BOOST_PROTO_EXTENDS(Expr, Derived, Domain)
            BOOST_PROTO_EXTENDS_ASSIGN()
            BOOST_PROTO_EXTENDS_SUBSCRIPT()
            BOOST_PROTO_EXTENDS_FUNCTION()
        };

    } // namespace exprns_

}}

#endif
