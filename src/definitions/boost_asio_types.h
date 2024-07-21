//
// Created by chopk on 11.04.24.
//

#ifndef TEST_BOOST_ASIO_TYPES_H
#define TEST_BOOST_ASIO_TYPES_H

#include <boost/asio.hpp>
#include <boost/asio/experimental/as_tuple.hpp>

using namespace boost;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;
namespace this_coro = boost::asio::this_coro;

inline constexpr auto no_ex_coro = asio::experimental::as_tuple(asio::use_awaitable);

#endif //TEST_BOOST_ASIO_TYPES_H
