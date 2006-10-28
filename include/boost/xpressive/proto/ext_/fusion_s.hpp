/////////////////////////////////////////////////////////////////////////////////
///// \file fusion_s.hpp
///// Make a binary proto parse tree a valid Fusion sequence
////
////  Copyright 2004 Eric Niebler. Distributed under the Boost
////  Software License, Version 1.0. (See accompanying file
////  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//#ifndef BOOST_PROTO_EXT_FUSION_S_HPP_EAN_10_23_2006
//#define BOOST_PROTO_EXT_FUSION_S_HPP_EAN_10_23_2006
//
//#include <boost/mpl/if.hpp>
//#include <boost/utility/enable_if.hpp>
//#include <boost/type_traits/is_same.hpp>
//#include <boost/type_traits/add_const.hpp>
//#include <boost/type_traits/add_reference.hpp>
//#include <boost/xpressive/proto/proto.hpp>
//
//#ifndef BOOST_PROTO_FUSION_V2
//# error Segmented Fusion support for Proto requires Fusion v2.
//#endif
//
//#include <boost/fusion/sequence/view/joint_view.hpp>
//#include <boost/fusion/sequence/view/single_view.hpp>
//#include <boost/fusion/sequence/view/transform_view.hpp>
//#include <boost/fusion/support/ext_/is_segmented.hpp>
//#include <boost/fusion/sequence/intrinsic/ext_/segments.hpp>
//
//namespace boost { namespace proto
//{
//    template<typename T>
//    struct single_view_of
//    {
//        typedef fusion::single_view<
//            typename add_reference<typename add_const<T>::type>::type
//        > const type;
//    };
//
//    template<typename Expr, typename EnableIf = void>
//    struct elements_of;
//
//    template<typename Expr>
//    struct elements_of<Expr, typename enable_if<is_unary<Expr> >::type>
//    {
//        typedef typename single_view_of<typename arg_type<Expr>::type>::type type;
//
//        static type make(Expr &expr)
//        {
//            return type(proto::arg(expr));
//        }
//    };
//
//    template<typename Expr>
//    struct elements_of<Expr, typename enable_if<is_binary<Expr> >::type>
//    {
//        typedef typename single_view_of<typename left_type<Expr>::type>::type left_view;
//        typedef typename single_view_of<typename right_type<Expr>::type>::type right_view;
//        typedef fusion::joint_view<left_view, right_view> const type;
//
//        static type make(Expr &expr)
//        {
//            return type(left_view(proto::left(expr)), right_view(proto::right(expr)));
//        }
//    };
//
//    template<typename Expr>
//    struct elements_of<Expr, typename enable_if<is_nary<Expr> >::type>
//    {
//        typedef typename single_view_of<typename Expr::functor_type>::type functor_view;
//        typedef fusion::joint_view<functor_view, typename Expr::args_type const> const type;
//
//        static type make(Expr &expr)
//        {
//            return type(functor_view(expr.functor), expr.args);
//        }
//    };
//
//    template<typename Tag>
//    struct as_element
//    {
//        template<typename Expr>
//        struct result
//          : mpl::if_<
//                is_same<Tag, typename tag_type<Expr>::type>
//              , typename add_reference<Expr>::type
//              , fusion::single_view<typename add_reference<Expr>::type>
//            >
//        {};
//
//        template<typename Expr>
//        typename result<Expr>::type operator()(Expr &expr) const
//        {
//            return typename result<Expr>::type(expr);
//        }
//    };
//
//    template<typename Sequence>
//    struct segments_impl
//    {
//        typedef typename tag_type<Sequence>::type tag_type;
//        typedef fusion::transform_view<
//            typename elements_of<Sequence>::type
//          , as_element<tag_type>
//        > type;
//
//        static type call(Sequence &sequence)
//        {
//            return type(elements_of<Sequence>::make(sequence), as_element<tag_type>());
//        }
//    };
//}}
//
//namespace boost { namespace fusion { namespace extension
//{
//    template<>
//    struct is_segmented_impl<proto::proto_sequence_tag>
//    {
//        template<typename Sequence>
//        struct apply
//          : mpl::true_
//        {};
//    };
//
//    template<>
//    struct segments_impl<proto::proto_sequence_tag>
//    {
//        template<typename Sequence>
//        struct apply
//          : proto::segments_impl<Sequence>
//        {};
//    };
//}}}
//
//#endif
