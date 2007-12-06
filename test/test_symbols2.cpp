///////////////////////////////////////////////////////////////////////////////
// test_symbols2.cpp
//
//  Copyright 2007 David Jenkins.
//  Copyright 2007 Eric Niebler.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <map>
#include <boost/version.hpp>
#include <boost/xpressive/xpressive_static.hpp>
#include <boost/xpressive/regex_actions.hpp>
#include <boost/test/unit_test.hpp>

#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

namespace xp = boost::xpressive;

///////////////////////////////////////////////////////////////////////////////
// test6
//  test case-sensitivity
void test6()
{
    using namespace boost::xpressive;

    std::string result;
    std::map<std::string,std::string> map1;
    map1["a"] = "1";
    map1["A"] = "2";
    map1["b"] = "3";
    map1["B"] = "4";
    std::string str("a A b B a A b B");
    sregex rx = icase(a1= map1) [ xp::ref(result) = a1 ] 
        >> repeat<3>( (' ' >> icase(a1= map1) [ xp::ref(result) += ',', xp::ref(result) += a1 ]) )
        >> repeat<4>( (' ' >>      (a1= map1) [ xp::ref(result) += ',', xp::ref(result) += a1 ]) );
    if(!regex_match(str, rx))
    {
        BOOST_ERROR("oops");
    }
    else
    {
        BOOST_CHECK_EQUAL(result, "1,1,3,3,1,2,3,4");
    }
}

///////////////////////////////////////////////////////////////////////////////
// test7
//  test multiple mutually-exclusive maps and default attribute value
void test7()
{
    using namespace boost::xpressive;

    std::string result;
    std::map<std::string,std::string> map1;
    map1["a"] = "1";
    map1["b"] = "2";
    std::map<std::string,std::string> map2;
    map2["c"] = "3";
    map2["d"] = "4";
    std::string str("abcde");
    sregex rx = *((a1= map1) | (a1= map2) | 'e') [ xp::ref(result) += (a1 | "9") ];
    if(!regex_match(str, rx))
    {
        BOOST_ERROR("oops");
    }
    else
    {
        BOOST_CHECK_EQUAL(result, "12349");
    }
}

#ifndef BOOST_XPRESSIVE_NO_WREGEX
struct City
{
    std::wstring name;
    char const* nickname;
    int population;
};

BOOST_TYPEOF_REGISTER_TYPE(City)

///////////////////////////////////////////////////////////////////////////////
// test8
//  test wide strings with structure result
void test8()
{
    using namespace boost::xpressive;

    City cities[] = {
        {L"Chicago", "The Windy City", 945000},
        {L"New York", "The Big Apple", 16626000},
        {L"\u041c\u043E\u0441\u043A\u0432\u0430", "Moscow", 9299000}
    };
    int const nbr_cities = sizeof(cities)/sizeof(*cities);

    std::map<std::wstring, City> map1;
    for(int i=0; i<nbr_cities; ++i)
    {
        map1[cities[i].name] = cities[i];
    }

    std::wstring str(L"Chicago \u041c\u043E\u0441\u043A\u0432\u0430");
    City result1, result2;
    wsregex rx = (a1= map1)[ xp::ref(result1) = a1 ] >> +_s
        >> (a1= map1)[ xp::ref(result2) = a1 ];
    if(!regex_match(str, rx))
    {
        BOOST_ERROR("oops");
    }
    else
    {
        BOOST_CHECK_EQUAL(result1.nickname, "The Windy City");
        BOOST_CHECK_EQUAL(result2.nickname, "Moscow");
    }
}
#else
void test8()
{
    // This test is empty
}
#endif

///////////////////////////////////////////////////////////////////////////////
// test9
//  test "not before" using a map
void test9()
{
    using namespace boost::xpressive;

    std::string result;
    std::string str("foobar");
    std::map<std::string,int> map1;
    map1["foo"] = 1;
    int xx = 0;
    sregex rx = ~before((a1=map1)[xp::ref(xx)=a1]) >> (s1=*_w)[ xp::ref(result) = s1 ];
    if(!regex_match(str, rx))
    {
        BOOST_CHECK_EQUAL(result, "");
    }
    else
    {
        BOOST_ERROR("oops");
    }
}

using namespace boost::unit_test;

///////////////////////////////////////////////////////////////////////////////
// init_unit_test_suite
//
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    test_suite *test = BOOST_TEST_SUITE("test_symbols");
    test->add(BOOST_TEST_CASE(&test6));
    test->add(BOOST_TEST_CASE(&test7));
    test->add(BOOST_TEST_CASE(&test8));
    test->add(BOOST_TEST_CASE(&test9));
    return test;
}

