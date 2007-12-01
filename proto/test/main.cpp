//#define BOOST_STRICT_CONFIG

#include <cstdio>
#include <iostream>
#include <typeinfo>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/min_max.hpp>
#include <boost/fusion/include/cons.hpp>
#include <boost/fusion/include/pop_front.hpp>
#include <boost/fusion/include/reverse.hpp>
#include <boost/xpressive/proto/proto.hpp>
#include <boost/xpressive/proto/transform.hpp>
#include <boost/xpressive/proto/context.hpp>

namespace boost { namespace fusion
{
    inline std::ostream &operator<<(std::ostream &sout, nil)
    {
        return sout << "<nil>";
    }
}}

using namespace boost::proto;
using namespace transform;
namespace mpl = boost::mpl;
namespace fusion = boost::fusion;

struct any_tag {};
struct Any : terminal<any_tag> {};
struct Char : terminal<char> {};

terminal<any_tag>::type char_;

template<typename Grammar, typename Expr>
void check(Expr const &)
{
    BOOST_MPL_ASSERT((matches<Expr, Grammar>));
}

template<typename Grammar, typename Expr>
void check_not(Expr const &)
{
    BOOST_MPL_ASSERT_NOT((matches<Expr, Grammar>));
}

template<typename T, typename U>
struct pair
{};

struct make_negate : function_transform
{
    template<typename Sig> struct result;

    template<typename This, typename T>
    struct result<This(T)>
      : negate<T>
    {};

    template<typename T>
    typename negate<T>::type
    operator()(T const &t) const
    {
        typename negate<T>::type that = {t};
        return that;
    }
};

//struct make_cons : function_transform
//{
//    template<typename Sig> struct result;
//
//    template<typename This, typename T, typename U>
//    struct result<This(T, U)>
//    {
//        typedef fusion::cons<T, U> type;
//    };
//
//    template<typename T, typename U>
//    fusion::cons<T, U>
//    operator()(T const &t, U const &u) const
//    {
//        return fusion::cons<T, U>(t, u);
//    }
//};

// perhaps change the default transform from pass-through to make-node, to
// be compatible with the ops role as a meta-function.
struct Promote
  : or_<
        when< terminal<float>, terminal<double>::type(_arg) >
      , when< posit<Promote>, make_negate(Promote(_arg))    >
      , when< terminal<char const *>, std::string(_arg)     >
      , when< terminal<_> >
      , when< nary_expr<_, vararg<Promote> > >
    >
{};

template<typename Int>
struct placeholder : Int
{};

struct zero : mpl::int_<0> {};
struct one  : mpl::int_<1> {};
struct two  : mpl::int_<2> {};
terminal<placeholder<one> >::type const _1 = {};
terminal<placeholder<two> >::type const _2 = {};

struct Arity
  : or_<
        when< terminal<placeholder<_> >
             , _arg
        >
      , when< terminal<_>
             , zero()
        >
      , when< nary_expr<_, vararg<Arity> >
             , fold<_, zero(), mpl::max<Arity, _state>() >
        >
    >
{};

struct ArgsAsList
  : when<
        function<terminal<_>, vararg<terminal<_> > >
      , reverse_fold<
            pop_front(_)
          , fusion::nil()
          , fusion::cons<_arg, _state>(_arg, _state)
        >
    >
{};

struct FoldTreeToList
  : or_<
        when<assign<_, terminal<_> >
             , _arg(_right)
        >
      , when<terminal<_>
             , _arg
        >
      , when<
            comma<FoldTreeToList, FoldTreeToList>
          , reverse_fold_tree<
                _
              , fusion::nil()
              , fusion::cons<FoldTreeToList, _state>(FoldTreeToList, _state)
            >
        >
    >
{};

struct noncopy
{
    noncopy() {}
    ~noncopy() {}
private:
    noncopy(const noncopy &);
    noncopy &operator=(const noncopy &);
};

struct disp
{
    template<typename T>
    void operator()(T const &t) const
    {
        std::printf("%s\n", typeid(T).name());
    }
};


template<typename E>
struct byvalexpr;

struct byvaldom
  : domain<by_value_generator<generator<byvalexpr> > >
{};

template<typename E>
struct byvalexpr
    : extends<E, byvalexpr<E>, byvaldom>
{
    explicit byvalexpr(E const &e = E())
      : extends<E, byvalexpr<E>, byvaldom>(e)
    {}

    using extends<E, byvalexpr<E>, byvaldom>::operator=;
};

byvalexpr<terminal<int>::type> A;
byvalexpr<terminal<int>::type> B;
byvalexpr<terminal<int>::type> C;

