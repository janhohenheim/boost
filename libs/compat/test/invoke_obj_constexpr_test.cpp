// Copyright 2024 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/compat/invoke.hpp>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)
#define BOOST_TEST_EQ(x, y) STATIC_ASSERT((x) == (y))

struct F
{
    constexpr int operator()() const
    {
        return -1;
    }

    constexpr int operator()( int x1 ) const noexcept
    {
        return x1;
    }

    constexpr int operator()( int x1, int x2 ) const
    {
        return 10*x1+x2;
    }

    constexpr int operator()( int x1, int x2, int x3 ) const noexcept
    {
        return 100*x1 + 10*x2 + x3;
    }
};

int main()
{
    {
        BOOST_TEST_EQ( boost::compat::invoke( F() ), -1 );
        BOOST_TEST_EQ( boost::compat::invoke( F(), 1 ), 1 );
        BOOST_TEST_EQ( boost::compat::invoke( F(), 1, 2 ), 12 );
        BOOST_TEST_EQ( boost::compat::invoke( F(), 1, 2, 3 ), 123 );
    }

    {
        constexpr F f = {};

        BOOST_TEST_EQ( boost::compat::invoke( f ), -1 );
        BOOST_TEST_EQ( boost::compat::invoke( f, 1 ), 1 );
        BOOST_TEST_EQ( boost::compat::invoke( f, 1, 2 ), 12 );
        BOOST_TEST_EQ( boost::compat::invoke( f, 1, 2, 3 ), 123 );
    }
}
