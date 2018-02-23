#pragma once

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

class Server : private boost::noncopyable
{
  boost::thread m_thread;
  volatile bool m_isStopped;

  boost::asio::io_service m_service;
  boost::asio::ip::tcp::socket m_socket;
  boost::asio::ip::tcp::acceptor m_acceptor;

  void cycle(const short int port);

public:

  Server();
  ~Server();

  void start(const short int port);
  void stop();
};
