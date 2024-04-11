//
// Created by chopk on 05.04.24.
//

#ifndef SIMPLE_HTTP_SERVER_HTTP_REQUEST_HANDLER_H
#define SIMPLE_HTTP_SERVER_HTTP_REQUEST_HANDLER_H

#include "raw_request_message.h"

namespace http {
    class http_request_handler {
    public:
        void handle_request(const raw_request_message& raw_request);
    };
};



#endif //SIMPLE_HTTP_SERVER_HTTP_REQUEST_HANDLER_H
