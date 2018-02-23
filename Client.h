#pragma once

#include <string>
#include <deque>
#include <map>

#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

class Client : private boost::noncopyable
{
  boost::thread m_thread;
  volatile bool m_isStopped;

  boost::asio::io_service m_service;
  boost::asio::ip::tcp::socket m_socket;

  std::deque<std::string> m_commands;
  boost::recursive_mutex m_cmdGuard;
  
  std::map<std::string, std::string> m_headerList;
  std::string m_body;
  
  bool m_processCollectBody;

  void cycle(const std::string& serverIP, const std::string& serverPort);

public:

  Client();
  ~Client();

  bool start(const std::string& serverIP, const std::string& serverPort);
  void stop();
  
  void runIO();
  
  void sendMessage(const std::string& message, bool front = false);
};
