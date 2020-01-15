#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/asio/basic_stream_socket.hpp>

#include "Client.h"

Client::Client::Client() :
  m_isStopped(false),
  m_socket(m_service),
  m_connected(false)
{
}

Client::~Client()
{
  stop();
}

bool Client::isConnected()
{
    return m_connected;
}

void Client::sendMessage(const std::vector<std::uint8_t>& message, bool front)
{
  boost::recursive_mutex::scoped_lock lock(m_cmdGuard);

  if (front)
  {
    m_commands.push_front(message);
  }
  else
  {
    m_commands.push_back(message);
  }
}

void Client::cycle(const std::string& serverIP, const std::string& serverPort)
{
  boost::system::error_code error;  
  boost::asio::ip::tcp::resolver::query query(serverIP, serverPort);
  boost::asio::ip::tcp::resolver resolver(m_service);
  boost::asio::ip::tcp::resolver::iterator destination = resolver.resolve(query, error);
  boost::asio::ip::tcp::endpoint endpoint;

  while (destination != boost::asio::ip::tcp::resolver::iterator())
  {
    endpoint = *destination++;
    std::cout<<endpoint<<std::endl;

    break;
  }

  bool waitRead = false;
  bool waitWrite = true;

  bool connectFail = false;

  while (!m_isStopped)
  {
    if (!m_connected)
    {
      m_socket.connect(endpoint, error);
      if (error)
      {
        std::cout << "Fatal error: can not connect to server." << std::endl;
        if (!connectFail)
        {
          connectFail = true;
        }

        boost::this_thread::sleep(boost::posix_time::milliseconds(500));
        continue;
      }

      m_connected = true;
      m_socket.non_blocking(true);
    }

    if (!waitRead && m_commands.size())
    {
      boost::recursive_mutex::scoped_lock lock(m_cmdGuard);
      std::vector<unsigned char> cmd = m_commands.front();
      
      m_socket.write_some(boost::asio::buffer(cmd), error);

      if (error)
      {
        m_connected = false;
        waitRead = false;
        waitWrite = true;

        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
        m_socket.cancel(error);
        m_socket.close(error);
        
        continue;
      }

      waitWrite = false;
    }

    if (!waitWrite)
    {
      waitRead = true;
      waitWrite = false;

      std::vector<std::uint8_t> inBuffer;

      std::size_t bytes_readable = 0;
      boost::asio::ip::tcp::socket::bytes_readable command;
      m_socket.io_control(command, error);

      bytes_readable = error ? 4096 : command.get();

      if (bytes_readable == 0)
      {
        continue;
      }

      inBuffer.resize(bytes_readable);

      size_t size = m_socket.read_some(boost::asio::buffer(&inBuffer[0], inBuffer.size()), error);

      if (error)
      {
        m_connected = false;
        waitRead = false;
        waitWrite = true;

        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
        m_socket.cancel(error);
        m_socket.close(error);

        continue;
      }

      inBuffer.resize(size);
      {
        boost::recursive_mutex::scoped_lock lock(m_rspGuard);
        m_responces.push_back(inBuffer);
      }

      {
        boost::recursive_mutex::scoped_lock lock(m_cmdGuard);
        m_commands.pop_front();
      }

      waitRead = false;
      waitWrite = true;
    }
  }
}

std::vector<std::uint8_t> Client::getResponce()
{
    if (!m_responces.size())
    {
        return std::vector<std::uint8_t>();
    }
    
    {
      boost::recursive_mutex::scoped_lock lock(m_rspGuard);
      std::vector<std::uint8_t> result = m_responces.front();
      m_responces.pop_front();
      
      return result;
    }
}

bool Client::hasResponce()
{
    {
        boost::recursive_mutex::scoped_lock lock(m_rspGuard);
        
        return m_responces.size();
    }
}

bool Client::start(const std::string& serverIP, const std::string& serverPort)
{
  if (serverIP.empty() || serverPort.empty())
  {
    return false;
  }

  m_thread = boost::thread(boost::bind(&Client::cycle, this, serverIP, serverPort));

  return true;
}

void Client::stop()
{
  if (m_isStopped)
  {
    return;
  }

  boost::this_thread::sleep(boost::posix_time::milliseconds(10));

  try
  {
    m_isStopped = true;

    boost::system::error_code error;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
    m_socket.cancel(error);
    m_socket.close(error);

    if (m_thread.joinable())
    {
        m_thread.join();
    }

    m_isStopped = false;
  }
  catch (const boost::thread_interrupted&)
  {
  }
}

void Client::runIO()
{
  m_service.run();
}
