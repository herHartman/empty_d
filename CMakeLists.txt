cmake_minimum_required(VERSION 3.27)
project(empty_d)

set(CMAKE_CXX_STANDARD 23)

add_executable(empty_d main.cpp
        http/protocol/response/http_response.h
        http/web/web_application.h
        http/web/router/resource_route.h
        http/web/router/uri_dispatcher.cpp
        http/web/router/resource_route.cpp
        http/handlers/handlers.cpp
        http/web/web_application.cpp
        http/web/exceptions/http_exceptions.h
        http/web/resources/plain_resource.cpp
        http/web/http_protocol.cpp
        http/handlers/http_request_handler.cpp
        http/handlers/http_request_handler.h
        core/boost_asio_types.h
        http/protocol/response/http_response.cpp
        http/protocol/response/response_writer.cpp
        http/protocol/response/response_writer.h
        network/tcp_server.cpp
        http/web/http_statuse.h
        http/web/http_headers.h
        http/web/http_headers.cpp
        http/protocol/http_writer.cpp
        http/protocol/http_writer.h
)
