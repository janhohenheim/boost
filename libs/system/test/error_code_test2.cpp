// Copyright 2020, 2021 Peter Dimov.
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt

#include <boost/system/error_code.hpp>
#include <boost/system/generic_category.hpp>
#include <boost/system/system_category.hpp>
#include <boost/core/lightweight_test.hpp>

namespace sys = boost::system;

int main()
{
    char buffer[ 4096 ], buffer2[ 4096 ];

    {
        sys::error_code ec;

        BOOST_TEST_EQ( ec.value(), 0 );
        BOOST_TEST( ec.category() == sys::system_category() );

        BOOST_TEST_EQ( ec.message(), ec.category().message( ec.value() ) );
        BOOST_TEST_CSTR_EQ( ec.message( buffer, sizeof( buffer ) ), ec.category().message( ec.value(), buffer2, sizeof( buffer2 ) ) );
        BOOST_TEST_CSTR_EQ( ec.message( buffer, sizeof( buffer ) ), ec.message().c_str() );

        {
            char const* msg = ec.message( nullptr, 0 );

            if( msg != nullptr )
            {
                BOOST_TEST_CSTR_EQ( msg, ec.message().c_str() );
            }
        }

        BOOST_TEST( !ec.failed() );
        BOOST_TEST( !ec );

        BOOST_TEST_EQ( ec.to_string(), std::string( "system:0" ) );
    }

    {
        sys::error_code ec;
        sys::error_code ec2( ec );

        BOOST_TEST_EQ( ec2.value(), 0 );
        BOOST_TEST( ec2.category() == sys::system_category() );

        BOOST_TEST_EQ( ec2.message(), ec2.category().message( ec2.value() ) );
        BOOST_TEST_CSTR_EQ( ec2.message( buffer, sizeof( buffer ) ), ec2.category().message( ec2.value(), buffer2, sizeof( buffer2 ) ) );
        BOOST_TEST_CSTR_EQ( ec2.message( buffer, sizeof( buffer ) ), ec2.message().c_str() );

        {
            char const* msg = ec2.message( nullptr, 0 );

            if( msg != nullptr )
            {
                BOOST_TEST_CSTR_EQ( msg, ec2.message().c_str() );
            }
        }

        BOOST_TEST( !ec2.failed() );
        BOOST_TEST( !ec2 );

        BOOST_TEST_EQ( ec, ec2 );
        BOOST_TEST_NOT( ec != ec2 );

        BOOST_TEST_EQ( ec2.to_string(), std::string( "system:0" ) );
    }

    {
        sys::error_code ec;
        sys::error_code ec2( ec.value(), ec.category() );

        BOOST_TEST_EQ( ec2.value(), 0 );
        BOOST_TEST( ec2.category() == sys::system_category() );

        BOOST_TEST_EQ( ec2.message(), ec2.category().message( ec2.value() ) );
        BOOST_TEST_CSTR_EQ( ec2.message( buffer, sizeof( buffer ) ), ec2.category().message( ec2.value(), buffer2, sizeof( buffer2 ) ) );
        BOOST_TEST_CSTR_EQ( ec2.message( buffer, sizeof( buffer ) ), ec2.message().c_str() );

        {
            char const* msg = ec2.message( nullptr, 0 );

            if( msg != nullptr )
            {
                BOOST_TEST_CSTR_EQ( msg, ec2.message().c_str() );
            }
        }

        BOOST_TEST( !ec2.failed() );
        BOOST_TEST( !ec2 );

        BOOST_TEST_EQ( ec, ec2 );
        BOOST_TEST_NOT( ec != ec2 );

        BOOST_TEST_EQ( ec2.to_string(), std::string( "system:0" ) );
    }

    {
        sys::error_code ec2( 5, sys::generic_category() );

        BOOST_TEST_EQ( ec2.value(), 5 );
        BOOST_TEST( ec2.category() == sys::generic_category() );

        BOOST_TEST_EQ( ec2.message(), ec2.category().message( ec2.value() ) );
        BOOST_TEST_CSTR_EQ( ec2.message( buffer, sizeof( buffer ) ), ec2.category().message( ec2.value(), buffer2, sizeof( buffer2 ) ) );
        BOOST_TEST_CSTR_EQ( ec2.message( buffer, sizeof( buffer ) ), ec2.message().c_str() );

        {
            char const* msg = ec2.message( nullptr, 0 );

            if( msg != nullptr )
            {
                BOOST_TEST_CSTR_EQ( msg, ec2.message().c_str() );
            }
        }

        BOOST_TEST( ec2.failed() );
        BOOST_TEST( ec2 );
        BOOST_TEST_NOT( !ec2 );

        sys::error_code ec;

        BOOST_TEST_NE( ec, ec2 );
        BOOST_TEST_NOT( ec == ec2 );

        BOOST_TEST_EQ( ec2.to_string(), std::string( "generic:5" ) );
    }

    {
        sys::error_code ec2( -4, sys::generic_category() );

        BOOST_TEST_EQ( ec2.value(), -4 );
        BOOST_TEST( ec2.category() == sys::generic_category() );

        BOOST_TEST_EQ( ec2.message(), ec2.category().message( ec2.value() ) );
        BOOST_TEST_CSTR_EQ( ec2.message( buffer, sizeof( buffer ) ), ec2.category().message( ec2.value(), buffer2, sizeof( buffer2 ) ) );
        BOOST_TEST_CSTR_EQ( ec2.message( buffer, sizeof( buffer ) ), ec2.message().c_str() );

        {
            char const* msg = ec2.message( nullptr, 0 );

            if( msg != nullptr )
            {
                BOOST_TEST_CSTR_EQ( msg, ec2.message().c_str() );
            }
        }

        BOOST_TEST( ec2.failed() );
        BOOST_TEST( ec2 );
        BOOST_TEST_NOT( !ec2 );

        sys::error_code ec;

        BOOST_TEST_NE( ec, ec2 );
        BOOST_TEST_NOT( ec == ec2 );

        BOOST_TEST_EQ( ec2.to_string(), std::string( "generic:-4" ) );
    }

    {
        sys::error_code ec2( 5, sys::system_category() );

        BOOST_TEST_EQ( ec2.value(), 5 );
        BOOST_TEST( ec2.category() == sys::system_category() );

        BOOST_TEST_EQ( ec2.message(), ec2.category().message( ec2.value() ) );
        BOOST_TEST_CSTR_EQ( ec2.message( buffer, sizeof( buffer ) ), ec2.category().message( ec2.value(), buffer2, sizeof( buffer2 ) ) );
        BOOST_TEST_CSTR_EQ( ec2.message( buffer, sizeof( buffer ) ), ec2.message().c_str() );

        {
            char const* msg = ec2.message( nullptr, 0 );

            if( msg != nullptr )
            {
                BOOST_TEST_CSTR_EQ( msg, ec2.message().c_str() );
            }
        }

        BOOST_TEST( ec2.failed() );
        BOOST_TEST( ec2 );
        BOOST_TEST_NOT( !ec2 );

        sys::error_code ec;

        BOOST_TEST_NE( ec, ec2 );
        BOOST_TEST_NOT( ec == ec2 );

        BOOST_TEST_EQ( ec2.to_string(), std::string( "system:5" ) );
    }

    {
        sys::error_code ec2( -4, sys::system_category() );

        BOOST_TEST_EQ( ec2.value(), -4 );
        BOOST_TEST( ec2.category() == sys::system_category() );

        BOOST_TEST_EQ( ec2.message(), ec2.category().message( ec2.value() ) );
        BOOST_TEST_CSTR_EQ( ec2.message( buffer, sizeof( buffer ) ), ec2.category().message( ec2.value(), buffer2, sizeof( buffer2 ) ) );
        BOOST_TEST_CSTR_EQ( ec2.message( buffer, sizeof( buffer ) ), ec2.message().c_str() );

        {
            char const* msg = ec2.message( nullptr, 0 );

            if( msg != nullptr )
            {
                BOOST_TEST_CSTR_EQ( msg, ec2.message().c_str() );
            }
        }

        BOOST_TEST( ec2.failed() );
        BOOST_TEST( ec2 );
        BOOST_TEST_NOT( !ec2 );

        sys::error_code ec;

        BOOST_TEST_NE( ec, ec2 );
        BOOST_TEST_NOT( ec == ec2 );

        BOOST_TEST_EQ( ec2.to_string(), std::string( "system:-4" ) );
    }

    return boost::report_errors();
}
