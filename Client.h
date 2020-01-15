#pragma once

#include <vector>
#include <deque>
#include <map>

#include <stdint.h>

#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

class Client : private boost::noncopyable
{
  boost::thread m_thread;
  volatile bool m_isStopped;

  boost::asio::io_service m_service;
  boost::asio::ip::tcp::socket m_socket;

  std::deque<std::vector<std::uint8_t>> m_commands;
  boost::recursive_mutex m_cmdGuard;
  
  std::deque<std::vector<std::uint8_t>> m_responces;
  boost::recursive_mutex m_rspGuard;

  bool m_connected;

  void cycle(const std::string& serverIP, const std::string& serverPort);

public:

  Client();
  ~Client();

  bool start(const std::string& serverIP, const std::string& serverPort);
  void stop();
  
  void runIO();
  
  void sendMessage(const std::vector<std::uint8_t>& message, bool front = false);
  std::vector<std::uint8_t> getResponce();
  
  bool hasResponce();

  bool isConnected();
};
