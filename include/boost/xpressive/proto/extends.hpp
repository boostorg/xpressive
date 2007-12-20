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
        }                                                                                           \
        /**/

        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_EXTENDS_ASSIGN_IMPL_(Expr, Derived, Domain, Const)                          \
        template<typename A>                                                                        \
        typename boost::mpl::apply_wrap1<                                                           \
            Domain                                                                                  \
          , boost::proto::expr<                                                                     \
                boost::proto::tag::assign                                                           \
              , boost::proto::args<                                                                 \
                    Derived BOOST_PROTO_CONST ## Const &                                            \
                  , typename boost::proto::result_of::as_expr_ref<A, Domain>::type                  \
                >                                                                                   \
            >                                                                                       \
        >::type const                                                                               \
        operator =(A &&a) BOOST_PROTO_CONST ## Const                                                \
        {                                                                                           \
            typedef                                                                                 \
                boost::proto::expr<                                                                 \
                    boost::proto::tag::assign                                                       \
                  , boost::proto::args<                                                             \
                        Derived BOOST_PROTO_CONST ## Const &                                        \
                      , typename boost::proto::result_of::as_expr_ref<A, Domain>::type              \
                    >                                                                               \
                >                                                                                   \
            that_type;                                                                              \
            that_type that = {                                                                      \
                {*static_cast<Derived BOOST_PROTO_CONST ## Const *>(this)                           \
              , {boost::proto::result_of::as_expr_ref<A, Domain>::call(a)}}                              \
            };                                                                                      \
            return Domain::make(that);                                                              \
        }                                                                                           \
        /**/

    #define BOOST_PROTO_EXTENDS_ASSIGN_CONST(Expr, Derived, Domain)                                 \
        BOOST_PROTO_EXTENDS_ASSIGN_IMPL_(Expr, Derived, Domain, 1)

    #define BOOST_PROTO_EXTENDS_ASSIGN_NON_CONST(Expr, Derived, Domain)                             \
        BOOST_PROTO_EXTENDS_ASSIGN_IMPL_(Expr, Derived, Domain, 0)

    #define BOOST_PROTO_EXTENDS_ASSIGN(Expr, Derived, Domain)                                       \
        BOOST_PROTO_EXTENDS_ASSIGN_CONST(Expr, Derived, Domain)                                     \
        BOOST_PROTO_EXTENDS_ASSIGN_NON_CONST(Expr, Derived, Domain)                                 \
        /**/

        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_EXTENDS_SUBSCRIPT_IMPL_(Expr, Derived, Domain, Const)                       \
        template<typename A>                                                                        \
        typename boost::mpl::apply_wrap1<                                                           \
            Domain                                                                                  \
          , boost::proto::expr<                                                                     \
                boost::proto::tag::subscript                                                        \
              , boost::proto::args<                                                                 \
                    Derived BOOST_PROTO_CONST ## Const &                                            \
                  , typename boost::proto::result_of::as_expr_ref<A, Domain>::type                  \
              >                                                                                     \
            >                                                                                       \
        >::type const                                                                               \
        operator [](A &&a) BOOST_PROTO_CONST ## Const                                               \
        {                                                                                           \
            typedef                                                                                 \
                boost::proto::expr<                                                                 \
                    boost::proto::tag::subscript                                                    \
                  , boost::proto::args<                                                             \
                        Derived BOOST_PROTO_CONST ## Const &                                        \
                      , typename boost::proto::result_of::as_expr_ref<A, Domain>::type              \
                    >                                                                               \
                >                                                                                   \
            that_type;                                                                              \
            that_type that = {                                                                      \
                {*static_cast<Derived BOOST_PROTO_CONST ## Const *>(this)                           \
              , {boost::proto::result_of::as_expr_ref<A, Domain>::call(a)}}                         \
            };                                                                                      \
            return Domain::make(that);                                                              \
        }                                                                                           \
        /**/

    #define BOOST_PROTO_EXTENDS_SUBSCRIPT_CONST(Expr, Derived, Domain)                              \
        BOOST_PROTO_EXTENDS_SUBSCRIPT_IMPL_(Expr, Derived, Domain, 1)

    #define BOOST_PROTO_EXTENDS_SUBSCRIPT_NON_CONST(Expr, Derived, Domain)                          \
        BOOST_PROTO_EXTENDS_SUBSCRIPT_IMPL_(Expr, Derived, Domain, 0)

    #define BOOST_PROTO_EXTENDS_SUBSCRIPT(Expr, Derived, Domain)                                    \
        BOOST_PROTO_EXTENDS_SUBSCRIPT_CONST(Expr, Derived, Domain)                                  \
        BOOST_PROTO_EXTENDS_SUBSCRIPT_NON_CONST(Expr, Derived, Domain)                              \
        /**/

        /// INTERNAL ONLY
        ///
    #define BOOST_PROTO_EXTENDS_FUNCTION_IMPL_(Expr, Derived, Domain, Const)                        \
        template<typename... A>                                                                     \
        typename boost::mpl::apply_wrap1<                                                           \
            Domain                                                                                  \
          , boost::proto::expr<                                                                     \
                boost::proto::tag::function                                                         \
              , boost::proto::args<                                                                 \
                    Derived BOOST_PROTO_CONST ## Const &                                            \
                  , typename boost::proto::result_of::as_expr_ref<A, Domain>::type...               \
                >                                                                                   \
            >                                                                                       \
        >::type const                                                                               \
        operator ()(A &&... a) BOOST_PROTO_CONST ## Const                                           \
        {                                                                                           \
           typedef                                                                                  \
                boost::proto::args<                                                                 \
                    Derived BOOST_PROTO_CONST ## Const &                                            \
                  , typename boost::proto::result_of::as_expr_ref<A, Domain>::type...               \
                >                                                                                   \
            args_type;                                                                              \
            typedef                                                                                 \
                boost::proto::expr<                                                                 \
                    boost::proto::tag::function                                                     \
                  , args_type                                                                       \
                >                                                                                   \
            that_type;                                                                              \
            that_type that = {                                                                      \
                boost::proto::argsns_::make_cons_<typename args_type::cons_type>(                   \
                    *static_cast<Derived BOOST_PROTO_CONST ## Const *>(this)                        \
                  , boost::proto::result_of::as_expr_ref<A, Domain>::call(a)...                     \
                )                                                                                   \
            };                                                                                      \
            return Domain::make(that);                                                              \
        }                                                                                           \
        /**/

    #define BOOST_PROTO_EXTENDS_FUNCTION_CONST(Expr, Derived, Domain)                               \
        BOOST_PROTO_EXTENDS_FUNCTION_IMPL_(Expr, Derived, Domain, 1)

    #define BOOST_PROTO_EXTENDS_FUNCTION_NON_CONST(Expr, Derived, Domain)                           \
        BOOST_PROTO_EXTENDS_FUNCTION_IMPL_(Expr, Derived, Domain, 0)

    #define BOOST_PROTO_EXTENDS_FUNCTION(Expr, Derived, Domain)                                     \
        BOOST_PROTO_EXTENDS_FUNCTION_CONST(Expr, Derived, Domain)                                   \
        BOOST_PROTO_EXTENDS_FUNCTION_NON_CONST(Expr, Derived, Domain)                               \
        /**/

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
            BOOST_PROTO_EXTENDS_ASSIGN(Expr, Derived, Domain)
            BOOST_PROTO_EXTENDS_SUBSCRIPT(Expr, Derived, Domain)
            BOOST_PROTO_EXTENDS_FUNCTION(Expr, Derived, Domain)
        };

    } // namespace exprns_

}}

#endif
