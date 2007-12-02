///////////////////////////////////////////////////////////////////////////////
/// \file fold.hpp
/// Contains definition of the fold<> and reverse_fold<> transforms.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_TRANSFORM_FOLD_HPP_EAN_11_04_2007
#define BOOST_PROTO_TRANSFORM_FOLD_HPP_EAN_11_04_2007

#include <boost/fusion/include/fold.hpp>
#include <boost/fusion/include/reverse.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/fusion.hpp>
#include <boost/xpressive/proto/traits.hpp>

#define UNREF(T)\
    typename remove_reference<T>::type

#define UNCVREF(T)\
    typename remove_cv<UNREF(T)>::type

namespace boost { namespace proto
{

    namespace transform
    {

        namespace detail
        {

            template<typename Transform, typename Visitor>
            struct as_callable
            {
                as_callable(Visitor &v)
                  : v_(v)
                {}

                typedef when<_, Transform> Tfx;

                template<typename Sig>
                struct result;

                template<typename This, typename Expr, typename State>
                struct result<This(Expr, State)>
                  : boost::result_of<Tfx(UNCVREF(Expr), UNCVREF(State), Visitor)>
                {};

                template<typename Expr, typename State>
                typename boost::result_of<Tfx(Expr, State, Visitor)>::type
                operator()(Expr const &expr, State const &state) const
                {
                    return Tfx()(expr, state, this->v_);
                }

            private:
                Visitor &v_;
            };

            struct reverse : function_transform
            {
                template<typename Sig> struct result;

                template<typename This, typename T>
                struct result<This(T)>
                  : fusion::result_of::reverse<T const>
                {};

                template<typename T>
                typename fusion::result_of::reverse<T const>::type
                operator()(T const &t) const
                {
                    return fusion::reverse(t);
                }
            };

        } // namespace detail

        template<typename Sequence, typename State0, typename Fun>
        struct fold : raw_transform
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
              : fusion::result_of::fold<
                    typename boost::result_of<when<_, Sequence>(Expr, State, Visitor)>::type
                  , typename boost::result_of<when<_, State0>(Expr, State, Visitor)>::type
                  , detail::as_callable<Fun, Visitor>
                >
            {};

            template<typename Expr, typename State, typename Visitor>
            typename result<fold(Expr, State, Visitor)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                detail::as_callable<Fun, Visitor> fun(visitor);
                return fusion::fold(
                    when<_, Sequence>()(expr, state, visitor)
                  , when<_, State0>()(expr, state, visitor)
                  , fun
                );
            }
        };

        template<typename Sequence, typename State, typename Fun>
        struct reverse_fold
          : fold<detail::reverse(Sequence), State, Fun>
        {};

    }

    template<typename Sequence, typename State, typename Fun>
    struct transform_category<transform::fold<Sequence, State, Fun> >
    {
        typedef raw_transform type;
    };

    template<typename Sequence, typename State, typename Fun>
    struct transform_category<transform::reverse_fold<Sequence, State, Fun> >
    {
        typedef raw_transform type;
    };

}}

#undef UNREF
#undef UNCVREF

#endif
