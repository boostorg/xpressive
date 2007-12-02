///////////////////////////////////////////////////////////////////////////////
/// \file call.hpp
/// Contains definition of the call<> transform.
//
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROTO_TRANSFORM_CALL_HPP_EAN_11_02_2007
#define BOOST_PROTO_TRANSFORM_CALL_HPP_EAN_11_02_2007

#include <boost/utility/result_of.hpp>
#include <boost/xpressive/proto/proto_fwd.hpp>
#include <boost/xpressive/proto/traits.hpp>

namespace boost { namespace proto
{

    namespace transform
    {
        namespace detail
        {
            template<typename T>
            T &uncv(T const &t)
            {
                return const_cast<T &>(t);
            }
            
            struct dont_care { dont_care(...); };

            typedef char (&yes_type)[2];
            typedef char no_type;

            struct private_type_
            {
                private_type_ const &operator,(int) const;
            };

            template<typename T>
            yes_type check_fun_arity(T const &);

            no_type check_fun_arity(private_type_ const &);

            template<typename Fun>
            struct fun_wrap : Fun
            {
                fun_wrap();
                typedef private_type_ const &(*pfun1)(dont_care);
                typedef private_type_ const &(*pfun2)(dont_care, dont_care);
                typedef private_type_ const &(*pfun3)(dont_care, dont_care, dont_care);
                operator pfun1() const;
                operator pfun2() const;
                operator pfun3() const;
            };

            template<typename Fun, typename Expr, typename State, typename Visitor>
            struct fun_arity
            {
                static fun_wrap<Fun> &fun_;
                static Expr &expr_;
                static State &state_;
                static Visitor &visitor_;

                static int const value =
                    (sizeof(check_fun_arity((fun_(expr_), 0)))-1)? 1 :
                    (sizeof(check_fun_arity((fun_(expr_, state_), 0)))-1)? 2 :
                    (sizeof(check_fun_arity((fun_(expr_, state_, visitor_), 0)))-1)? 3 :
                    -1;
            };
            
            template<
                typename Fun
              , typename Expr
              , typename State
              , typename Visitor
              , int Needs
              , int Arity =
                    (fun_arity<Fun, Expr, State, Visitor>::value > Needs) ?
                    fun_arity<Fun, Expr, State, Visitor>::value : Needs
            >
            struct call_;

            template<typename Fun, typename Expr, typename State, typename Visitor, int Needs>
            struct call_<Fun, Expr, State, Visitor, Needs, 1>
            {
                typedef typename boost::result_of<Fun(Expr)>::type type;
                
                template<typename A, typename B, typename C>
                static type call(A &&expr, B &&, C &&)
                {
                    Fun f;
                    return f(expr);
                }
            };

            template<typename Fun, typename Expr, typename State, typename Visitor, int Needs>
            struct call_<Fun, Expr, State, Visitor, Needs, 2>
            {
                typedef typename boost::result_of<Fun(Expr, State)>::type type;
                
                template<typename A, typename B, typename C>
                static type call(A &&expr, B &&state, C &&)
                {
                    Fun f;
                    return f(expr, state);
                }
            };
            
            template<typename Fun, typename Expr, typename State, typename Visitor, int Needs>
            struct call_<Fun, Expr, State, Visitor, Needs, 3>
            {
                typedef typename boost::result_of<Fun(Expr, State, Visitor)>::type type;
                
                template<typename A, typename B, typename C>
                static type call(A &&expr, B &&state, C &&visitor)
                {
                    Fun f;
                    return f(expr, state, visitor);
                }
            };
        }
        
        template<typename Fun, typename... Args>
        struct call : raw_transform
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
              : boost::result_of<
                    Fun(typename boost::result_of<when<_, Args>(Expr, State, Visitor)>::type...)
                >
            {};

            template<typename Expr, typename State, typename Visitor>
            typename result<call(Expr, State, Visitor)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                Fun f;
                return f(when<_, Args>()(expr, state, visitor)...);
            }
        };
        
        template<typename Fun, typename Arg0>
        struct call<Fun, Arg0> : raw_transform
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
              : detail::call_<
                    Fun
                  , typename boost::result_of<when<_, Arg0>(Expr, State, Visitor)>::type
                  , State
                  , Visitor
                  , 1
                >
            {};

            template<typename Expr, typename State, typename Visitor>
            typename result<call(Expr, State, Visitor)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                return result<call(Expr, State, Visitor)>::call(
                    when<_, Arg0>()(expr, state, visitor)
                  , state
                  , visitor
                );
            }
        };
        
        template<typename Fun, typename Arg0, typename Arg1>
        struct call<Fun, Arg0, Arg1> : raw_transform
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
              : detail::call_<
                    Fun
                  , typename boost::result_of<when<_, Arg0>(Expr, State, Visitor)>::type
                  , typename boost::result_of<when<_, Arg1>(Expr, State, Visitor)>::type
                  , Visitor
                  , 2
                >
            {};

            template<typename Expr, typename State, typename Visitor>
            typename result<call(Expr, State, Visitor)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                return result<call(Expr, State, Visitor)>::call(
                    when<_, Arg0>()(expr, state, visitor)
                  , when<_, Arg1>()(expr, state, visitor)
                  , visitor
                );
            }
        };
        
        template<typename Fun, typename Arg0, typename Arg1, typename Arg2>
        struct call<Fun, Arg0, Arg1, Arg2> : raw_transform
        {
            template<typename Sig>
            struct result;

            template<typename This, typename Expr, typename State, typename Visitor>
            struct result<This(Expr, State, Visitor)>
              : boost::result_of<
                    Fun(
                        typename boost::result_of<when<_, Arg0>(Expr, State, Visitor)>::type
                      , typename boost::result_of<when<_, Arg1>(Expr, State, Visitor)>::type
                      , typename boost::result_of<when<_, Arg2>(Expr, State, Visitor)>::type
                    )
                >
            {};

            template<typename Expr, typename State, typename Visitor>
            typename result<call(Expr, State, Visitor)>::type
            operator()(Expr const &expr, State const &state, Visitor &visitor) const
            {
                Fun f;
                return f(
                    when<_, Arg0>()(expr, state, visitor)
                  , when<_, Arg1>()(expr, state, visitor)
                  , detail::uncv(when<_, Arg2>()(expr, state, visitor)) // HACK
                );
            }
        };

        template<typename Fun, typename... Args>
        struct call<Fun(Args...)>
          : call<Fun, Args...>
        {};

    }

    template<typename Fun, typename... Args>
    struct transform_category<transform::call<Fun, Args...> >
    {
        typedef raw_transform type;
    };

}}

#endif
