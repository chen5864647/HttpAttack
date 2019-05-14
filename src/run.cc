#include <boost/asio.hpp>
#include <array>
#include <string>
#include <iostream>

#include "threadpool.hpp"

std::string url;
std::string http;
std::array<char, 9192> buffer;
char datar[39];

void read_handler(const boost::system::error_code &ec, 
                    size_t bytes_transferred, 
                    boost::asio::ip::tcp::socket &sock) {
    if (!ec) {
        std::cout << std::string(buffer.data(), bytes_transferred) << std::endl;
        sock.async_read_some(boost::asio::buffer(buffer), read_handler);
    }
    else 
        std::cerr << "There is something wrong with the read" << std::endl;
}

void connect_handler(const boost::system::error_code &ec, 
                    boost::asio::ip::tcp::socket &sock) {
    if (!ec) {
        
        boost::asio::write(sock, boost::asio::buffer(datar));
        auto read_handler_bind = std::bind(read_handler, std::placeholders::_1, std::placeholders::_2, sock);
        sock.async_read_some(boost::asio::buffer(buffer), read_handler_bind);
    }
    else
        std::cerr << "There is something wrong with the connect" << std::endl;
}

void resolve_handler(const boost::system::error_code &ec, 
                    boost::asio::ip::tcp::resolver::iterator it, 
                    boost::asio::ip::tcp::socket &sock) {
    if (!ec) {
        auto connect_handler_bind = std::bind(connect_handler, std::placeholders::_1, sock);
        sock.async_connect(*it, connect_handler_bind);
    }
}

inline
void funnc(boost::asio::io_service &io_service,
            boost::asio::ip::tcp::resolver &resolver,
            boost::asio::ip::tcp::socket &sock,
            boost::asio::ip::tcp::resolver::query &query) {
    auto resolver_handler_bind = std::bind(resolve_handler, std::placeholders::_1, std::placeholders::_2, sock);
    resolver.async_resolve(query, resolver_handler_bind);
}

void func() {

    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::socket sock(io_service);
    boost::asio::deadline_timer timer(io_service, boost::posix_time::seconds(1));

    boost::asio::ip::tcp::resolver::query query(url.c_str(), "80"); 
    
    timer.async_wait(funnc);
    io_service.run();

}

void work() {
    ThreadPool pool(1000);

    while (true)
        pool.enqueue(func);

}