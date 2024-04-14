
#ifndef EMPTY_HTTP_SERVER_TRANSPORT_H
#define EMPTY_HTTP_SERVER_TRANSPORT_H

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/experimental/channel.hpp>
#include <memory>
#include <utility>

using boost::asio::awaitable;
using boost::asio::ip::tcp;
using boost::asio::experimental::channel;
using boost::asio::deferred;


namespace network {
    class transport {
    public:
        typedef std::shared_ptr<transport> transport_p;

        explicit transport(tcp::socket socket)
            : socket_(std::move(socket)) {}

        template<typename MutableBufferSequence>
        awaitable<std::size_t> read(const MutableBufferSequence& buffers) {
            std::size_t len = co_await socket_.async_read_some(buffers, boost::asio::use_awaitable);
            co_return len;
        }

        template<typename ConstBufferSequence>
        awaitable<void> write(const ConstBufferSequence& buffers) {
            co_await socket_.async_write_some(buffers, boost::asio::use_awaitable);
        }

        [[nodiscard]] auto get_executor() {
            return socket_.get_executor();
        }

        [[nodiscard]] bool is_open() const {
            socket_.is_open();
        }

        void close() {
            socket_.shutdown(tcp::socket::shutdown_send);
            socket_.close();
        }

    private:
        tcp::socket socket_;
    };

} // namespace network

#endif // EMPTY_HTTP_SERVER_TRANSPORT_H
