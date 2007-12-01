///////////////////////////////////////////////////////////////////////////////
// make_expr.hpp
//
//  Copyright 2006 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include <boost/xpressive/proto/proto.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/test/unit_test.hpp>

using namespace boost;
using namespace proto;

template<typename E> struct ewrap;

struct mydomain
  : domain<generator<ewrap> >
{};

template<typename E> struct ewrap
  : extends<E, ewrap<E>, mydomain>
{
    ewrap(E const &e = E())
      : extends<E, ewrap<E>, mydomain>(e)
    {}
};

void test_make_expr()
{
    int i = 42;
    posit<terminal<int>::type>::type p1 = make_expr<tag::posit>(1);
    posit<terminal<int>::type>::type p2 = make_expr<tag::posit>(i);
    BOOST_CHECK_EQUAL(arg(arg(p2)), 42);

    ewrap<posit<ewrap<terminal<int>::type> >::type> p3 = make_expr<tag::posit, mydomain>(i);
    BOOST_CHECK_EQUAL(arg(arg(p3)), 42);

    ewrap<plus<
        ewrap<posit<ewrap<terminal<int>::type> >::type>
      , ewrap<terminal<int>::type>
    >::type> p4 = make_expr<tag::plus>(p3, 0);
    BOOST_CHECK_EQUAL(arg(arg(left(p4))), 42);
}

void test_unpack_expr()
{
    int i = 42;
    fusion::vector<int> v1(1);
    fusion::vector<int&> v2(i);
    posit<terminal<int>::type>::type p1 = unpack_expr<tag::posit>(v1);
    posit<terminal<int>::type>::type p2 = unpack_expr<tag::posit>(v2);
    BOOST_CHECK_EQUAL(arg(arg(p2)), 42);

    ewrap<posit<ewrap<terminal<int>::type> >::type> p3 = unpack_expr<tag::posit, mydomain>(v2);
    BOOST_CHECK_EQUAL(arg(arg(p3)), 42);

    fusion::vector<ewrap<posit<ewrap<terminal<int>::type> >::type>, int> v3(p3, 0);
    ewrap<plus<
        ewrap<posit<ewrap<terminal<int>::type> >::type>
      , ewrap<terminal<int>::type>
    >::type> p4 = unpack_expr<tag::plus>(v3);
    BOOST_CHECK_EQUAL(arg(arg(left(p4))), 42);
}

using namespace unit_test;
///////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("test make_expr, unpack_expr and friends");

    test->add(BOOST_TEST_CASE(&test_make_expr));
    test->add(BOOST_TEST_CASE(&test_unpack_expr));

    return test;
}
