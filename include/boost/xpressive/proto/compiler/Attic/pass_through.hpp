/////////////////////////////////////////////////////////////////////////////////
///// \file pass_through.hpp
///// 
////
////  Copyright 2004 Eric Niebler. Distributed under the Boost
////  Software License, Version 1.0. (See accompanying file
////  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//#ifndef BOOST_PROTO_COMPILER_PASS_THROUGH_HPP_EAN_04_23_2006
//#define BOOST_PROTO_COMPILER_PASS_THROUGH_HPP_EAN_04_23_2006
//
//#include <boost/utility/enable_if.hpp>
//#include <boost/type_traits/is_same.hpp>
//#include <boost/xpressive/proto/proto_fwd.hpp>
//
//namespace boost { namespace proto
//{
//    ///////////////////////////////////////////////////////////////////////////////
//    // pass_through_compiler
//    template<typename DomainTag>
//    struct pass_through_compiler
//    {
//        // compile_helper
//        template<typename Expr, typename State, typename Visitor, typename EnableIf = void>
//        struct compile_helper
//        {
//            typedef typename meta::compile<Expr, State, Visitor, DomainTag>::type type;
//
//            static type const &call(type const &expr, State const &state, Visitor &visitor, int)
//            {
//                return expr;
//            }
//
//            static type const call(Expr const &expr, State const &state, Visitor &visitor, ...)
//            {
//                return proto::compile(expr, state, visitor, DomainTag());
//            }
//        };
//
//        template<typename Expr, typename State, typename Visitor>
//        struct compile_helper<Expr, State, Visitor, typename disable_if<is_expr<Expr> >::type>
//        {
//            typedef Expr type;
//
//            static Expr const &call(Expr const &expr, State const &state, Visitor &visitor, int)
//            {
//                return expr;
//            }
//        };
//
//        // apply_helper
//        template<typename Expr, typename State, typename Visitor, typename EnableIf>
//        struct apply_helper;
//
//        // is_unary apply
//        template<typename Expr, typename State, typename Visitor>
//        struct apply_helper<Expr, State, Visitor, typename enable_if<is_unary<Expr> >::type>
//        {
//            typedef typename arg_type<Expr>::type arg_type;
//            typedef compile_helper<arg_type, State, Visitor> compiler_type;
//            typedef typename compiler_type::type new_arg_type;
//            typedef unary_expr<new_arg_type, typename tag_type<Expr>::type> type;
//            typedef typename mpl::if_<is_same<Expr, type>, Expr const &, type const>::type const_reference;
//
//            static type const &call(type const &expr, State const &state, Visitor &visitor, int)
//            {
//                return expr; // pass-through, no recompilation necessary.
//            }
//
//            static type const call(Expr const &expr, State const &state, Visitor &visitor, ...)
//            {
//                return proto::make_expr<typename tag_type<Expr>::type>(
//                    compiler_type::call(proto::arg(expr), state, visitor, 0)
//                );
//            }
//        };
//
//        // is_binary apply
//        template<typename Expr, typename State, typename Visitor>
//        struct apply_helper<Expr, State, Visitor, typename enable_if<is_binary<Expr> >::type>
//        {
//            typedef typename left_type<Expr>::type left_type;
//            typedef typename right_type<Expr>::type right_type;
//            typedef compile_helper<left_type, State, Visitor> left_compiler_type;
//            typedef compile_helper<right_type, State, Visitor> right_compiler_type;
//            typedef typename left_compiler_type::type new_left_type;
//            typedef typename right_compiler_type::type new_right_type;
//            typedef binary_expr<new_left_type, new_right_type, typename tag_type<Expr>::type> type;
//            typedef typename mpl::if_<is_same<Expr, type>, Expr const &, type const>::type const_reference;
//
//            static type const &call(type const &expr, State const &state, Visitor &visitor, int)
//            {
//                return expr; // pass-through, no recompilation necessary.
//            }
//
//            static type const call(Expr const &expr, State const &state, Visitor &visitor, ...)
//            {
//                return proto::make_expr<typename tag_type<Expr>::type>(
//                    left_compiler_type::call(proto::left(expr), state, visitor, 0)
//                  , right_compiler_type::call(proto::right(expr), state, visitor, 0)
//                );
//            }
//        };
//
//        // is_nary apply
//        template<typename Expr, typename State, typename Visitor>
//        struct apply_helper<Expr, State, Visitor, typename enable_if<is_nary<Expr> >::type>
//        {
//            // BUGBUG handle nary_op here
//            typedef Expr type;
//            typedef Expr const &const_reference;
//
//            static Expr const &call(Expr const &expr, State const &, Visitor &, int)
//            {
//                return expr;
//            }
//        };
//
//        template<typename Expr, typename State, typename Visitor>
//        struct apply
//          : apply_helper<Expr, State, Visitor, void>
//        {
//        };
//
//        template<typename Expr, typename State, typename Visitor>
//        static typename apply<Expr, State, Visitor>::const_reference
//        call(Expr const &expr, State const &state, Visitor &visitor)
//        {
//            return apply<Expr, State, Visitor>::call(expr, state, visitor, 0);
//        }
//    };
//
//}}
//
//#endif
