/* Example of Outcome
(C) 2017-2024 Niall Douglas <http://www.nedproductions.biz/> (3 commits)


Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include <boost/asio/async_result.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/outcome.hpp>

#include <vector>

namespace asio = boost::asio;
namespace outcome = boost::outcome_v2;

using byte = char;
using boost::system::error_code;

void old(asio::ip::tcp::socket skt)
{
  //! [old-use-case]
  // Dynamically allocate a buffer to read into. This must be move-only
  // so it can be attached to the completion handler, hence the unique_ptr.
  auto buffer = std::make_unique<std::vector<byte>>(1024);

  // Begin an asynchronous socket read, upon completion invoke
  // the lambda function specified
  skt.async_read_some(asio::buffer(buffer->data(), buffer->size()),

                      // Retain lifetime of the i/o buffer until completion
                      [buffer = std::move(buffer)](const error_code &ec, size_t bytes) {
                        // Handle the buffer read
                        if(ec)
                        {
                          std::cerr << "Buffer read failed with " << ec << std::endl;
                          return;
                        }
                        std::cout << "Read " << bytes << " bytes into buffer" << std::endl;

                        // buffer will be dynamically freed now
                      });
  //! [old-use-case]
}

asio::awaitable<void> new_(asio::ip::tcp::socket skt)
{
  //! [new-use-case]
  // As coroutines suspend the calling thread whilst an asynchronous
  // operation executes, we can use stack allocation instead of dynamic
  // allocation
  char buffer[1024];

  // Asynchronously read data, suspending this coroutine until completion,
  // returning the bytes of the data read into the result.
  try
  {
    // The use_awaitable completion token represents the current coroutine
    // (requires Coroutines TS)
    size_t bytesread =  //
    co_await skt.async_read_some(asio::buffer(buffer), asio::use_awaitable);
    std::cout << "Read " << bytesread << " bytes into buffer" << std::endl;
  }
  catch(const std::system_error &e)
  {
    std::cerr << "Buffer read failed with " << e.what() << std::endl;
  }
  //! [new-use-case]
}

//! [as_result]
namespace detail
{
  // Type sugar for wrapping an external completion token
  template <class CompletionToken> struct as_result_t
  {
    CompletionToken token;
  };
}  // namespace detail

// Factory function for wrapping a third party completion token with
// our type sugar
template <class CompletionToken>  //
inline auto as_result(CompletionToken &&token)
{
  return detail::as_result_t<std::decay_t<CompletionToken>>{std::forward<CompletionToken>(token)};
};
//! [as_result]

//! [async_result1]
// Tell ASIO about a new kind of completion token, the kind returned
// from our as_result() factory function. This implementation is
// for functions with handlers void(error_code, T) only.
template <class CompletionToken, class T>                        //
struct asio::async_result<detail::as_result_t<CompletionToken>,  //
                          void(error_code, T)>                   //

{
  // The result type we shall return
  using result_type = outcome::result<T, error_code>;
  // The awaitable type to be returned by the initiating function,
  // the co_await of which will yield a result_type
  using return_type = //
  typename asio::async_result<CompletionToken, void(result_type)> //
  ::return_type;

  //! [async_result1]
  //! [async_result2]
  // Wrap a completion handler with void(error_code, T) converting
  // handler
  template <class Handler>
  struct completion_handler {
    // Our completion handler spec
    void operator()(error_code ec, T v)
    {
      // Call the underlying completion handler, whose
      // completion function is void(result_type)
      if(ec)
      {
        // Complete with a failed result
        _handler(result_type(outcome::failure(ec)));
        return;
      }
      // Complete with a successful result
      _handler(result_type(outcome::success(v)));
    }

    Handler _handler;
  };

  // NOTE the initiate member function initiates the async operation,
  // and we want to defer to what would be the initiation of the
  // async_result whose handler signature is void(result_type).
  template <class Initiation, class... Args>
  static return_type
  initiate(
    Initiation&& init,
    detail::as_result_t<CompletionToken>&& token,
    Args&&... args)
  {
    // The async_initiate<CompletionToken, void(result_type)> helper
    // function will invoke the async initiation method of the
    // async_result<CompletionToken, void(result_type)>, as desired.
    // Instead of CompletionToken and void(result_type)	we start with
    // detail::as_result_t<CompletionToken> and void(ec, T), so
    // the inputs need to be massaged then passed along.
    return asio::async_initiate<CompletionToken, void(result_type)>(
      // create a new initiation which wraps the provided init
      [init = std::forward<Initiation>(init)](
        auto&& handler, auto&&... initArgs) mutable {
        std::move(init)(
          // we wrap the handler in the converting completion_handler from
          // above, and pass along the args
          completion_handler<std::decay_t<decltype(handler)>>{
            std::forward<decltype(handler)>(handler)},
          std::forward<decltype(initArgs)>(initArgs)...);
      },
      // the new initiation is called with the handler unwrapped from
      // the token, and the original initiation arguments.
      token.token,
      std::forward<Args>(args)...);
  }
};
//! [async_result2]

asio::awaitable<void> outcome_(asio::ip::tcp::socket skt)
{
  char buffer[1024];

#if 0
  {  // debug
    using my_result_t = decltype(as_result(token));
    asio::async_result<my_result_t, void(error_code, size_t)>::completion_handler_type handler(as_result(token));
    asio::async_result<my_result_t, void(error_code, size_t)> result(handler);
    error_code ec;
    handler(ec, 5);
    outcome::result<size_t, error_code> r = co_await result.get();
  }
#endif

#if 1
  //! [outcome-use-case]
  // Asynchronously read data, suspending this coroutine until completion,
  // returning the bytes of the data read into the result, or any failure.
  outcome::result<size_t, error_code> bytesread =  //
  co_await skt.async_read_some(asio::buffer(buffer), as_result(asio::use_awaitable));

  // Usage is exactly like ordinary Outcome. Note the lack of exception throw!
  if(bytesread.has_error())
  {
    std::cerr << "Buffer read failed with " << bytesread.error() << std::endl;
    return;
  }
  std::cout << "Read " << bytesread.value() << " bytes into buffer" << std::endl;
  //! [outcome-use-case]
#endif
}

int main(void)
{
  return 0;
}
