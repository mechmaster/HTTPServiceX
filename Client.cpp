#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include "Client.h"

Client::Client::Client() :
  m_isStopped(false),
  m_socket(m_service)
{
  m_processCollectBody = false;
}

Client::~Client()
{
  stop();
}

void Client::sendMessage(const std::string& message, bool front)
{
  boost::recursive_mutex::scoped_lock lock(m_cmdGuard);

  m_processCollectBody = false;

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

  bool connected = false;
  bool waitRead = false;
  bool waitWrite = true;

  bool connectFail = false;

  std::string response;

  while (!m_isStopped)
  {
    if (!connected)
    {
      m_socket.connect(endpoint, error);
      if (error)
      {
        std::cout << "Connect fail!" << std::endl;
        if (!connectFail)
        {
          connectFail = true;
        }

        boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
        continue;
      }

      boost::asio::socket_base::non_blocking_io command(true);
      m_socket.io_control(command, error);

      connected = true;
    }

    if (!waitRead && m_commands.size())
    {
      boost::recursive_mutex::scoped_lock lock(m_cmdGuard);
      std::string cmd = m_commands.front();

      m_socket.write_some(boost::asio::buffer(cmd), error);

      if (error)
      {
        connected = false;
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

      std::string inBuffer;

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
        connected = false;
        waitRead = false;
        waitWrite = true;

        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
        m_socket.cancel(error);
        m_socket.close(error);

        continue;
      }

      inBuffer.resize(size);
      response.append(inBuffer);

      if (!m_processCollectBody)
      {
        std::string header;
        std::size_t pos = response.find("\r\n\r\n");
        if (pos == std::string::npos)
        {
          continue;
        }
        else
        {
          header = response.substr(0, pos);
          response.erase(0, header.size() + 4);
        }

        std::deque<std::string> stringsToParse;

        typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
        boost::char_separator<char> sep{"\r\n", "", boost::keep_empty_tokens};
        tokenizer tokenParser{header, sep};
        for (const auto &t : tokenParser)
        {
          if (!t.empty())
          {
            stringsToParse.push_back(t);
          }
        }
        
        std::string cmdAnswer = stringsToParse.front();

        //-----------------------------------------------------------------------
        std::string http_version;
        unsigned int status_code;
        std::string status_message;

        std::vector<std::string> result;
        boost::algorithm::split(result, cmdAnswer, boost::algorithm::is_any_of(" "));
        if (result.size() == 3)
        {
          http_version = result[0];
          status_message = result[2];

          try
          {
            status_code = boost::lexical_cast<unsigned int>(result[1]);
          }
          catch (const boost::bad_lexical_cast&)
          {
            status_code = 0;
          }

          stringsToParse.pop_front();
        }
        //-----------------------------------------------------------------------
        
        std::string currentHeaderString;
        while (stringsToParse.size())
        {
          currentHeaderString = stringsToParse.front();
          
          std::string key;
          std::string value;
          
          std::string::size_type pos =  currentHeaderString.find(":");
          if (pos != std::string::npos)
          {
            key = currentHeaderString.substr(0, pos);
            value = currentHeaderString.substr(pos + 2, currentHeaderString.size());

            m_headerList.insert(std::make_pair(key, value));
            std::cout << "key = " << key << "; value = " << value << std::endl;
            stringsToParse.pop_front();
          }
        }

        m_processCollectBody = true;
      }

      std::map<std::string, std::string>::iterator iter = m_headerList.find("Content-Length");
      if (iter != m_headerList.end())
      {
        unsigned int bodySize = 0;
        try
        {
          bodySize = boost::lexical_cast<unsigned int>(iter->second);
        }
        catch (const boost::bad_lexical_cast&)
        {
          bodySize = 0;
        }
        
        if (response.size() < bodySize)
        {
          continue;
        }
        
        m_processCollectBody = false;

        // emit cmdAnswerReady;

        std::cout << "Response = " << response << std::endl;
      }
      else
      {
        std::map<std::string, std::string>::iterator iterType = m_headerList.find("Content-Type");
        if (iterType != m_headerList.end())
        {
          std::string type = iterType->second;
          std::size_t pos = type.find(";");
          if (pos != std::string::npos)
          {
            type = type.substr(0, pos);
          }

          static std::string beginTag;
          static std::string endTag;

          std::string doc;
          if (type == "text/html")
          {
            doc = "html";
            beginTag = boost::str(boost::format("<%s") %doc);
            endTag = boost::str(boost::format("</%s>") %doc);
          }
          else if (type == "text/xml")
          {
            doc = "xml";
            beginTag = boost::str(boost::format("<%s") %doc);
            endTag = boost::str(boost::format("</%s>") %doc);          
          }

          std::size_t beginPos = response.find(beginTag);
          if (beginPos == std::string::npos)
          {
            boost::algorithm::to_upper(beginTag);
            beginPos = response.find(beginTag);
          }
          
          std::size_t endPos = response.rfind(endTag);
          if (endPos == std::string::npos)
          {
            boost::algorithm::to_upper(endTag);
            endPos = response.find(endTag);
          }

          if (beginPos == std::string::npos || endPos == std::string::npos)
          {
            continue;
          }

          m_processCollectBody = false;

          // emit cmdAnswerReady;
          
          std::cout << "Response = " << response << std::endl;
        }
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

  try
  {
    m_isStopped = true;

    boost::system::error_code error;
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

void Client::runIO()
{
  m_service.run();
}
