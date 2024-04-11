//
// Created by chopk on 31.03.24.
//

#ifndef SIMPLE_HTTP_SERVER_HTTP_METHODS_H
#define SIMPLE_HTTP_SERVER_HTTP_METHODS_H

namespace http {
    enum http_methods {
        HTTP_OPTIONS,
        HTTP_GET,
        HTTP_HEAD,
        HTTP_POST,
        HTTP_PUT,
        HTTP_PATCH,
        HTTP_DELETE,
        HTTP_TRACE,
        HTTP_CONNECT
    };
}

#endif //SIMPLE_HTTP_SERVER_HTTP_METHODS_H
