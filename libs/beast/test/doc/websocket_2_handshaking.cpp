//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#include <boost/config.hpp>

#ifdef BOOST_MSVC
#pragma warning(push)
#pragma warning(disable: 4459) // declaration hides global declaration
#endif

#include <boost/beast/_experimental/unit_test/suite.hpp>

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace {

#include "websocket_common.ipp"

void
snippets()
{
    {
    //[code_websocket_2_1

        stream<tcp_stream> ws(ioc);
        net::ip::tcp::resolver resolver(ioc);
        get_lowest_layer(ws).connect(resolver.resolve("www.example.com", "ws"));

        // Do the websocket handshake in the client role, on the connected stream.
        // The implementation only uses the Host parameter to set the HTTP "Host" field,
        // it does not perform any DNS lookup. That must be done first, as shown above.

        ws.handshake(
            "www.example.com",  // The Host field
            "/"                 // The request-target
        );

    //]
    }

    {

    stream<tcp_stream> ws(ioc);

    {
    //[code_websocket_2_2

        // This variable will receive the HTTP response from the server
        response_type res;

        // Perform the websocket handshake in the client role.
        // On success, `res` will hold the complete HTTP response received.

        ws.handshake(
            res,                // Receives the HTTP response
            "www.example.com",  // The Host field
            "/"                 // The request-target
        );

    //]
    }

    //--------------------------------------------------------------------------

    {
    //[code_websocket_2_3

        // Perform the websocket handshake in the server role.
        // The stream must already be connected to the peer.

        ws.accept();

    //]
    }

    {
    //[code_websocket_2_4

        // This buffer will hold the HTTP request as raw characters
        std::string s;

        // Read into our buffer until we reach the end of the HTTP request.
        // No parsing takes place here, we are just accumulating data.

        net::read_until(sock, net::dynamic_buffer(s), "\r\n\r\n");

        // Now accept the connection, using the buffered data.
        ws.accept(net::buffer(s));

    //]
    }

    }

    {
    //[code_websocket_2_5

        // This buffer is required for reading HTTP messages
        flat_buffer buffer;

        // Read the HTTP request ourselves
        http::request<http::string_body> req;
        http::read(sock, buffer, req);

        // See if its a WebSocket upgrade request
        if(websocket::is_upgrade(req))
        {
            // Construct the stream, transferring ownership of the socket
            stream<tcp_stream> ws(std::move(sock));

            // Clients SHOULD NOT begin sending WebSocket
            // frames until the server has provided a response.
            BOOST_ASSERT(buffer.size() == 0);

            // Accept the upgrade request
            ws.accept(req);
        }
        else
        {
            // Its not a WebSocket upgrade, so
            // handle it like a normal HTTP request.
        }

    //]
    }

    {
        //[code_websocket_2_6

        // a function to select the most preferred protocol from a comma-separated list
        auto select_protocol = [](string_view offered_tokens) -> std::string
        {
            // tokenize the Sec-Websocket-Protocol header offered by the client
            http::token_list offered( offered_tokens );

            // an array of protocols supported by this server
            // in descending order of preference
            static const std::array<string_view, 3>
                supported = {{
                "v3.my.chat",
                "v2.my.chat",
                "v1.my.chat"
            }};

            std::string result;

            for (auto proto : supported)
            {
                auto iter = std::find(offered.begin(), offered.end(), proto);
                if (iter != offered.end())
                {
                    // we found a supported protocol in the list offered by the client
                    result.assign(proto.begin(), proto.end());
                    break;
                }
            }

            return result;
        };


        // This buffer is required for reading HTTP messages
        flat_buffer buffer;

        // Read the HTTP request ourselves
        http::request<http::string_body> req;
        http::read(sock, buffer, req);

        // See if it's a WebSocket upgrade request
        if(websocket::is_upgrade(req))
        {
            // we store the selected protocol in a std::string here because
            // we intend to capture it in the decorator's lambda below
            std::string protocol =
                select_protocol(
                    req[http::field::sec_websocket_protocol]);

            if (protocol.empty())
            {
                // none of our supported protocols were offered
                http::response<http::string_body> res;
                res.result(http::status::bad_request);
                res.body() = "No valid sub-protocol was offered."
                              " This server implements"
                              " v3.my.chat,"
                              " v2.my.chat"
                              " and v1.my.chat";
                http::write(sock, res);
            }
            else
            {
                // Construct the stream, transferring ownership of the socket
                stream<tcp_stream> ws(std::move(sock));

                ws.set_option(
                    stream_base::decorator(
                        [protocol](http::response_header<> &hdr) {
                            hdr.set(
                                http::field::sec_websocket_protocol,
                                protocol);
                        }));

                // Accept the upgrade request
                ws.accept(req);
            }
        }
        else
        {
            // Its not a WebSocket upgrade, so
            // handle it like a normal HTTP request.
        }

        //]
    }

}

struct websocket_2_test
    : public boost::beast::unit_test::suite
{
    void
    run() override
    {
        BEAST_EXPECT(&snippets);
    }
};

BEAST_DEFINE_TESTSUITE(beast,doc,websocket_2);

} // (anon)

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif
