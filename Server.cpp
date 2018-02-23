#include <boost/bind.hpp>

#include "Server.h"

Server::Server() :
  m_isStopped(false),
  m_socket(m_service),
  m_acceptor(m_service)
{
}

Server::~Server()
{
  stop();
}

void Server::start(const short int port)
{
  m_thread = boost::thread(boost::bind(&Server::cycle, this, port));
}

void Server::stop()
{
  if (m_isStopped)
  {
    return;
  }

  try
  {
    m_isStopped = true;

    boost::system::error_code error;

    m_acceptor.cancel(error);
    m_acceptor.close(error);

    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
    m_socket.cancel(error);
    m_socket.close(error);

    m_thread.join();

    m_isStopped = false;
  }
  catch (const boost::thread_interrupted&)
  {
  }
}

void Server::cycle(const short int port)
{
  boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), port);

  bool accept = false;
  std::string response;

  while (!m_isStopped)
  {
    if (!accept)
    {
      boost::system::error_code error;

      m_acceptor.open(ep.protocol());
      m_acceptor.bind(ep, error);

      if (error)
      {
        continue;
      }

      m_acceptor.listen(1, error);

      if (error)
      {
        continue;
      }

      m_acceptor.accept(m_socket, ep, error);

      if (error)
      {
        continue;
      }

      accept = true;

      boost::asio::socket_base::non_blocking_io command1(true);
      m_socket.io_control(command1, error);
    }

    boost::system::error_code error;
    std::string inBuffer;

    std::size_t bytes_readable = 0;
    boost::asio::ip::tcp::socket::bytes_readable command2;
    m_socket.io_control(command2, error);

    bytes_readable = error ? 4096 : command2.get();

    if (!bytes_readable)
    {
      continue;
    }

    inBuffer.resize(bytes_readable);

    size_t size = m_socket.read_some(boost::asio::buffer(&inBuffer[0], inBuffer.size()), error);

    if (error)
    {
      m_acceptor.cancel(error);
      m_acceptor.close(error);

      accept = false;
      continue;
    }

    inBuffer.resize(size);

    response.append(inBuffer);
  }
}
