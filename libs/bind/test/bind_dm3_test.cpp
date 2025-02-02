#include <boost/config.hpp>

#if defined(BOOST_MSVC)
#pragma warning(disable: 4786)  // identifier truncated in debug info
#pragma warning(disable: 4710)  // function not inlined
#pragma warning(disable: 4711)  // function selected for automatic inline expansion
#pragma warning(disable: 4514)  // unreferenced inline removed
#endif

//
//  bind_dm3_test.cpp - data members (regression 1.31 - 1.32)
//
//  Copyright (c) 2005 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#if defined(BOOST_GCC) && BOOST_GCC >= 130000 && BOOST_GCC < 150000
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=113256
# pragma GCC diagnostic ignored "-Wdangling-reference"
#endif

#include <boost/bind/bind.hpp>
#include <boost/core/lightweight_test.hpp>
#include <utility>

using namespace boost::placeholders;

//

int main()
{
    typedef std::pair<int, int> pair_type;

    pair_type pair( 10, 20 );

    int const & x = boost::bind( &pair_type::first, _1 )( pair );

    BOOST_TEST( &pair.first == &x );

    return boost::report_errors();
}
