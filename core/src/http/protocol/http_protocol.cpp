//
// Created by chopk on 06.04.24.
//

#include "http_protocol.h"

  
awaitable<void> base_protocol::handle() {
  try {
    while (transport_->is_open()) {
      std::size_t data_len =
          co_await transport_->read(boost::asio::buffer(buffer_));
      if (!request_parser_.is_parse_message_complete()) {
        std::size_t last_message_index = request_parser_.parse_message(
            buffer_.data(), data_len, raw_request_message_);
        if (request_parser_.is_parse_message_complete()) {

          auto stream_reader = std::make_shared<http::http_body_stream_reader>(
              std::make_shared<
                  http::http_body_stream_reader::read_lock_channel>(
                  transport_->get_executor(), 1));
          http::http_request request(stream_reader, raw_request_message_);
          request_parser_.set_payload(stream_reader);
          co_spawn(transport_->get_executor(), handle_request(request),
                   detached);
          if (last_message_index < data_len) {
            co_await request_parser_.parse_body(
                &buffer_[last_message_index - 1],
                data_len - last_message_index + 1, raw_request_message_);
          }
        }
      } else {
        co_await request_parser_.parse_body(buffer_.data(), data_len,
                                            raw_request_message_);
      }
    }
  } catch (std::exception &e) {
    std::printf("echo Exception: %s\n", e.what());
  }
}