int main()
{
    int dummy=0;
    noncopy non_;
    
    terminal<int>::type u = {42};
    terminal<int>::type const t = {42};

    int j=0;

    expr<tag::function,
        args<
            expr<tag::terminal, term<int> > const &
          , expr<tag::terminal, term<int const &> >
          , expr<tag::terminal, term<int &> >
          , expr<tag::terminal, term<int> > const &
        >
    > that = t(1,j,t);

    function<
        expr<tag::terminal, term<int> > const &
      , expr<tag::terminal, term<int const &> >
      , expr<tag::terminal, term<int &> >
      , expr<tag::terminal, term<int> > const &
    >::type other = t(1,j,t);

    expr<tag::assign,
        args<
            expr<tag::terminal, term<int> > const &
          , expr<tag::terminal, term<int const &> >
        >
    > that2 = (t = 1);

    expr<tag::posit,
        args<expr<tag::terminal, term<int> > const &>
    > that3 = +t;

    expr<tag::posit,
        args<expr<tag::terminal, term<int> > &>
    > that4 = +u;

    expr<tag::plus,
        args<
            expr<tag::terminal, term<int> > &
          , expr<tag::terminal, term<int &> >
        >
    > that5 = u + j;

    std::printf("%d %d\n", arg_c<0>(arg_c<0>(that5)), arg_c<0>(arg_c<1>(that5)));

    check<_>(u+j);
    check<terminal<int> >(u);
    check<plus<terminal<int>, terminal<int&> > >(u + j);
    check<plus<terminal<int>, terminal<int> > >(u + j);

    terminal<pair<int,double> >::type w = {};
    check<terminal<pair<int,double> > >(w);
    check<terminal<pair<_,double> > >(w);

    check<
        or_<
            minus<terminal<int>, terminal<int> >
          , plus<terminal<int>, terminal<int> >
        >
    >(u + j);

    check<function<Any> >(char_());
    check<function<Any, Char, Char> >(char_('a', 'b'));

    check_not<function<Any, Char> >(char_());
    check<function<Any, Char> >(char_('a'));
    check_not<function<Any, Char> >(char_('a', 'b'));

    check<function<Any, vararg<Char> > >(char_());
    check<function<Any, vararg<Char> > >(char_('a'));
    check<function<Any, vararg<Char> > >(char_('a', 'b'));

    terminal<float>::type ff = {1.F};
    check<Promote>(ff+ff);
    plus<terminal<double>::type, terminal<double>::type>::type dd = 
        Promote::call(ff+ff, dummy, non_);

    plus<terminal<double>::type, terminal<int>::type>::type du = 
        Promote::call(ff+u, dummy, non_);
    std::printf("%g %d\n", arg_c<0>(arg_c<0>(du)), arg_c<0>(arg_c<1>(du)));

    plus<negate<terminal<double>::type>::type, terminal<int>::type>::type ndu = 
        Promote::call(+ff+u, dummy, non_);
    std::printf("%g %d\n", arg_c<0>(arg_c<0>(arg_c<0>(ndu))), arg_c<0>(arg_c<1>(ndu)));

    terminal<char const *>::type sz = {"hello"};
    std::string str = Promote::call(sz, dummy, non_);

    std::printf(
        "%d %d %d\n"
      , (int)Arity::call(sz, dummy, non_)
      , (int)Arity::call(_1 + 0, dummy, non_)
      , (int)Arity::call(_2 + _1, dummy, non_)
    );

    using fusion::cons;
    cons<char, cons<int, cons<float> > > mylist1 =
        ArgsAsList::call(_1('a', 42, 3.14f), dummy, non_);
    std::cout << mylist1.car << ' ' << mylist1.cdr.car << ' ' << mylist1.cdr.cdr.car << std::endl;

    cons<int, cons<char, cons<std::string> > > mylist2
        (FoldTreeToList::call( (_1 = 1, 'a', str), dummy, non_ ));
    std::cout << mylist2.car << ' ' << mylist2.cdr.car << ' ' << mylist2.cdr.cdr.car << std::endl;

    default_context ctx;
    int r1 = eval(as_expr(1) + as_expr(2), ctx);
    std::cout << r1 << std::endl;

    display_expr((_1 = 1, 'a', str));

    byvalexpr<
        expr<tag::plus
          , args<
                byvalexpr<expr<tag::terminal, term<int> > >
              , byvalexpr<expr<tag::divides, args<
                    byvalexpr<expr<tag::terminal, term<int> > >
                  , byvalexpr<expr<tag::terminal, term<int> > >
                > > >
            >
        >
    > bve = A+B/C;

    return 0;
}
