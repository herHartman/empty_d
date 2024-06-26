//
// Created by chopk on 12.04.24.
//

#ifndef EMPTY_D_HTTP_WRITER_H
#define EMPTY_D_HTTP_WRITER_H

#include "../../network/transport.h"
#include "../web/http_headers.h"
#include <memory>
#include <utility>
#include "../../core/boost_asio_types.h"

namespace http {
    class http_writer {

    public:
        explicit http_writer(network::transport::transport_p transport) : transport_(std::move(transport)) {}
        awaitable<void> write_headers(const http::web::http_headers& headers);
    private:
        network::transport::transport_p transport_;
    };
}
#endif //EMPTY_D_HTTP_WRITER_H
